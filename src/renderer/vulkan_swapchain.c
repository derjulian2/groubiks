
#include <groubiks/renderer/vulkan_swapchain.h>


VulkanSwapChain CreateSwapChain(VulkanActiveDevice device, VkSurfaceKHR surface, GLFWwindow* win) {
    result_t err = 0;
    uint32_t imageCount;
    VulkanSwapChain swapchain;
    VkResult vkerr = VK_SUCCESS;
    VulkanSwapChainDetails details;
    VkSwapchainCreateInfoKHR createInfo;
    details = _getSwapChainCapabilities(device, surface);
    swapchain = malloc(sizeof(VulkanSwapChain_t));
    if (details == NULL || swapchain == NULL)
    { goto error; }
    /* pick some stuff for the swapchain */
    if (details->m_formats.size == 0 || details->m_modes.size == 0)
    { log_error("no swapchain formats/modes available."); goto error; }
    err = _pickSurfaceFormat(swapchain, details);
    if (err != 0)
    { goto error; }
    err = _pickPresentMode(swapchain, details);
    if (err != 0)
    { goto error; }
    err = _pickSwapExtent(swapchain, details, win);
    if (err != 0)
    { goto error; }
    /* create the actual thing */
    imageCount = details->m_capabilities.minImageCount + 1;
    if (details->m_capabilities.maxImageCount != 0 &&
        (details->m_capabilities.minImageCount + 1) > details->m_capabilities.maxImageCount)
    { imageCount = details->m_capabilities.maxImageCount; }
    memzero(createInfo);
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapchain->m_swapchain_format.format;
    createInfo.imageColorSpace = swapchain->m_swapchain_format.colorSpace;
    createInfo.imageExtent = swapchain->m_swapchain_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = details->m_capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapchain->m_swapchain_mode;
    createInfo.clipped = VK_TRUE;

    uint32_t queueFamilyIndices[] = { 
        device->m_family_indices.m_graphics_family.value,
        device->m_family_indices.m_present_family.value
    };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = &queueFamilyIndices[0];
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    vkerr = vkCreateSwapchainKHR(
        device->m_logical_device, 
        &createInfo, 
        NULL, 
        &swapchain->m_swapchain);
    if (vkerr != VK_SUCCESS)
    { goto error; }

    /* retrieve image handles */
    vkerr = vkGetSwapchainImagesKHR(
        device->m_logical_device, 
        swapchain->m_swapchain, 
        &imageCount, 
        NULL);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    swapchain->m_swapchain_images = make_vector(VkImage, NULL, imageCount, &err);
    if (err != 0)
    { goto error; }
    vkerr = vkGetSwapchainImagesKHR(
        device->m_logical_device, 
        swapchain->m_swapchain, 
        &imageCount, 
        swapchain->m_swapchain_images.data);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    
    /* setup imageviews */
    err = _setupImageViews(swapchain, device);
    if (err != 0)
    { goto error; }

    log_info("successfully setup swapchain and retrieved image-handles.");
    free_vector(&details->m_formats);
    free_vector(&details->m_modes);
    free(details);
    return swapchain;
error:
    log_error("failed to setup swapchain.");
    if (details != NULL)
    {
        free_vector(&details->m_formats);
        free_vector(&details->m_modes);
        free(details);
    }
    free_vector(&swapchain->m_swapchain_images);
    return NULL;
}

VulkanSwapChainDetails _getSwapChainCapabilities(VulkanActiveDevice device, VkSurfaceKHR surface) {
    VkResult vkerr = VK_SUCCESS;
    result_t err = 0;
    uint32_t formatCount, modeCount;
    VulkanSwapChainDetails res = malloc(sizeof(VulkanSwapChainDetails_t));
    if (res == NULL)
    { return NULL; }
    memzero(*res);
    /* retrieve capabilities */
    vkerr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device->m_physical_device,
        surface,
        &res->m_capabilities);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    /* retrieve formats */
    vkerr = vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->m_physical_device,
        surface,
        &formatCount,
        NULL);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    res->m_formats = make_vector(VkSurfaceFormatKHR, NULL, formatCount, &err);
    if (err != 0)
    { goto error; }
    vkerr = vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->m_physical_device,
        surface,
        &formatCount,
        res->m_formats.data);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    /* retrieve modes */
    vkerr = vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->m_physical_device,
        surface,
        &modeCount,
        NULL);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    res->m_modes = make_vector(VkPresentModeKHR, NULL, modeCount, &err);
    if (err != 0)
    { goto error; }
    vkerr = vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->m_physical_device,
        surface,
        &modeCount,
        res->m_modes.data);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    log_info("retrieved swapchain-capabilities.");
    return res;
