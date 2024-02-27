#pragma once

#ifdef __APPLE__

#include <vulkan/vulkan.h>

VkSurfaceKHR CreateCocoaSurface(VkInstance instance, void* handle);

#endif
