
#ifndef GROUBIKS_VULKAN_CONTEXT_H
#define GROUBIKS_VULKAN_CONTEXT_H

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/vector.h>
#include <groubiks/utility/string.h>
#include <groubiks/utility/macros.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

typedef vector_t PhysicalDeviceVector_t;
typedef vector_t StringVector_t;
typedef vector_t RawStringVector_t;
typedef vector_t QueueVector_t;
typedef vector_t SurfaceVector_t;
typedef vector_t ImageVector_t;
typedef vector_t FrameBufferVector_t;
typedef int Renderer_result_t;

typedef struct {
    VkShaderModule m_vertex;
    VkShaderModule m_fragment;
    VkPipelineLayout m_layout;
} VulkanShaderManager_t;

typedef struct {
    VkViewport m_viewport;
    VkRect2D m_scissors;
} VulkanViewManager_t;

typedef struct {
    VkPhysicalDevice m_physical_device;
    VkDevice m_logical_device;
    QueueVector_t m_queues;
} VulkanDeviceInstance_t;

typedef vector_t DeviceInstanceVector_t;

typedef struct {
    PhysicalDeviceVector_t m_available_devices;
    DeviceInstanceVector_t m_device_instances;
} VulkanDeviceManager_t;

typedef struct {
    RawStringVector_t m_validationlayers;
    RawStringVector_t m_extensions;
    VkDebugUtilsMessengerEXT m_debug_messenger;
} VulkanExtensionManager_t;

typedef struct {
    SurfaceVector_t m_surfaces;
    ImageVector_t m_images;
    FrameBufferVector_t m_framebuffers;
    VkRenderPass m_renderpass;

    VkCommandPool m_commandpool;
    VkCommandBuffer m_commandbuffer;

    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchain_format;
    VkExtent2D m_swapchain_extent
} VulkanPresentationManager_t;

typedef struct {
    VkInstance m_instance;
    VulkanExtensionManager_t m_ext_mngr;
    VulkanDeviceManager_t m_device_mngr;
    VulkanPresentationManager_t m_presentation_mngr;
} VulkanContext_t;

/** 
 * @brief initializes the applications interface with the Vulkan-API.
 *        should be cleaned up with DestroyVulkanContext() after usage or on error.
 */
Renderer_result_t CreateVulkanContext(VulkanContext_t* ctx);
Renderer_result_t VulkanContext_GetDevices(VulkanContext_t* ctx);

Renderer_result_t CreateVulkanExtensionManager(VulkanExtensionManager_t* ext_mngr);
Renderer_result_t VulkanExtensionManager_VerifyValidationLayers(VulkanExtensionManager_t* ext_mngr);
Renderer_result_t VulkanExtensionManager_VerifyExtensions(VulkanExtensionManager_t* ext_mngr);

Renderer_result_t CreateVulkanDeviceManager(VulkanDeviceManager_t* dvc_mngr);
Renderer_result_t VulkanDeviceManager_PickSuitableDevice(VulkanDeviceManager_t* dvc_mngr);

Renderer_result_t CreateVulkanPresentationManager(VulkanPresentationManager_t* pres_mngr);
Renderer_result_t VulkanPresentationManager_QuerySupport(VulkanPresentationManager_t* pres_mngr);

void DestroyVulkanContext(VulkanContext_t* ctx);
void DestroyVulkanExtensionManager(VulkanExtensionManager_t* ext_mngr);
void DestroyVulkanDeviceManager(VulkanDeviceManager_t* dvc_mngr);
void DestroyVulkanPresentationManager(VulkanPresentationManager_t* pres_mngr);

#endif