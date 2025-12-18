
#ifndef GROUBIKS_VULKAN_CONTEXT_H
#define GROUBIKS_VULKAN_CONTEXT_H

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

#include <groubiks/renderer/Extensions.h>
#include <groubiks/renderer/Devices.h>



typedef struct {
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VulkanExtensions_t m_extensions;
    VulkanDevices_t m_devices;
    // VulkanPresentationManager_t m_presentation_mngr;
} VulkanContext_t;

/** 
 * @brief initializes the applications interface with the Vulkan-API.
 *        should be cleaned up with DestroyVulkanContext() after usage or on error.
 */
GroubiksResult_t CreateVulkanContext(VulkanContext_t* ctx);
GroubiksResult_t _setupExtensions(VulkanContext_t* ctx);
GroubiksResult_t _setupVulkanInstance(VulkanContext_t* ctx);
GroubiksResult_t _setupDebugMessenger(VulkanContext_t* ctx);
GroubiksResult_t _setupDevices(VulkanContext_t* ctx);

void DestroyVulkanContext(VulkanContext_t* ctx);
void _destroyDebugMessenger(VulkanContext_t* ctx);

GroubiksResult_t VulkanContext_Draw();

static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_GroubiksDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

#endif