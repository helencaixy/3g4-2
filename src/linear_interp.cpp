#include <math.h>
#include "VulkanWindow.h"

void find_nearby_slices(int z, int &left_slice, int &right_slice, int &nearest, float &frac_left, float &frac_right)
  // For a particular z value, finds the two nearest CT slices and works out the fractions of each to use in linear interpolation
{
  float precise_slice;

  precise_slice = z/DELTA_Z;
  left_slice = (int) floor(precise_slice);
  right_slice = left_slice + 1;
  frac_right = precise_slice - left_slice;
  frac_left = right_slice - precise_slice;
}

void construct_yz_reslice_linear(VulkanWindow* frame)
  // Linear interpolation between the two nearest CT slices
{
  float frac_left, frac_right;
  int z, y, nearest_slice, left_slice, right_slice;
  unsigned char *left, *right;
  unsigned char *out = frame->texture_store.yz_rgba;
  unsigned char d;
  int alpha = (int)(frame->reslice_alpha * 255.0f);

  for (z=0; z<VOLUME_DEPTH; z++) {
    find_nearby_slices(z, left_slice, right_slice, nearest_slice, frac_left, frac_right);
    left = frame->data + (int) frame->x_reslice_position + left_slice * (VOLUME_WIDTH * VOLUME_HEIGHT);
    right = left + VOLUME_WIDTH * VOLUME_HEIGHT;
    for (y=0; y<VOLUME_HEIGHT; y++) {
      d = (unsigned char) (frac_left * *left + frac_right * *right);
      *out++ = frame->MapItoR[d];
      *out++ = frame->MapItoG[d];
      *out++ = frame->MapItoB[d];
      *out++ = alpha;
      left += VOLUME_WIDTH;
      right += VOLUME_WIDTH;
    }
  }
}

void construct_xz_reslice_linear(VulkanWindow* frame)
  // Linear interpolation between the two nearest CT slices
{
  float frac_left, frac_right;
  int x, z, nearest_slice, left_slice, right_slice;
  unsigned char *left, *right;
  unsigned char *out = frame->texture_store.xz_rgba;
  unsigned char d;
  int alpha = (int)(frame->reslice_alpha * 255.0f);

  for (z=0; z<VOLUME_DEPTH; z++) {
    find_nearby_slices(z, left_slice, right_slice, nearest_slice, frac_left, frac_right);
    left = frame->data + (int) frame->y_reslice_position * VOLUME_WIDTH + left_slice * (VOLUME_WIDTH * VOLUME_HEIGHT);
    right = left + VOLUME_WIDTH * VOLUME_HEIGHT;
    for (x=0; x<VOLUME_WIDTH; x++) {
      d = (unsigned char) (frac_left * *left + frac_right * *right);
      *out++ = frame->MapItoR[d];
      *out++ = frame->MapItoG[d];
      *out++ = frame->MapItoB[d];
      *out++ = alpha;
      left ++;
      right ++;
    }
  }
}
