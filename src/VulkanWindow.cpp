#include "VulkanWindow.h"
#include "VulkanException.h"
#include "3g4_lab_app.h"
#include "linear_interp.h"
#include <wx/file.h>
#include <wx/log.h> 

BEGIN_EVENT_TABLE(VulkanWindow, wxFrame)
EVT_MENU(wxID_EXIT, VulkanWindow::OnQuit)
EVT_MENU(wxID_ABOUT, VulkanWindow::OnAbout)
EVT_COMMAND_SCROLL(VA_SLIDER_ID, VulkanWindow::OnVASlider)
EVT_COMMAND_SCROLL(RO_SLIDER_ID, VulkanWindow::OnROSlider)
EVT_COMMAND_SCROLL(SO_SLIDER_ID, VulkanWindow::OnSOSlider)
EVT_COMMAND_SCROLL(XY_SLIDER_ID, VulkanWindow::OnXYSlider)
EVT_COMMAND_SCROLL(XZ_SLIDER_ID, VulkanWindow::OnXZSlider)
EVT_COMMAND_SCROLL(YZ_SLIDER_ID, VulkanWindow::OnYZSlider)
EVT_COMMAND_SCROLL(ST_SLIDER_ID, VulkanWindow::OnSTSlider)
EVT_COMMAND_SCROLL(SR_SLIDER_ID, VulkanWindow::OnSRSlider)
EVT_CHECKBOX(INTERP_CHECKBOX_ID, VulkanWindow::OnInterp)
EVT_CHECKBOX(LIGHTS_CHECKBOX_ID, VulkanWindow::OnLights)
EVT_CHECKBOX(SLICE_CHECKBOX_ID, VulkanWindow::OnSlice)
EVT_CHECKBOX(SURFACE_CHECKBOX_ID, VulkanWindow::OnSurface)
EVT_CHECKBOX(FLY_CHECKBOX_ID, VulkanWindow::OnFly)
EVT_CHECKBOX(SKYBOX_CHECKBOX_ID, VulkanWindow::OnSkybox)
EVT_BUTTON(ISO_BUTTON_ID, VulkanWindow::OnCalcIso)
EVT_CLOSE(VulkanWindow::OnClose)
END_EVENT_TABLE()

VulkanWindow::VulkanWindow(wxWindow *parent, const wxString &title, wxString filename, const wxPoint &pos, const wxSize &size)
  : wxFrame(parent, wxID_ANY, title, pos, size)//, m_canvas(nullptr)
{
  wxLogDebug("Loading data");

  // Load the data
  wxFile fid;
  bool read_in = false;
  if (wxFile::Access(filename, wxFile::read)) {
    if (fid.Open(filename)) {
      if (fid.Length() == VOLUME_WIDTH * VOLUME_HEIGHT * NUM_SLICES) {
        fid.Read(data, fid.Length());
        read_in = true;
      }
    }
  }
  if (!read_in) {
    std::cout << "Invalid data file" << std::endl;
    wxLogDebug("Invalid data file");
    exit(1);
  }

  // Lay out the canvases and controls
  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(wxID_ABOUT, "&About");
  fileMenu->Append(wxID_EXIT, "&Quit");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  SetMenuBar(menuBar);
  wxStatusBar *statusBar = CreateStatusBar(2);
  int widths[2] = { -2, -1 };
  statusBar->SetStatusWidths(2, widths);
  int styles[2] = { wxSB_SUNKEN, wxSB_SUNKEN };
  statusBar->SetStatusStyles(2, styles);

  // The window is layed out using topsizer, inside which are lower level windows and sizers for the canvases and controls
  topsizer = new wxBoxSizer(wxHORIZONTAL); SetSizer(topsizer);
  canvas_h_sizer = new wxBoxSizer(wxHORIZONTAL);
  canvas_v1_sizer = new wxBoxSizer(wxVERTICAL);
  canvas_v2_sizer = new wxBoxSizer(wxVERTICAL);
  wxScrolledWindow *controlwin = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxHSCROLL | wxVSCROLL);
  topsizer->Add(canvas_h_sizer, 1, wxEXPAND);
  topsizer->Add(controlwin, 0, wxEXPAND);
  canvas_h_sizer->Add(canvas_v1_sizer, 1, wxEXPAND);
  canvas_h_sizer->Add(canvas_v2_sizer, 1, wxEXPAND);

  // Create a sizer to manage the contents of the scrolled control window
  wxBoxSizer *control_sizer = new wxBoxSizer(wxVERTICAL);
  controlwin->SetSizer(control_sizer);
  controlwin->SetScrollRate(10, 10);
  int controlsize, slidersize;
