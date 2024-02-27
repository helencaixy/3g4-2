#pragma once
#include <vector>

/* 3-d point in space data structure */
typedef struct {
  float x;
  float y;
  float z;
} point_t;

/* Triangle */
typedef struct {
  unsigned int a;
  unsigned int b;
  unsigned int c;
} triangle_t;

extern "C" {
  void isosurf(int width, int height, int num_slices, float resolution, float slice_sep, unsigned char *data, int threshold, void *points, void *normals, void *triangles);
}
