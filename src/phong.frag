#version 450 core

// Constants which are used to define the amount and colour of light
const vec3 ambientLight = vec3(0.15, 0.15, 0.15);
const vec3 diffuseLight1 = vec3(0.3, 0.3, 0.3);
const vec3 diffuseLight2 = vec3(0.55, 0.55, 0.55);
const vec3 specularLight = vec3(0.45, 0.45, 0.45);
const float roughness = 0.3; // between about 0.2 and 0.8

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

// Outputs of the vertex shader which are interpolated for each fragment (pixel)
// on the screen and then passed to the fragment shader
layout(location = 0) smooth in vec3 N;
layout(location = 1) smooth in vec3 L1;
layout(location = 2) smooth in vec3 L2;
layout(location = 3) smooth in vec3 V;

// Fragment shaders have to output an RGBA value which is the pixel colour
layout(location = 0) out vec4 col;

void main()
{
  // normalise vectors, since they have been linearly interpolated
  vec3 nN = normalize(N);
  vec3 nL1 = normalize(L1);
  vec3 nL2 = normalize(L2);
  vec3 nV = normalize(V);

  // ambient light
  vec3 ambientCol = ambientLight * ubo.diffuseCol.rgb;

  // diffuse light 
  float NdotL1 = max(dot(nN, nL1), 0.0);
  float NdotL2 = max(dot(nN, nL2), 0.0);
  vec3 diffuseCol = (NdotL1 * diffuseLight1 + NdotL2 * diffuseLight2) * ubo.diffuseCol.rgb;

  // specular light 
  float shininess = 2.0 / (roughness * roughness);
  float specularAmount = 2.0*(1.0 - roughness)*(1.0 - roughness);
  vec3 R1 = -reflect(nL1, nN);
  vec3 R2 = -reflect(nL2, nN);
  float RdotV1 = pow(max(dot(R1, nV), 0.0), shininess);
  float RdotV2 = pow(max(dot(R2, nV), 0.0), shininess);
  vec3 specularCol = (RdotV1 + RdotV2) * specularLight * specularAmount;

  // Overall colour, including opacity
  col = vec4(ambientCol + diffuseCol + specularCol, ubo.diffuseCol.a);
}
