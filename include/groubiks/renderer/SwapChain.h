
#ifndef GROUBIKS_SWAPCHAIN_H
#define GROUBIKS_SWAPCHAIN_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/vector.h>
#include <groubiks/utility/log.h>
#include <groubiks/renderer/ActiveDevice.h>
#include <groubiks/renderer/Devices.h>

declare_vector(VkImage);
declare_vector(VkImageView);
declare_vector(VkSurfaceFormatKHR);
declare_vector(VkPresentModeKHR);

typedef struct {
    VkSurfaceCapabilitiesKHR m_capabilities;
    vector_t(VkSurfaceFormatKHR) m_formats;
    vector_t(VkPresentModeKHR) m_modes;
} VulkanSwapChainDetails_t;
typedef VulkanSwapChainDetails_t* VulkanSwapChainDetails;

typedef struct {
    VkSwapchainKHR m_swapchain;
    VkExtent2D m_swapchain_extent;
    VkSurfaceFormatKHR m_swapchain_format;
    VkPresentModeKHR m_swapchain_mode;
    vector_t(VkImage) m_swapchain_images;
    vector_t(VkImageView) m_swapchain_imageviews;
} VulkanSwapChain_t;
typedef VulkanSwapChain_t* VulkanSwapChain;


VulkanSwapChain CreateSwapChain(VulkanActiveDevice device, VkSurfaceKHR surface, GLFWwindow* win);
VulkanSwapChainDetails _getSwapChainCapabilities(VulkanActiveDevice device, VkSurfaceKHR surface);
GroubiksResult_t _pickSurfaceFormat(VulkanSwapChain swpchn, VulkanSwapChainDetails details);
GroubiksResult_t _pickPresentMode(VulkanSwapChain swpchn, VulkanSwapChainDetails details);
GroubiksResult_t _pickSwapExtent(VulkanSwapChain swpchn, VulkanSwapChainDetails details, GLFWwindow* win);
GroubiksResult_t _setupImageViews(VulkanSwapChain swpchn, VulkanActiveDevice device);

void DestroySwapChain(VulkanSwapChain swpchn, VkDevice device);


#endif