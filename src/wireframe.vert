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

// Input variables which are different for each vertex in the object
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNormal;

void main() {
  // gl_Position is a standard output variable which we always need
  // We would normally pass pre-multiplied matrices via the ubo
  // Make the wireframe position just a bit outside the surface so we can see it
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertPos + 0.2 * vertNormal, 1.0);
}