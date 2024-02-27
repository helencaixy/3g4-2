#include "VulkanWindow.h"
#include "VulkanCanvas.h"

// Construct the data slice image
void VulkanWindow::construct_data_slice(void)
{
  int x, y, data_index, slice_index, alpha;

  // Set alpha (range 0 - 255) from reslice_alpha (range 0 - 1)
  alpha = (int)(reslice_alpha * 255.0f);

  // Simply extract the appropriate slice from the 3D data
  slice_index = 0;
  data_index = (int)data_slice_number * VOLUME_WIDTH * VOLUME_HEIGHT;
  for (y = 0; y < VOLUME_HEIGHT; y++) {
    for (x = 0; x < VOLUME_WIDTH; x++) {
      texture_store.data_rgba[slice_index++] = MapItoR[data[data_index]];
      texture_store.data_rgba[slice_index++] = MapItoG[data[data_index]];
      texture_store.data_rgba[slice_index++] = MapItoB[data[data_index]];
      texture_store.data_rgba[slice_index++] = alpha;
      data_index++;
    }
  }
}

// EXERCISE 1
// This function currently constructs a blank, transparent x-z reslice. Modify it so
// it constructs a valid reslice according to the current y_reslice_position, with
// transparency given by reslice_alpha. Use nearest neighbour interpolation.
void VulkanWindow::construct_xz_reslice_nearest_neighbour(void)
{
  int x, z, nearest_slice, data_index, reslice_index, alpha;

  // Set alpha (range 0 - 255) from reslice_alpha (range 0 - 1)
  alpha = 0;

  // Construct xz_rgba reslice image from data
  reslice_index = 0;
  for (z = 0; z < VOLUME_DEPTH; z++) {
    // nearest_slice = ...
    // data_index = ...
    for (x = 0; x < VOLUME_WIDTH; x++) {
      texture_store.xz_rgba[reslice_index++] = MapItoR[0];
      texture_store.xz_rgba[reslice_index++] = MapItoG[0];
      texture_store.xz_rgba[reslice_index++] = MapItoB[0];
      texture_store.xz_rgba[reslice_index++] = alpha;
      // data_index = ...
    }
  }
}

// EXERCISE 1
// This function currently constructs a blank, transparent y-z reslice. Modify it so
// it constructs a valid reslice according to the current x_reslice_position, with
// transparency given by reslice_alpha. Use nearest neighbour interpolation.
void VulkanWindow::construct_yz_reslice_nearest_neighbour(void)
{
  int z, y, nearest_slice, data_index, reslice_index, alpha;

  // Set alpha (range 0 - 255) from reslice_alpha (range 0 - 1)
  alpha = 0;

  // Construct xz_rgba reslice image from data
  reslice_index = 0;
  for (z = 0; z < VOLUME_DEPTH; z++) {
    // nearest_slice = ...
    // data_index = ...
    for (y = 0; y < VOLUME_HEIGHT; y++) {
      texture_store.yz_rgba[reslice_index++] = MapItoR[0];
      texture_store.yz_rgba[reslice_index++] = MapItoG[0];
      texture_store.yz_rgba[reslice_index++] = MapItoB[0];
      texture_store.yz_rgba[reslice_index++] = alpha;
      // data_index = ...
    }
  }
}

// The modelling matrix is affected by mouse movement in the normal mode when
// fly-through is not enabled. Each of the commands post-multiplies the current
// matrix, so they are in reverse order of application to the original model.
glm::mat4 VulkanCanvas::modellingTransformation()
{
  glm::mat4 matrix(1.0f);

  matrix = glm::translate(matrix, glm::vec3(scene_pan[0], scene_pan[1], 0.0f));
  matrix = matrix * scene_rotate;
  matrix = glm::scale(matrix, glm::vec3(scene_scale, scene_scale, scene_scale));
  matrix = glm::translate(matrix, glm::vec3(-VOLUME_WIDTH / 2.0, -VOLUME_HEIGHT / 2.0, -VOLUME_DEPTH / 2.0));

  return matrix;
}

// EXERCISE 2
// Adjust the view matrix to represent the current displacement between the viewpoint
// and the scene. The translational displacement is (offset_x, offset_y, offset_z), while
// the rotation around the y-axis is 'heading', and rotation around the x-axis is 'pitch'.
// You may use the functions glm::rotate and glm::translate
// glm::rotate(glm::mat4 m, float angle, glm::vec3 axis)
// glm::translate(glm::mat4 m, glm::vec3 v)
// (see the modelling transformation above for some examples of these)
// 'fly' is true if the 'fly-through' option is checked
glm::mat4 VulkanCanvas::viewingTransformation()
{
  glm::mat4 matrix(1.0f);

  if (fly) {
    // matrix = ...
  }

  matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -depth_offset));

  return matrix;
}

// EXERCISE 3
// Select which shaders to use when rendering the surface.
//
// Available Shader Types:
// Use the appropriate Shader IDs in the selectShaders() function.
//
// Shader Type  Vertex Shader ID  Fragment Shader ID
// -----------  ----------------  ------------------
// Phong        PHONG_VERTEX      PHONG_FRAGMENT
// Reflection   REFLECT_VERTEX    REFLECT_FRAGMENT
// Refraction   REFRACT_VERTEX    REFRACT_FRAGMENT
// Gouraud      GOURAUD_VERTEX    GOURAUD_FRAGMENT
void VulkanCanvas::selectShaders(void)
{
  surface_vertex_shader = PHONG_VERTEX;
  surface_fragment_shader = PHONG_FRAGMENT;
}
