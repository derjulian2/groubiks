
#include <groubiks/renderer/vulkan_swapchain.h>

define_dynarray(VkSurfaceFormatKHR, VkSurfaceFormat,
    (comp, NULL)
);

define_dynarray(VkImage, VkImage);
define_dynarray(VkImageView, VkImageView);
define_dynarray(VkPresentModeKHR, VkPresentMode);


groubiks_result_t
vk_swapchain_create(struct vk_swapchain* pSwapChain,
    struct vk_device_context* pDeviceContext,
    VkSurfaceKHR surface,
    GLFWwindow* pWin)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    

    u32 qfis[2] = { 
        pDeviceContext->m_qfis.m_graphics_family.value, 
        pDeviceContext->m_qfis.m_present_family.value
    };

    *pSwapChain = vk_swapchain_null;
    struct vk_swapchain_details details = vk_swapchain_details_null;

    err = vk_swapchain_details_create(&details, 
        pDeviceContext->m_physical_device, 
        surface
    );
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

    err = vk_swapchain_pick_details(pSwapChain, &details);

    VkSwapchainCreateInfoKHR createInfo;
    vk_fill_struct_swapchain_createinfo(&createInfo, 
        surface, 
        &pSwapChain->m_format, 
        pSwapChain->m_mode, 
        &pSwapChain->m_extent, 
        &details.m_capabilities,
        &qfis[0],
        (sizeof(qfis) / sizeof(u32))
    );

    vkErr = vkCreateSwapchainKHR(pDeviceContext->m_logical_device, 
        &createInfo, 
        NULL, 
        &pSwapChain->m_swapchain
    );
    if (vkErr != VK_SUCCESS) { goto error; }

    err = vk_swapchain_get_image_handles(pSwapChain, pDeviceContext, createInfo.minImageCount);

    err = vk_swapchain_setup_imageviews(pSwapChain, pDeviceContext->m_logical_device);

cleanup:
    free_dynarray(VkSurfaceFormat, &details.m_formats);
    free_dynarray(VkPresentMode, &details.m_modes);

    if (err   != GROUBIKS_SUCCESS ||
        vkErr != VK_SUCCESS) 
    { goto error; }

    log_info("created swapchain.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to create swapchain.");
    return err;
}


void
vk_swapchain_free(struct vk_swapchain* pSwapChain, VkDevice device)
{
    if (pSwapChain->m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, pSwapChain->m_swapchain, NULL);
    }
    dynarray_for_each(VkImageView, &pSwapChain->m_imageviews, imageView) {
        vkDestroyImageView(device, *imageView, NULL);
    }
    free_dynarray(VkImageView, &pSwapChain->m_imageviews);
    free_dynarray(VkImage, &pSwapChain->m_images);
}


groubiks_result_t
vk_swapchain_get_image_handles(struct vk_swapchain* pSwapChain,
    struct vk_device_context* pDeviceContext,
    u32 imageCount)
{
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;

    vkErr = vkGetSwapchainImagesKHR(pDeviceContext->m_logical_device, 
        pSwapChain->m_swapchain, 
        &imageCount, 
        NULL
    );
    if (vkErr != VK_SUCCESS)
    { goto error; }
    pSwapChain->m_images = make_dynarray(VkImage, 
        NULL, 
        imageCount, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS)
    { goto error; }

    vkErr = vkGetSwapchainImagesKHR(pDeviceContext->m_logical_device, 
        pSwapChain->m_swapchain, 
        &imageCount, 
        pSwapChain->m_images.data
    );
    if (vkErr != VK_SUCCESS)
    { goto error; }

    log_info("retrieved image-handles.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to retrieve image-handles.");
    return GROUBIKS_VULKAN_ERROR;
}


