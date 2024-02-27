#version 450 core

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

// Output variables which will be interpolated and passed to the fragment shader
layout(location = 0) smooth out vec3 N;
layout(location = 1) smooth out vec3 L1;
layout(location = 2) smooth out vec3 L2;
layout(location = 3) smooth out vec3 V;

void main()
{
  // Get point, view and normal in world coordinates
  // We would normally pass pre-multiplied matrices via the ubo
  mat4 modelViewMtx = ubo.view * ubo.model;
  vec3 wPos = (modelViewMtx * vec4(vertPos, 1.0)).xyz;
  V = -wPos;
  N = normalize(mat3(modelViewMtx) * vertNormal);

  // Get light directions in world coordinates
  // If w == 0.0 then it is a directional light, else positional
  if (ubo.light1Pos.w == 0.0) {
    L1 = normalize(ubo.light1Pos.xyz);
  } else {
    L1 = ubo.light1Pos.xyz - wPos;
  }
  if (ubo.light2Pos.w == 0.0) {
    L2 = normalize(ubo.light2Pos.xyz);
  } else {
    L2 = ubo.light2Pos.xyz - wPos;
  }

  // gl_Position is a standard output variable which we always need
  gl_Position = ubo.proj * modelViewMtx * vec4(vertPos, 1.0);
}      