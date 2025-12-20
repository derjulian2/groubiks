
#ifndef GROUBIKS_VULKAN_CONTEXT_H
#define GROUBIKS_VULKAN_CONTEXT_H

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

#include <groubiks/renderer/Extensions.h>
#include <groubiks/renderer/Devices.h>
#include <groubiks/renderer/RenderContext.h>
#include <groubiks/renderer/GraphicsPipeline.h>
#include <groubiks/renderer/Commands.h>

typedef struct {
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VulkanExtensions m_extensions;
    VulkanDevices m_devices;
    vector_t(RenderContext) m_render_ctxs;
} VulkanContext_t;
typedef VulkanContext_t* VulkanContext;

/** 
 * @brief initializes the applications interface with the Vulkan-API.
 *        should be cleaned up with DestroyVulkanContext() after usage or on error.
 */
VulkanContext CreateVulkanContext();
GroubiksResult_t _setupExtensions(VulkanContext_t* ctx);
GroubiksResult_t _setupVulkanInstance(VulkanContext_t* ctx);
GroubiksResult_t _setupDebugMessenger(VulkanContext_t* ctx);
GroubiksResult_t _setupDevices(VulkanContext_t* ctx);

RenderContext VulkanContext_AddRenderContext(VulkanContext ctx, GLFWwindow* win);
GroubiksResult_t VulkanContext_Draw(VulkanContext ctx, RenderContext rndr_ctx);

void DestroyVulkanContext(VulkanContext ctx);
void _destroyDebugMessenger(VulkanContext_t* ctx);

static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_GroubiksDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

#endif