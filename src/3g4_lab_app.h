#pragma once
#include <wx/wx.h>

class wx3g4LabApp :
    public wxApp
{
public:
    wx3g4LabApp();
    virtual ~wx3g4LabApp();
    virtual bool OnInit() override;
    virtual void OnIdle(wxIdleEvent &event);
    virtual void OnXLaunchIdle(wxIdleEvent &event);
};

