#version 450

// Fragment shaders have to output an RGBA value which is the pixel colour
layout(location = 0) out vec4 col;

void main() {
  col = vec4(1.0, 1.0, 1.0, 0.6);
}