error:
    log_error("failed to retrieve swapchain-capabilities.");
    free_vector(&res->m_formats);
    free_vector(&res->m_modes);
    free(res);
    return NULL;
}

result_t _pickSurfaceFormat(VulkanSwapChain swapchain, VulkanSwapChainDetails details) {
    vector_for_each(VkSurfaceFormatKHR, &details->m_formats, format) {
        if (format->format     == VK_FORMAT_B8G8R8_SRGB &&
            format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { 
            swapchain->m_swapchain_format = *format; 
            log_info("found preferred surface-format.");
            return 0; 
        }
    }
    log_info("failed to find preferred surface-format.");
    if (details->m_formats.size == 0)
    { log_error("failed to find any surface-format."); return -1; }
    swapchain->m_swapchain_format = details->m_formats.data[0];
    log_info("picked first of available surface-formats.");
    return 0;
}

result_t _pickPresentMode(VulkanSwapChain swapchain, VulkanSwapChainDetails details) {
    /* set present-mode to guaranteed options right away. other present modes may be checked here if desired */
    swapchain->m_swapchain_mode = VK_PRESENT_MODE_FIFO_KHR;
    log_info("found viable present-mode for swapchain.");
    return 0;
}

result_t _pickSwapExtent(VulkanSwapChain swapchain, VulkanSwapChainDetails details, GLFWwindow* win) {
    if (details->m_capabilities.currentExtent.width != UINT32_MAX) { 
        swapchain->m_swapchain_extent = details->m_capabilities.currentExtent;
        logf_info("set swapextent to currentextent of %dx%d.",
            swapchain->m_swapchain_extent.width,
            swapchain->m_swapchain_extent.height);
    }
    else {
        int width, height;

        glfwGetFramebufferSize(win, &width, &height);
        VkExtent2D extent = {
            .width = width,
            .height = height
        };
        extent.width = clamp(extent.width, 
            details->m_capabilities.minImageExtent.width, 
            details->m_capabilities.maxImageExtent.width);
        extent.height = clamp(extent.height,
            details->m_capabilities.minImageExtent.height,
            details->m_capabilities.maxImageExtent.height);
        swapchain->m_swapchain_extent = extent;
        logf_info("determined valid swapextent of %dx%d.", 
            extent.width, extent.height);
    }
    return 0;
}

result_t _setupImageViews(VulkanSwapChain swapchain, VulkanActiveDevice device) {
    result_t err = 0;
    VkResult vkerr = VK_SUCCESS;

    swapchain->m_swapchain_imageviews = make_vector(VkImageView, NULL, swapchain->m_swapchain_images.size, &err);
    if (err != 0)
    { return -1; }

    vector_for_each(VkImageView, &swapchain->m_swapchain_imageviews, imageView) {
        int idx = vector_make_index(VkImageView, &swapchain->m_swapchain_imageviews, imageView);
        VkImageViewCreateInfo createInfo;
        memzero(createInfo);
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchain->m_swapchain_images.data[idx];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchain->m_swapchain_format.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        vkerr = vkCreateImageView(
            device->m_logical_device, 
            &createInfo, 
            NULL,
            imageView);
        if (vkerr != 0)
        { goto error; }
    }
    log_info("successfully setup imageviews");
    return 0;
error:
    log_error("failed to setup imageviews.");
    free_vector(&swapchain->m_swapchain_imageviews);
    return -1;
}

void DestroySwapChain(VulkanSwapChain swapchain, VkDevice device) {
    free_vector(&swapchain->m_swapchain_images);
    vector_for_each(VkImageView, &swapchain->m_swapchain_imageviews, imageView)
    { vkDestroyImageView(device, *imageView, NULL); }
    free_vector(&swapchain->m_swapchain_imageviews);
    vkDestroySwapchainKHR(device, swapchain->m_swapchain, NULL);
}