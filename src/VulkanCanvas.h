#pragma once
#include "wx/wx.h"
#include "guistructs.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifdef __linux__
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#endif
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <array>
#include <optional>
#include <unordered_set>
#ifdef __linux__
#include "wxWayland.h"
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/normal.hpp>

#include "vk_mem_alloc.h"

// Constants
#define FPS_UPDATE_INTERVAL 1.0
#define VOLUME_WIDTH 350
#define VOLUME_HEIGHT 440
#define NUM_SLICES 145
#define DELTA_Z 2.439
#define PIXEL_DIMENSION 0.05
#define VOLUME_DEPTH ((int) ((NUM_SLICES - 1) * DELTA_Z) + 1)
#define ORTHO_VIEW_DEPTH 4000
#define INITIAL_DEPTH_OFFSET ORTHO_VIEW_DEPTH/2
#define FLY_SPEED 400.0
#define BACKGROUND_RGB 0.0f, 0.0f, 0.0f
#define OBJECT_RGB 0.87f, 0.80f, 0.70f

enum shader_types {
  GOURAUD_VERTEX = 8,
  GOURAUD_FRAGMENT = 9,
  PHONG_VERTEX = 10,
  PHONG_FRAGMENT = 11,
  REFLECT_VERTEX = 12,
  REFLECT_FRAGMENT = 13,
  REFRACT_VERTEX = 14,
  REFRACT_FRAGMENT = 15,
};

// Canvas identifiers
enum canvas_t { 
  CANVAS_TR = 0,
  CANVAS_BR = 1,
  CANVAS_BL = 2,
  CANVAS_TL = 3
};

// Texture identifiers - first three deliberately match canvases
enum texture_t {
  TEXTURE_DATA = 0,
  TEXTURE_XZ = 1,
  TEXTURE_YZ = 2,
  TEXTURE_SKYBOX = 3
};

// Object identifiers
enum object_t { 
  OBJECT_SLICE = 0, 
  OBJECT_OUTLINE = 1, 
  OBJECT_SKYBOX = 2, 
  OBJECT_SURFACE = 3 
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete()
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
  glm::vec3 pos;
  glm::vec2 texCoord;
  glm::vec3 normal;

  static VkVertexInputBindingDescription getBindingDescription()
  {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
  {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
  }

  bool operator==(const Vertex &other) const
  {
    return pos == other.pos && texCoord == other.texCoord;
  }
};

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
  alignas(16) glm::vec4 l1_pos;
  alignas(16) glm::vec4 l2_pos;
  alignas(16) glm::vec3 cameraPos;
  alignas(16) glm::vec4 diffuseCol;
};

struct object {
  object_t type;
  int n_indices;
  int first_index;
  texture_t texture_id;
  glm::vec3 colour;
  glm::mat4 model_matrix;
};

enum class GfxVendor {
  Generic,

  AMD,
  IntelLegacy,
  IntelNoLegacy,
  Intel,
  Nvidia,
  Apple,
  Mesa,

  MAX
};

class texture_storage {
public:
  unsigned char data_rgba[VOLUME_WIDTH * VOLUME_HEIGHT * 4], xz_rgba[VOLUME_WIDTH * VOLUME_DEPTH * 4], yz_rgba[VOLUME_HEIGHT * VOLUME_DEPTH * 4];
};


