#include <wx/wxprec.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <sstream>
#include <chrono>
#include "3g4_lab_app.h"
#include "VulkanWindow.h"
#include "VulkanException.h"

#ifdef __linux__
#include <X11/Xlib.h>
#endif

#pragma warning(disable: 28251)

VulkanWindow *mainFrame;

// Functions run before wxWidgets window begins loading
wx3g4LabApp::wx3g4LabApp()
{
  // The following lines may need commenting or uncommenting on Linux systems if the program crashes on launch
#ifdef __linux__
  //setenv("GDK_BACKEND", "x11", 1);
  XInitThreads();
#endif
}

wx3g4LabApp::~wx3g4LabApp()
{
}

bool wx3g4LabApp::OnInit()
{
  wxString filename;
  if (argc != 2 && argc != 3) {
    std::wcout << "Usage:      " << argv[0] << " [filename]" << std::endl;
    std::wcout << "Defaulting to hard coded path (may not work)" << std::endl;
    //exit(1);
    filename = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + "ct_data.dat";
  } else {
    filename = argv[1];
    std::wcout << "Loading " << filename << std::endl;
  }

  try {
    mainFrame = new VulkanWindow(NULL, "Laboratory 3G4", wxString(filename), wxDefaultPosition, wxSize(1280, 1020));
  }
  catch (VulkanException &ve) {
    std::string status = ve.GetStatus();
    std::stringstream ss;
    ss << ve.what() << "\n" << status;
    wxMessageBox(ss.str(), "Vulkan Error");
    return false;
  }
  catch (std::runtime_error &err) {
    std::stringstream ss;
    ss << "Error encountered trying to create the Vulkan canvas:\n";
    ss << err.what();
    wxMessageBox(ss.str(), "Vulkan Error");
    return false;
  }
  mainFrame->SetMinSize(wxSize(400, 400));
  mainFrame->Show(true);
  return true;
}

void wx3g4LabApp::OnIdle(wxIdleEvent &event)
{
  static unsigned long long last_time = 0;
  unsigned long long this_time;
  static int fps_update_counter = 0;
  float time_diff;

  using namespace std::chrono;
  // Update frames-per-second display
  this_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

  if (last_time == 0) last_time = this_time;
  else {
    fps_update_counter++;
    time_diff = (this_time - last_time) / 1.0E6;
    if (time_diff >= FPS_UPDATE_INTERVAL) {
      mainFrame->fps = (int)(fps_update_counter / time_diff);
      mainFrame->SetStatusText(wxString::Format("%d frames per second", mainFrame->fps), 1);
      mainFrame->tl_canvas->fps = mainFrame->fps;
      last_time = this_time;
      fps_update_counter = 0;
    }
  }

  if (mainFrame->tl_canvas->fly) mainFrame->tl_canvas->flyMove();
  mainFrame->tl_canvas->drawFrame();
  wxWakeUpIdle();
}

// Workaround to ensure all Vulkan canvases are drawn correctly on window creation.
// Forcefully redraw entire window on repeat until fly-through mode is enabled.
void wx3g4LabApp::OnXLaunchIdle(wxIdleEvent &event)
{
  if (mainFrame->tl_canvas) {
    mainFrame->Refresh();
    mainFrame->Update();
    wxWakeUpIdle();
  }
}

wxIMPLEMENT_APP(wx3g4LabApp);
