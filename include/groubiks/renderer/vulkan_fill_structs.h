
#ifndef GROUBIKS_VULKAN_FILL_STRUCTS_H
#define GROUBIKS_VULKAN_FILL_STRUCTS_H

/**
 * @file vulkan_fill_structs.h
 * @date 18/01/26
 * @author Julian Benzel
 * @brief utility-methods to make filling out
 *        vulkan-structs look less ugly in my code.
 *        credits to https://vulkan-tutorial.com
 */

#include <vulkan/vulkan.h>
#include <groubiks/utility/common.h>

void 
vk_fill_struct_swapchain_createinfo(VkSwapchainCreateInfoKHR* pCreateInfo,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR* pFormat,
    VkPresentModeKHR mode,
    VkExtent2D* pExtent,
    VkSurfaceCapabilitiesKHR* pCapabilities,
    u32* pQfis,
    u32  qfiCount
);

void
vk_fill_struct_imageview_createinfo(VkImageViewCreateInfo* pCreateInfo,
    VkImage image,
    VkFormat format
);

void
vk_fill_struct_device_createinfo(VkDeviceCreateInfo* pCreateInfo,
    const VkPhysicalDeviceFeatures* pFeatures,
    const VkDeviceQueueCreateInfo* pCreateInfos,
    u32 createInfoCount,
    const char* const* pLayers,
    u32 layerCount,
    const char* const* pExtensions,
    u32 extCount
);

void
vk_fill_struct_devicequeue_createinfo(VkDeviceQueueCreateInfo* pCreateInfo,
    u32 qFamilyIdx,
    const float* qPriority
);

#endif