#ifdef WIN32
  controlsize = 162 * this->GetDPIScaleFactor();
  slidersize = 140 * this->GetDPIScaleFactor();
#else
  controlsize = 210;
  slidersize = 150;
#endif
  controlwin->SetMinSize(wxSize(controlsize, -1));

  // Create the rendering controls and add them to the control sizer
  show_reslices = true;
  wxCheckBox *slice_cb = new wxCheckBox(controlwin, SLICE_CHECKBOX_ID, "show slice and reslices");
  control_sizer->Add(slice_cb, 0, wxALL, 5);
  slice_cb->SetValue(show_reslices);

  show_surface = false;
  wxCheckBox *surface_cb = new wxCheckBox(controlwin, SURFACE_CHECKBOX_ID, "show surface");
  control_sizer->Add(surface_cb, 0, wxALL, 5);
  surface_cb->SetValue(show_surface);

  show_skybox = false;
  wxCheckBox *skybox_cb = new wxCheckBox(controlwin, SKYBOX_CHECKBOX_ID, "show skybox");
  control_sizer->Add(skybox_cb, 0, wxALL, 5);
  skybox_cb->SetValue(show_skybox);

  mobile_lights = false;
  wxCheckBox *lights_cb = new wxCheckBox(controlwin, LIGHTS_CHECKBOX_ID, "lights move with viewer");
  control_sizer->Add(lights_cb, 0, wxALL, 5);
  lights_cb->SetValue(mobile_lights);

  fly = false;
  wxCheckBox *fly_cb = new wxCheckBox(controlwin, FLY_CHECKBOX_ID, "fly-through");
  control_sizer->Add(fly_cb, 0, wxALL, 5);
  fly_cb->SetValue(fly);

  view_angle = 0;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "view angle (degrees)"), 0, wxTOP | wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, VA_SLIDER_ID, view_angle, 0, 135, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL | wxSL_VALUE_LABEL), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

  reslice_alpha = 0.5;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "reslice opacity"), 0, wxTOP | wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, RO_SLIDER_ID, (int)round(reslice_alpha * 10.0f), 0, 10, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL | wxSL_VALUE_LABEL), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
  surface_alpha = 1.0;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "surface opacity"), 0, wxTOP | wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, SO_SLIDER_ID, (int)round(surface_alpha * 10.0f), 0, 10, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL | wxSL_VALUE_LABEL), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

  // Separate the rendering controls from the reslice controls
#ifdef WIN32
  control_sizer->AddSpacer(54);
#else
  control_sizer->AddSpacer(34);
#endif

  // Create the reslice controls and add them to the control sizer
  interpolate = true;
  wxCheckBox *interp_cb = new wxCheckBox(controlwin, INTERP_CHECKBOX_ID, "linear interpolation");
  control_sizer->Add(interp_cb, 0, wxALL, 5);
  interp_cb->SetValue(interpolate);

  data_slice_number = NUM_SLICES / 2.0;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "x-y data slice"), 0, wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, XY_SLIDER_ID, data_slice_number, 0, NUM_SLICES - 1, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL), 0, wxALL, 5);

  y_reslice_position = VOLUME_HEIGHT / 2.0;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "x-z reslice"), 0, wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, XZ_SLIDER_ID, y_reslice_position, 0, VOLUME_HEIGHT - 1, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL), 0, wxALL, 5);

  x_reslice_position = VOLUME_WIDTH / 2.0;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "y-z reslice"), 0, wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, YZ_SLIDER_ID, x_reslice_position, 0, VOLUME_WIDTH - 1, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL), 0, wxALL, 5);

  // Separate the reslice controls from the isosurface controls
