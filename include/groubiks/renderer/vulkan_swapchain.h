
#ifndef GROUBIKS_VK_SWAPCHAIN_H
#define GROUBIKS_VK_SWAPCHAIN_H

/**
 * @file vulkan_swapchain.h
 * @date 22/12/25
 * @author Julian Benzel
 * @brief interface to a vulkan-swapchain. 
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/vector.h>
#include <groubiks/utility/log.h>
#include <groubiks/renderer/ActiveDevice.h>
#include <groubiks/renderer/Devices.h>

declare_vector(VkSurfaceFormatKHR);
declare_vector(VkPresentModeKHR);

struct vk_swapchain_details {
    VkSurfaceCapabilitiesKHR m_capabilities;
    vector_t(VkSurfaceFormatKHR) m_formats;
    vector_t(VkPresentModeKHR) m_modes;
};

groubiks_result_t vk_swapchain_details_get(struct vk_swapchain_details* pDetails);

declare_vector(VkImage);
declare_vector(VkImageView);

struct vk_swapchain {
    VkSwapchainKHR m_swapchain;
    VkExtent2D m_extent;
    VkSurfaceFormatKHR m_format;
    VkPresentModeKHR m_mode;
    vector_t(VkImage) m_images;
    vector_t(VkImageView) m_imageviews;
};

groubiks_result_t vk_swapchain_make();

VulkanSwapChain CreateSwapChain(VulkanActiveDevice device, VkSurfaceKHR surface, GLFWwindow* win);
VulkanSwapChainDetails _getSwapChainCapabilities(VulkanActiveDevice device, VkSurfaceKHR surface);
result_t _pickSurfaceFormat(VulkanSwapChain swpchn, VulkanSwapChainDetails details);
result_t _pickPresentMode(VulkanSwapChain swpchn, VulkanSwapChainDetails details);
result_t _pickSwapExtent(VulkanSwapChain swpchn, VulkanSwapChainDetails details, GLFWwindow* win);
result_t _setupImageViews(VulkanSwapChain swpchn, VulkanActiveDevice device);

void DestroySwapChain(VulkanSwapChain swpchn, VkDevice device);


#endif