class VulkanCanvas :
  public wxPanel {
public:
  VulkanCanvas(wxWindow *pParent,
               texture_storage *texture_store,
               canvas_t type,
               wxWindowID id = wxID_ANY,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxString &name = "VulkanCanvasName");

  virtual ~VulkanCanvas() noexcept;


  bool data_updated = false, xz_updated = false, yz_updated = false, objects_updated = false, show_surface = false, show_slices = true, fly = false, mobile_lights = false, show_skybox = false, uniformBufferUpdateRequired = true, show_wireframe = false;
  float view_angle = 0, offset_x = 0, offset_y = 0, offset_z = 0, heading = 0, pitch = 0;
  std::unordered_set<int> keys_pressed = {};
  glm::mat4 view_matrix = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 l1_pos = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  glm::vec4 l2_pos = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

  glm::float32 surface_opacity = 1.0f;
  float slice_opacity = 0.5f;

  float depth_offset = -INITIAL_DEPTH_OFFSET;
  float scene_scale = 2.5;
  std::vector<float> scene_pan = { 0.0f, 0.0f };
  glm::mat4 scene_rotate = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, -1.0f, 0.0f, 0.0f,
                                     0.0f, 0.0f, -1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f);

  float fps = 25;

  std::vector<Vertex> vertices = {
    // SLICE and OUTLINE
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{(float)VOLUME_WIDTH, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{(float)VOLUME_WIDTH, (float)VOLUME_HEIGHT, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.0f, (float)VOLUME_HEIGHT, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

    {{(float)VOLUME_WIDTH, 0.0f, (float)VOLUME_DEPTH}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.0f, 0.0f, (float)VOLUME_DEPTH}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

    {{0.0f, (float)VOLUME_HEIGHT, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, (float)VOLUME_HEIGHT, (float)VOLUME_DEPTH}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{(float)VOLUME_WIDTH, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, 0.0f, (float)VOLUME_DEPTH}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    // SKYBOX
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
  };
  int fixed_vertices = 20;

  std::vector<uint32_t> indices = {
      // SLICE
      0, 1, 2, 2, 3, 0,
      0, 1, 4, 4, 5, 0,
      0, 6, 7, 7, 5, 0,
      // OUTLINE 
      0, 1, 2, 3, 0, 0xFFFFFFFF,
      8, 9, 4, 5, 8, 0xFFFFFFFF,
      10, 6, 7, 11, 10, 0xFFFFFFFF,
      // SKYBOX
      12, 13, 14, 14, 15, 12,
      16, 13, 12, 12, 17, 16,
      14, 18, 19, 19, 15, 14,
      16, 17, 19, 19, 18, 16,
      12, 15, 19, 19, 17, 12,
      13, 16, 14, 14, 16, 18,
  };
  int fixed_indices = 72;

  std::vector<object> objects = {
      { OBJECT_SLICE, 6, 0, TEXTURE_DATA, glm::vec3(0.0f, 0.0f, 1.0f), glm::mat4(1.0f) },
      { OBJECT_SLICE, 6, 6, TEXTURE_XZ, glm::vec3(1.0f, 0.0f, 0.0f), glm::mat4(1.0f) },
      { OBJECT_SLICE, 6, 12, TEXTURE_YZ, glm::vec3(0.0f, 1.0f, 0.0f), glm::mat4(1.0f) },
      { OBJECT_OUTLINE, 6, 18, TEXTURE_DATA, glm::vec3(0.0f, 0.0f, 1.0f), glm::mat4(1.0f) },
      { OBJECT_OUTLINE, 6, 24, TEXTURE_XZ, glm::vec3(1.0f, 0.0f, 0.0f), glm::mat4(1.0f) },
      { OBJECT_OUTLINE, 6, 30, TEXTURE_YZ, glm::vec3(0.0f, 1.0f, 0.0f), glm::mat4(1.0f) },
      { OBJECT_SKYBOX, 36, 36, TEXTURE_SKYBOX, glm::vec3(0.0f, 0.0f, 1.0f), glm::mat4(1.0f) }
  };
  int fixed_objects = 7;

  void flyMove(void);
  void drawFrame();
  void gui_initHandleContextFromWxWidgetsWindow(WindowHandleInfo &handleInfoOut, class wxWindow *wxw);

private:

  int num_objects;
  int num_descriptors;
  int num_uniform_buffers;
  int num_textures;

  canvas_t canvas_type;

  texture_storage *pTexture_store;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline slices3DPipeline;
  VkPipeline slices2DPipeline;
  VkPipeline sliceOutlinePipeline;
  VkPipeline solidSurfacePipeline;
  VkPipeline transparentSurfacePipelineFront;
  VkPipeline transparentSurfacePipelineBack;
  VkPipeline wireframePipeline;
  VkPipeline skyboxPipeline;

  VkCommandPool commandPool;

  VkImage depthImage;
  VmaAllocation depthImageAllocation;
  VkImageView depthImageView;

  std::vector<VkImage> textureImage;
  std::vector<VmaAllocation> textureImageAllocation;
  std::vector<VkImageView> textureImageView;
  std::vector<VkSampler> textureSampler;

  VkBuffer vertexBuffer;
  VmaAllocation vertexBufferAllocation;
  VkBuffer indexBuffer;
  VmaAllocation indexBufferAllocation;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VmaAllocation> uniformBuffersAllocations;
  std::vector<void *> uniformBuffersMapped;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;

  VmaAllocator allocator;

  bool framebufferResized = false;

  WindowInfo g_window_info{};

  int surface_vertex_shader;
  int surface_fragment_shader;

  void initVulkan(const wxSize &size);
  void cleanupSwapChain();
  void cleanup();
  void recreateSwapChain();
  void createInstance();
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createAllocator();
  void createSwapChain(const wxSize &size);
  void createImageViews();
  void createRenderPass();
  void createDescriptorSetLayout();
  void createSlices3DPipeline();
  void createSlices2DPipeline();
  void createSliceOutlinePipeline();
  void createSolidSurfacePipeline();
  void createTransparentSurfacePipelineFront();
  void createTransparentSurfacePipelineBack();
  void createWireframePipeline();
  void createSkyboxPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createDepthResources();
  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  VkFormat findDepthFormat();
  bool hasStencilComponent(VkFormat format);
  void createTextureImage(uint32_t image_id, uint32_t width, uint32_t height, unsigned char *image_pixels, int layers = 1);
  void updateTextureImage(uint32_t image_id, uint32_t width, uint32_t height, unsigned char *image_pixels, int layers = 1);
  void createTextureImageView(uint32_t image_id, int layers = 1);
  void createTextureSampler(uint32_t image_id);
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int layers = 1);
  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VmaAllocation &alloc, int layers = 1);
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int layers = 1);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layers = 1);
  void createVertexBuffer();
  void updateVertexBuffer();
  void createIndexBuffer();
  void updateIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void updateDescriptorSets();
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VmaAllocation &bufferAllocation);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void createCommandBuffers();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void createSyncObjects();
  void updateUniformBuffer(uint32_t currentImage, uint32_t object_id);
  VkShaderModule createShaderModule(const std::vector<char> &code);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const wxSize &size);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();
  std::vector<char> readFile(const std::string &filename);
  glm::mat4 modellingTransformation();
  glm::mat4 viewingTransformation();
  void loadCubemapFromFiles(const std::vector<std::string> filenames, unsigned char *output);
  void selectShaders(void);
  void findFiles(void);

  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnResize(wxSizeEvent &event);
  void OnPaintException(const std::string &msg);
  void OnEraseBG(wxEraseEvent &evt);
  void OnMouse(wxMouseEvent &event);
  void OnKeyDown(wxKeyEvent &event);
  void OnKeyUp(wxKeyEvent &event);
  void OnKillFocus(wxFocusEvent &event);

  WindowInfo &gui_getWindowInfo();
  VkSurfaceKHR CreateFramebufferSurface(VkInstance instance, struct WindowHandleInfo &windowInfo);
#ifdef _WIN32
  VkSurfaceKHR CreateWinSurface(VkInstance instance, HWND hwindow);
#endif
#ifdef __linux__
  VkSurfaceKHR CreateWaylandSurface(VkInstance instance, wl_display *wl_display, wl_surface *wl_surface);
  VkSurfaceKHR CreateXlibSurface(VkInstance instance, Display *dpy, Window window);
  std::unique_ptr<wxWlSubsurface> m_subsurface;
#endif

  DECLARE_EVENT_TABLE()

};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
