#pragma once
#include "wx/wxprec.h"
#include "VulkanCanvas.h"
#include "isosurf.h"

// Widget identifiers
enum { 
  VA_SLIDER_ID = wxID_HIGHEST + 1,
  RO_SLIDER_ID,
  SO_SLIDER_ID,
  ST_SLIDER_ID,
  SR_SLIDER_ID,
  XY_SLIDER_ID,
  XZ_SLIDER_ID,
  YZ_SLIDER_ID,
  ISO_BUTTON_ID,
  SURFACE_CHECKBOX_ID,
  LIGHTS_CHECKBOX_ID,
  FLY_CHECKBOX_ID,
  SKYBOX_CHECKBOX_ID,
  SLICE_CHECKBOX_ID,
  INTERP_CHECKBOX_ID
};

class VulkanWindow :
    public wxFrame
{
public:
    VulkanWindow(wxWindow *parent, const wxString& title, wxString filename, const wxPoint& pos, const wxSize& size);
    virtual ~VulkanWindow();

    wxBoxSizer* m_sizer;

    VulkanCanvas *tl_canvas;
    unsigned char data[VOLUME_WIDTH * VOLUME_HEIGHT * NUM_SLICES], MapItoR[256], MapItoG[256], MapItoB[256];
    int view_angle, data_tex_width, data_tex_height, xz_tex_width, xz_tex_height, yz_tex_width, yz_tex_height, fps;
    float surface_alpha, reslice_alpha;
    float data_slice_number=0, x_reslice_position=0, y_reslice_position=0;
    unsigned int data_texture, xz_texture, yz_texture;
    bool fly, mobile_lights, show_surface, show_reslices, show_skybox;
    texture_storage texture_store;

    void construct_data_slice_image(void);
    void construct_data_slice(void);
    void construct_xz_reslice(void);
    void construct_yz_reslice(void);

    wxBoxSizer *topsizer;
    wxBoxSizer *canvas_h_sizer;
    wxBoxSizer *canvas_v1_sizer;
    wxBoxSizer *canvas_v2_sizer;
private:
    void OnCreate(wxPaintEvent& event);
    
    std::vector<point_t> points;
    std::vector<triangle_t> triangles;
    unsigned char threshold;
    int surface_resolution, n_tris;
    bool interpolate;
    VulkanCanvas *tr_canvas, *bl_canvas, *br_canvas;

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnVASlider(wxScrollEvent& event);
    void OnROSlider(wxScrollEvent& event);
    void OnSOSlider(wxScrollEvent& event);
    void OnSRSlider(wxScrollEvent& event);
    void OnSTSlider(wxScrollEvent& event);
    void OnXYSlider(wxScrollEvent& event);
    void OnXZSlider(wxScrollEvent& event);
    void OnYZSlider(wxScrollEvent& event);
    void OnSlice(wxCommandEvent& event);
    void OnSurface(wxCommandEvent& event);
    void OnLights(wxCommandEvent& event);
    void OnFly(wxCommandEvent& event);
    void OnSkybox(wxCommandEvent& event);
    void OnCalcIso(wxCommandEvent& event);
    void OnInterp(wxCommandEvent& event);
    void construct_xz_reslice_nearest_neighbour(void);
    void construct_yz_reslice_nearest_neighbour(void);
    double mesh_area(void);
    double mesh_volume(void);
    void set_threshold_colourmap(void);
    DECLARE_EVENT_TABLE()
};