#ifdef WIN32
  control_sizer->AddSpacer(54);
#else
  control_sizer->AddSpacer(34);
#endif

  // Create the isosurface controls and add them to the control sizer
  threshold = 255;
  set_threshold_colourmap();
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "surface threshold"), 0, wxTOP | wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, ST_SLIDER_ID, threshold, 0, 255, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL | wxSL_VALUE_LABEL), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

  surface_resolution = 5;
  control_sizer->Add(new wxStaticText(controlwin, wxID_ANY, "surface resolution"), 0, wxTOP | wxLEFT, 10);
  control_sizer->Add(new wxSlider(controlwin, SR_SLIDER_ID, surface_resolution, 1, 40, wxDefaultPosition, wxSize(slidersize, -1), wxSL_HORIZONTAL | wxSL_VALUE_LABEL), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
  control_sizer->Add(new wxButton(controlwin, ISO_BUTTON_ID, "Calculate isosurface"), 0, wxALIGN_CENTRE | wxALL, 5);

  Bind(wxEVT_PAINT, &VulkanWindow::OnCreate, this);
}


VulkanWindow::~VulkanWindow()
{
}

void VulkanWindow::OnCreate(wxPaintEvent &event)//wxWindowCreateEvent& event)//(wxPaintEvent& event)
{
  // Create the four canvases and add them to the relevant sizer
  tl_canvas = new VulkanCanvas(this, &texture_store, CANVAS_TL, wxID_ANY);//, CANVAS_TL);
  tr_canvas = new VulkanCanvas(this, &texture_store, CANVAS_TR, wxID_ANY);//, CANVAS_TR);
  bl_canvas = new VulkanCanvas(this, &texture_store, CANVAS_BL, wxID_ANY);//, CANVAS_BL);
  br_canvas = new VulkanCanvas(this, &texture_store, CANVAS_BR, wxID_ANY);//, CANVAS_BR);
  canvas_v1_sizer->Add(tl_canvas, 1, wxEXPAND | wxALL, 2);
  canvas_v1_sizer->Add(bl_canvas, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2);
  canvas_v2_sizer->Add(tr_canvas, 1, wxEXPAND | wxTOP | wxBOTTOM, 2);
  canvas_v2_sizer->Add(br_canvas, 1, wxEXPAND | wxBOTTOM, 2);
  SendSizeEvent();

  construct_data_slice_image();
  construct_xz_reslice();
  construct_yz_reslice();
  for (std::vector<object>::iterator o = tl_canvas->objects.begin(); o != tl_canvas->objects.end(); o++) {
    if ((o->type == OBJECT_SLICE) || (o->type == OBJECT_OUTLINE)) {
      if (o->texture_id == TEXTURE_DATA) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, data_slice_number * (float)DELTA_Z));
      } else if (o->texture_id == TEXTURE_XZ) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y_reslice_position, 0.0f));
      } else if (o->texture_id == TEXTURE_YZ) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_reslice_position, 0.0f, 0.0f));
      }
    }
  }
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
  tr_canvas->drawFrame();
  bl_canvas->drawFrame();
  br_canvas->drawFrame();

  Unbind(wxEVT_PAINT, &VulkanWindow::OnCreate, this);

#ifdef __linux__
  WindowHandleInfo handle;
  tl_canvas->gui_initHandleContextFromWxWidgetsWindow(handle, this);
  if (handle.backend == WindowHandleInfo::Backend::X11) {
    wxTheApp->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wx3g4LabApp::OnXLaunchIdle));
  }
