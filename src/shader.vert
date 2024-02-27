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
layout(location = 1) in vec2 vertTexCoord;

// Output variables which will be interpolated and passed to the fragment shader
layout(location = 0) smooth out vec2 fragTexCoord;

void main() {
  // Copy through texture coord
  fragTexCoord = vertTexCoord;

  // gl_Position is a standard output variable which we always need
  // We would normally pass pre-multiplied matrices via the ubo
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertPos, 1.0);
}