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
layout (binding = 2) uniform samplerCube skybox;

// Outputs of the vertex shader which are interpolated for each fragment (pixel)
// on the screen and then passed to the fragment shader
layout(location = 0) smooth in vec3 fragNormal;
layout(location = 1) smooth in vec3 fragPos;

// Fragment shaders have to output an RGBA value which is the pixel colour
layout(location = 0) out vec4 col;

void main()
{
  float ratio = 1.00 / 1.52;
  vec3 I = normalize(fragPos - ubo.cameraPos);
  vec3 R = refract(I, normalize(fragNormal), ratio);
  col = vec4(texture(skybox, -R).rgb, ubo.diffuseCol.a);
}