#endif
}

void VulkanWindow::OnClose(wxCloseEvent &event)
// Called when application is closed down (including window manager close)
{
  Destroy();
  exit(0); // a workaround for a wxWidgets bug, should be able to remove this once the wxWidgets bug is fixed
}

void VulkanWindow::OnQuit(wxCommandEvent &event)
// Callback for the "Quit" menu item
{
  Close(true);
}

void VulkanWindow::OnAbout(wxCommandEvent &event)
// Callback for the "About" menu item
{
  wxMessageDialog about(this, "Laboratory 3G4 (wxWidgets / Vulkan version)\nAndrew Gee - January 2016\nChristopher Chivers - 2022/23", "About Laboratory 3G4", wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void VulkanWindow::OnROSlider(wxScrollEvent &event)
// Callback for the reslice opacity slider
{
  reslice_alpha = (float)event.GetPosition() / 10.0f;
  construct_data_slice_image();
  construct_xz_reslice();
  construct_yz_reslice();
  tl_canvas->slice_opacity = reslice_alpha;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnSOSlider(wxScrollEvent &event)
// Callback for the surface opacity slider
{
  surface_alpha = (float)event.GetPosition() / 10.0f;
  tl_canvas->surface_opacity = surface_alpha;
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnSRSlider(wxScrollEvent &event)
// Callback for the surface resolution slider
{
  surface_resolution = event.GetPosition();
}

void VulkanWindow::OnSTSlider(wxScrollEvent &event)
// Callback for the surface threshold slider
{
  threshold = event.GetPosition();
  set_threshold_colourmap();
  construct_data_slice_image();
  construct_xz_reslice();
  construct_yz_reslice();
  tl_canvas->drawFrame();
  tr_canvas->drawFrame();
  bl_canvas->drawFrame();
  br_canvas->drawFrame();
}

void VulkanWindow::OnVASlider(wxScrollEvent &event)
// Callback for the view angle slider
{
  view_angle = event.GetPosition();
  tl_canvas->view_angle = (float)view_angle;
  //tl_canvas->configured = false; // this will cause the projection matrix to be reconfigured
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnXYSlider(wxScrollEvent &event)
// Callback for the XY slider
{
  data_slice_number = event.GetPosition();
  construct_data_slice_image();
  for (std::vector<object>::iterator o = tl_canvas->objects.begin(); o != tl_canvas->objects.end(); o++) {
    if ((o->type == OBJECT_SLICE) || (o->type == OBJECT_OUTLINE)) {
      if (o->texture_id == TEXTURE_DATA) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, data_slice_number * (float)DELTA_Z));
      }
    }
  }
  tl_canvas->uniformBufferUpdateRequired;
  tl_canvas->drawFrame();
  tr_canvas->drawFrame();
}

void VulkanWindow::OnXZSlider(wxScrollEvent &event)
// Callback for the XZ slider
{
  y_reslice_position = event.GetPosition();
  construct_xz_reslice();
  for (std::vector<object>::iterator o = tl_canvas->objects.begin(); o != tl_canvas->objects.end(); o++) {
    if ((o->type == OBJECT_SLICE) || (o->type == OBJECT_OUTLINE)) {
      if (o->texture_id == TEXTURE_XZ) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y_reslice_position, 0.0f));
      }
    }
  }
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
  br_canvas->drawFrame();
}

void VulkanWindow::OnYZSlider(wxScrollEvent &event)
// Callback for the YZ slider
{
  x_reslice_position = event.GetPosition();
  construct_yz_reslice();
  for (std::vector<object>::iterator o = tl_canvas->objects.begin(); o != tl_canvas->objects.end(); o++) {
    if ((o->type == OBJECT_SLICE) || (o->type == OBJECT_OUTLINE)) {
      if (o->texture_id == TEXTURE_YZ) {
        o->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_reslice_position, 0.0f, 0.0f));
      }
    }
  }
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
  bl_canvas->drawFrame();
}

