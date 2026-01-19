
#ifndef GROUBIKS_VK_SWAPCHAIN_H
#define GROUBIKS_VK_SWAPCHAIN_H

/**
 * @file vulkan_swapchain.h
 * @date 19/01/26
 * @author Julian Benzel
 * @brief interface to a vulkan-swapchain. 
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h>
#include <groubiks/renderer/vulkan_device_context.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/dynarray.h>
#include <groubiks/utility/log.h>

declare_dynarray(VkSurfaceFormatKHR, VkSurfaceFormat);
declare_dynarray(VkPresentModeKHR, VkPresentMode);

declare_dynarray(VkImage, VkImage);
declare_dynarray(VkImageView, VkImageView);

struct vk_swapchain_details {
    VkSurfaceCapabilitiesKHR         m_capabilities;
    struct dynarray(VkSurfaceFormat) m_formats;
    struct dynarray(VkPresentMode)   m_modes;
};

struct vk_swapchain {
    VkSwapchainKHR               m_swapchain;
    VkExtent2D                   m_extent;
    VkSurfaceFormatKHR           m_format;
    VkPresentModeKHR             m_mode;
    struct dynarray(VkImage)     m_images;
    struct dynarray(VkImageView) m_imageviews;
};

inline VkSurfaceCapabilitiesKHR 
VkSurfaceCapabilitiesKHR_null() {
    VkSurfaceCapabilitiesKHR tmp;
    memzero(tmp);
    return tmp;
}

inline VkExtent2D 
VkExtent2D_null() {
    VkExtent2D tmp;
    memzero(tmp);
    return tmp;
}

inline VkSurfaceFormatKHR
VkSurfaceFormatKHR_null() {
    VkSurfaceFormatKHR tmp;
    memzero(tmp);
    return tmp;
}

#define vk_swapchain_details_null \
(struct vk_swapchain_details) { \
    .m_capabilities = VkSurfaceCapabilitiesKHR_null(), \
    .m_formats      = null_dynarray(VkSurfaceFormat), \
    .m_modes        = null_dynarray(VkPresentMode) \
}

#define vk_swapchain_null \
(struct vk_swapchain) { \
    .m_swapchain  = VK_NULL_HANDLE, \
    .m_extent     = VkExtent2D_null(), \
    .m_format     = VkSurfaceFormatKHR_null(), \
    .m_mode       = 0, \
    .m_images     = null_dynarray(VkImage), \
    .m_imageviews = null_dynarray(VkImageView) \
}

groubiks_result_t
vk_swapchain_create(struct vk_swapchain* pSwapChain,
    struct vk_device_context* pDeviceContext,
    VkSurfaceKHR surface,
    GLFWwindow* pWin
);

void
vk_swapchain_free(struct vk_swapchain* pSwapChain,
    VkDevice device
);

groubiks_result_t
vk_swapchain_details_get_surfaceformats(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);

groubiks_result_t
vk_swapchain_details_get_presentmodes(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);

groubiks_result_t
vk_swapchain_details_create(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);

groubiks_result_t 
vk_swapchain_pick_details(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails
);

groubiks_result_t 
vk_swapchain_pick_surface_format(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails
);

groubiks_result_t
vk_swapchain_pick_extent(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails
);

groubiks_result_t 
vk_swapchain_pick_presentmode(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails
);

groubiks_result_t
vk_swapchain_get_image_handles(struct vk_swapchain* pSwapChain,
    struct vk_device_context* pDeviceContext,
    u32 imageCount
);

groubiks_result_t
vk_swapchain_setup_imageviews(struct vk_swapchain* pSwapChain,
    VkDevice ldevice
);

#endif