groubiks_result_t
vk_swapchain_details_get_surfaceformats(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    uint32_t surfaceFormatCount = 0;

    vkErr = vkGetPhysicalDeviceSurfaceFormatsKHR(device,
        surface,
        &surfaceFormatCount,
        NULL
    );
    if (vkerr != VK_SUCCESS) { goto error; }
    dynarray_reserve(str, 
        &pSwapChainDetails->m_formats, 
        surfaceFormatCount, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { goto error; }
    vkErr = vkGetPhysicalDeviceSurfaceFormatsKHR(device, 
        surface, 
        &surfaceFormatCount, 
        &pSwapChainDetails->m_formats.data
    );
    if (vkerr != VK_SUCCESS) { goto error; }
    log_info("retrieved surface-formats.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to retrieve surface-formats.");
    return GROUBIKS_ERROR;
}


groubiks_result_t
vk_swapchain_details_get_presentmodes(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    uint32_t presentModeCount = 0;

    vkErr = vkGetPhysicalDeviceSurfacePresentModesKHR(device,
        surface,
        &presentModeCount,
        NULL
    );
    if (vkerr != VK_SUCCESS) { goto error; }
    dynarray_reserve(str, 
        &pSwapChainDetails->m_modes, 
        presentModeCount, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { goto error; }
    vkErr = vkGetPhysicalDeviceSurfacePresentModesKHR(device, 
        surface, 
        &presentModeCount, 
        &pSwapChainDetails->m_modes.data
    );
    if (vkerr != VK_SUCCESS) { goto error; }
    log_info("retrieved surface-presentmodes.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to retrieve surface-presentmodes.");
    return GROUBIKS_ERROR;
}


groubiks_result_t
vk_swapchain_details_create(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;

    vkErr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, 
        surface,
        &pSwapChainDetails->m_capabilities
    );
    if (vkErr != VK_SUCCESS) {
        log_error("failed to retrieve surface-capabilities.");
        return GROUBIKS_ERROR;
    }

    err = vk_swapchain_details_get_presentmodes(pSwapChainDetails, device, surface);
    err = vk_swapchain_details_get_surfaceformats(pSwapChainDetails, device, surface);

    log_info("retrieved surface-capabilities.");
    return GROUBIKS_SUCCESS;
}

groubiks_result_t 
vk_swapchain_pick_details(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
    return vk_swapchain_pick_extent(pSwapChain, pSwapChainDetails) ||
           vk_swapchain_pick_surface_format(pSwapChain, pSwapChainDetails) ||
           vk_swapchain_pick_presentmode(pSwapChain, pSwapChainDetails);
}

groubiks_result_t 
vk_swapchain_pick_surface_format(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
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

groubiks_result_t
vk_swapchain_pick_extent(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
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
}


groubiks_result_t 
vk_swapchain_pick_presentmode(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
    /* set present-mode to guaranteed options right away. other present modes may be checked here if desired */
    swapchain->m_swapchain_mode = VK_PRESENT_MODE_FIFO_KHR;
    log_info("found viable present-mode for swapchain.");
}


groubiks_result_t
vk_swapchain_setup_imageviews(struct vk_swapchain* pSwapChain,
    VkDevice ldevice)
{
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;

    pSwapChain->m_imageviews = make_dynarray(VkImageView, 
        NULL, 
        pSwapChain->m_images.size, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { goto error; }

    dynarray_for_each(VkImageView, &pSwapChain.m_imageviews, imageView) {
        uint32_t idx = dynarray_index(&pSwapChain->m_imageviews, imageview);
        VkImageViewCreateInfo createInfo;
        vk_fill_struct_imageview_createinfo(&createInfo, 
            pSwapChain->m_images[idx], 
            pSwapChain->m_format
        );
        vkErr = vkCreateImageView(ldevice, &createInfo, NULL, imageView);
        if (vkErr != VK_SUCCESS) { goto error; }
    }
    log_info("setup imageviews.");
    return GROUBIKS_SUCCESS;
error:
    free_dynarray(VkImageView, &pSwapChain->m_imageviews);
    log_error("failed to setup imageviews.");
    return GROUBIKS_VULKAN_ERROR;
}