void VulkanWindow::OnSlice(wxCommandEvent &event)
// Callback for the slice/reslice checkbox
{
  show_reslices = event.IsChecked();
  tl_canvas->show_slices = show_reslices;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnLights(wxCommandEvent &event)
// Callback for the mobile lights checkbox
{
  mobile_lights = event.IsChecked();
  tl_canvas->mobile_lights = mobile_lights;
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnSurface(wxCommandEvent &event)
// Callback for the surface checkbox
{
  show_surface = event.IsChecked();
  tl_canvas->show_surface = show_surface;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnFly(wxCommandEvent &event)
// Callback for the fly checkbox
{
  fly = event.IsChecked();
  tl_canvas->fly = fly;
  if (fly) {
    tl_canvas->heading = 0;
    tl_canvas->pitch = 0;
    tl_canvas->offset_x = 0;
    tl_canvas->offset_y = 0;
    tl_canvas->offset_z = 0;
    //tl_canvas->scene_rotate = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
    //    0.0f, -1.0f, 0.0f, 0.0f,
    //    0.0f, 0.0f, -1.0f, 0.0f,
    //    0.0f, 0.0f, 0.0f, 1.0f);
    //tl_canvas->scene_scale = 2.5;
    tl_canvas->scene_pan = { 0.0f, 0.0f };
    tl_canvas->keys_pressed.clear();
    wxTheApp->Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wx3g4LabApp::OnXLaunchIdle));
    wxTheApp->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wx3g4LabApp::OnIdle));
    SetStatusText("Estimating frame rate", 1);
    fps = 5000; // initial frame per second estimate
  } else {
    //tl_canvas->scene_rotate = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
    //    0.0f, -1.0f, 0.0f, 0.0f,
    //    0.0f, 0.0f, -1.0f, 0.0f,
    //    0.0f, 0.0f, 0.0f, 1.0f);
    //tl_canvas->scene_scale = 2.5;
    tl_canvas->scene_pan = { 0.0f, 0.0f };
    wxTheApp->Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wx3g4LabApp::OnIdle));
    SetStatusText(' ', 1);
  }
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnSkybox(wxCommandEvent &event)
// Callback for the skybox checkbox
{
  show_skybox = event.IsChecked();
  tl_canvas->show_skybox = show_skybox;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnCalcIso(wxCommandEvent &event)
// Callback for the calculate isosurface button
{
  std::vector<point_t> normals;

  //tl_canvas->SetCurrent(*tl_canvas->context);
  isosurf(VOLUME_WIDTH, VOLUME_HEIGHT, NUM_SLICES, (float)surface_resolution, DELTA_Z, data, threshold, (void *)&points, (void *)&normals, (void *)&triangles);
  SetStatusText(wxString::Format("Surface has %li vertices and %li triangles, area=%.3lf cm2, volume=%.3lf ml", (long)points.size(),
                (long)triangles.size(), mesh_area() * PIXEL_DIMENSION * PIXEL_DIMENSION, mesh_volume() * PIXEL_DIMENSION * PIXEL_DIMENSION * PIXEL_DIMENSION));

  // This removes any previous surface 
  tl_canvas->vertices.resize(tl_canvas->fixed_vertices);
  tl_canvas->indices.resize(tl_canvas->fixed_indices);
  tl_canvas->objects.resize(tl_canvas->fixed_objects);

  if (triangles.size() > 0) {
    for (long i = 0; i < points.size(); i++) {
      Vertex vertex = { {points[i].x, points[i].y, points[i].z}, {0.0f, 0.0f}, {normals[i].x, normals[i].y, normals[i].z} };
      tl_canvas->vertices.push_back(vertex);
    }
    // We have to add fixed_vertices to these indices, since there are already some 
    // (non-surface) vertices in this list
    for (long i = 0; i < triangles.size(); i++) {
      tl_canvas->indices.push_back(triangles[i].a + tl_canvas->fixed_vertices);
      tl_canvas->indices.push_back(triangles[i].b + tl_canvas->fixed_vertices);
      tl_canvas->indices.push_back(triangles[i].c + tl_canvas->fixed_vertices);
    }
    normals.clear();

    tl_canvas->objects.push_back({ OBJECT_SURFACE, (int)triangles.size() * 3 , tl_canvas->fixed_indices, (texture_t)0, glm::vec3(OBJECT_RGB), glm::mat4(1.0f)});
  }

  tl_canvas->objects_updated = true;
  tl_canvas->uniformBufferUpdateRequired = true;
  tl_canvas->drawFrame();
}

void VulkanWindow::OnInterp(wxCommandEvent &event)
// Callback for the linear interpolation checkbox
{
  interpolate = event.IsChecked();
  construct_xz_reslice();
  construct_yz_reslice();
  tl_canvas->drawFrame();
  bl_canvas->drawFrame();
  br_canvas->drawFrame();
}

void VulkanWindow::set_threshold_colourmap(void)
// Populates the colourmaps for the given threshold
{
  int i;

  for (i = 0; i < 256; i++) {
    if (i > threshold) {
      MapItoR[i] = 255; MapItoG[i] = 0; MapItoB[i] = 255;
    } else {
      MapItoR[i] = i; MapItoG[i] = i; MapItoB[i] = i;
    }
  }
}

void VulkanWindow::construct_data_slice_image(void)
// Construct the data slice image
{

  construct_data_slice();

  // Bind the new reslice image to the texture map in the view window
  tl_canvas->data_updated = true;
  tr_canvas->data_updated = true;
}

void VulkanWindow::construct_xz_reslice(void)
// Construct the x-z reslice image
{
  if (interpolate) construct_xz_reslice_linear(this);
  else construct_xz_reslice_nearest_neighbour();
  tl_canvas->xz_updated = true;
  br_canvas->xz_updated = true;
}

void VulkanWindow::construct_yz_reslice(void)
// Construct the y-z reslice image
{
  if (interpolate) construct_yz_reslice_linear(this);
  else construct_yz_reslice_nearest_neighbour();
  tl_canvas->yz_updated = true;
  bl_canvas->yz_updated = true;
}

double VulkanWindow::mesh_volume(void)

// Write a function to calculate the volume enclosed by the isosurface.
// The function should return the volume, instead of the default value (zero) 
// that it currently returns. The units should be cubic pixels.
{
  point_t a, b, c;
  double x, y, z, v, volume;
  int t;

  volume = 0.0;

  for (t = 0; t < triangles.size(); t++) {
    a = points[triangles[t].a];
    b = points[triangles[t].b];
    c = points[triangles[t].c];

    // Calculate a x b
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;

    // Calculate c . (a x b)
    v = c.x * x + c.y * y + c.z * z;

    // Volume of tetrahedron is 1/6 * [ c . (a x b) ]
    volume += v / 6.0;
  }

  return volume;
}

double VulkanWindow::mesh_area(void)
// Calculates surface area of mesh in units of square pixels - note area of triangle abc = 0.5 * | ab x ac |
{
  point_t a, b, c, ab, ac;
  double x, y, z, area;
  int t;

  area = 0.0;

  for (t = 0; t < triangles.size(); t++) {
    a = points[triangles[t].a];
    b = points[triangles[t].b];
    c = points[triangles[t].c];

    // Calculate ab and ac
    ab.x = b.x - a.x; ab.y = b.y - a.y; ab.z = b.z - a.z;
    ac.x = c.x - a.x; ac.y = c.y - a.y; ac.z = c.z - a.z;

    // Calculate ab x ac
    x = ab.y * ac.z - ab.z * ac.y;
    y = ab.z * ac.x - ab.x * ac.z;
    z = ab.x * ac.y - ab.y * ac.x;

    // Area is 1/2 | ab x ac |
    area += sqrt(x * x + y * y + z * z) / 2.0;
  }

  return area;
}
