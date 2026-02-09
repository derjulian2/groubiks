
#ifndef GROUBIKS_VULKAN_CTX_H
#define GROUBIKS_VULKAN_CTX_H

/**
 * @file vulkan_context.h
 * @date 19/01/26
 * @author Julian Benzel
 * @brief basic Vulkan-graphics-API setu.
 *        credits to https://vulkan-tutorial.com without which i would
 *        not have been able to learn using it so easily.
 */

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>
#include <groubiks/renderer/vulkan_extras.h>
#include <groubiks/renderer/vulkan_fill_structs.h>

declare_dynarray(VkPhysicalDevice, VkPhysicalDevice);

struct vk_context {
    VkInstance                        m_instance;
    VkDebugUtilsMessengerEXT          m_debug_messenger;
    GLFWwindow*                       m_window;
    VkSurfaceKHR                      m_surface;
    struct dynarray(VkPhysicalDevice) m_physical_devices;
};

#define vk_context_null \
(struct vk_context) { \
    .m_instance         = VK_NULL_HANDLE,                 \
    .m_debug_messenger  = VK_NULL_HANDLE,                 \
    .m_window           = NULL,                           \
    .m_physical_devices = null_dynarray(VkPhysicalDevice) \
}

groubiks_result_t 
vk_context_create(struct vk_context* pVulkanContext, 
    struct vk_extras* pExtras,
    GLFWwindow* pWindow
);

void 
free_vk_context(struct vk_context* pVulkanContext);

groubiks_result_t
vk_context_setup_windowsurface(struct vk_context* pVulkanContext);

groubiks_result_t 
vk_context_setup_instance(struct vk_context* pVulkanContext,
    struct vk_extras* pExtras
);

groubiks_result_t 
vk_context_setup_debug_messenger(struct vk_context* pVulkanContext);

groubiks_result_t 
vk_context_setup_devices(struct vk_context* pVulkanContext);

static inline VKAPI_ATTR VkBool32 
VKAPI_CALL vk_groubiks_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity)
    {
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT):
        log_info(pCallbackData->pMessage);
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT):
        log_warning(pCallbackData->pMessage);
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT):
        log_error(pCallbackData->pMessage);
    default:
        log(INFO_LOG, pCallbackData->pMessage);
    }
    return VK_FALSE;
}

#endif