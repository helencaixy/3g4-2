#version 450

// These are variables, or "uniforms" that get passed to vertex and fragment shaders
layout (binding = 2) uniform samplerCube samplerCubeMap;

layout (location = 0) smooth in vec3 fragUVW;

layout (location = 0) out vec4 col;

void main() 
{
  col = texture(samplerCubeMap, -fragUVW);
}