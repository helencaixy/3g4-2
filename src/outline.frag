#version 450

// These are variables, or "uniforms" that get passed to vertex and fragment shaders
layout(binding = 0) uniform UniformBufferObject {
  mat4 model;       // modelling matrix
  mat4 view;        // viewing matrix
  mat4 proj;        // projection matrix
  vec4 light1Pos;   // view position of light 1
  vec4 light2Pos;   // view position of light 2
  vec3 cameraPos;   // view position of camera
  vec4 diffuseCol;  // colour and opacity of object
} ubo;

// Fragment shaders have to output an RGBA value which is the pixel colour
layout(location = 0) out vec4 col;

void main() {
  col = vec4(ubo.diffuseCol.rgb, 1.0);
}