#version 450

// These are variables, or "uniforms" that get passed to vertex and fragment shaders
layout(binding = 1) uniform sampler2D texSampler;

// Outputs of the vertex shader which are interpolated for each fragment (pixel)
// on the screen and then passed to the fragment shader
layout(location = 0) smooth in vec2 fragTexCoord;

// Fragment shaders have to output an RGBA value which is the pixel colour
layout(location = 0) out vec4 col;

void main() {
  col = texture(texSampler, fragTexCoord);
}