
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
    GLFWwindow* pWindow)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    
    VkSwapchainCreateInfoKHR createInfo;
    struct vk_swapchain_details details = vk_swapchain_details_null;
    u32 qfis[2] = { 
        pDeviceContext->m_qfis.m_graphics_family.value, 
        pDeviceContext->m_qfis.m_present_family.value
    };

    *pSwapChain = vk_swapchain_null;

    err = vk_swapchain_details_create(&details, 
        pDeviceContext->m_physical_device, 
        surface
    );
    check(err == GROUBIKS_SUCCESS);

    err = vk_swapchain_pick_details(pSwapChain, &details, pWindow);
    check(err == GROUBIKS_SUCCESS);
    
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
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    err = vk_swapchain_get_image_handles(pSwapChain, pDeviceContext, createInfo.minImageCount);
    check(err == GROUBIKS_SUCCESS);
    err = vk_swapchain_setup_imageviews(pSwapChain, pDeviceContext->m_logical_device);
    check(err == GROUBIKS_SUCCESS);

    cleanup(
        free_dynarray(VkSurfaceFormat, &details.m_formats);
        free_dynarray(VkPresentMode, &details.m_modes);
    )

    log_info(VK_SWAPCHAIN_CREATE_SUCCESS_STR);
    return err;
    except(
        free_vk_swapchain(pSwapChain, pDeviceContext->m_logical_device);
        log_error(VK_SWAPCHAIN_CREATE_FAIL_STR);
    )
}


void
free_vk_swapchain(struct vk_swapchain* pSwapChain, VkDevice device)
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
    groubiks_result_t err         = GROUBIKS_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;

    vkErr = vkGetSwapchainImagesKHR(pDeviceContext->m_logical_device, 
        pSwapChain->m_swapchain, 
        &imageCount, 
        NULL
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    dynarray_resize(VkImage, &pSwapChain->m_images, imageCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    vkErr = vkGetSwapchainImagesKHR(pDeviceContext->m_logical_device, 
        pSwapChain->m_swapchain, 
        &imageCount, 
        pSwapChain->m_images.data
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    logf_info(VK_SWAPCHAIN_IMAGE_HANDLES_SUCCESS_FSTR, pSwapChain->m_images.size);
    return err;
    except(err,
        log_error(VK_SWAPCHAIN_IMAGE_HANDLES_FAIL_STR);
    )
}


groubiks_result_t
vk_swapchain_details_get_surfaceformats(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    uint32_t surfaceFormatCount = 0;

    vkErr = vkGetPhysicalDeviceSurfaceFormatsKHR(device,
        surface,
        &surfaceFormatCount,
        NULL
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    dynarray_resize(VkSurfaceFormat, 
        &pSwapChainDetails->m_formats, 
        surfaceFormatCount, 
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC)
    
    vkErr = vkGetPhysicalDeviceSurfaceFormatsKHR(device, 
        surface, 
        &surfaceFormatCount, 
        pSwapChainDetails->m_formats.data
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_SWAPCHAIN_SURFACE_FMTS_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_SWAPCHAIN_SURFACE_FMTS_FAIL_STR);
    )
}


groubiks_result_t
vk_swapchain_details_get_presentmodes(struct vk_swapchain_details* pSwapChainDetails,
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    uint32_t presentModeCount = 0;

    vkErr = vkGetPhysicalDeviceSurfacePresentModesKHR(device,
        surface,
        &presentModeCount,
        NULL
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    dynarray_resize(VkPresentMode, 
        &pSwapChainDetails->m_modes, 
        presentModeCount, 
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    vkErr = vkGetPhysicalDeviceSurfacePresentModesKHR(device, 
        surface, 
        &presentModeCount, 
        pSwapChainDetails->m_modes.data
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    log_info(VK_SWAPCHAIN_PRESENTMODES_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_SWAPCHAIN_PRESENTMODES_FAIL_STR);
    )
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
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    err = vk_swapchain_details_get_presentmodes(pSwapChainDetails, device, surface);
    check(err == GROUBIKS_SUCCESS);
    err = vk_swapchain_details_get_surfaceformats(pSwapChainDetails, device, surface);
    check(err == GROUBIKS_SUCCESS);

    log_info(VK_SWAPCHAIN_SURFACE_CAPS_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_SWAPCHAIN_SURFACE_CAPS_FAIL_STR);
    )
}

groubiks_result_t 
vk_swapchain_pick_details(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails,
    GLFWwindow* pWindow)
{
    return vk_swapchain_pick_extent(pSwapChain, pSwapChainDetails, pWindow) ||
           vk_swapchain_pick_surface_format(pSwapChain, pSwapChainDetails) ||
           vk_swapchain_pick_presentmode(pSwapChain, pSwapChainDetails);
}

groubiks_result_t 
vk_swapchain_pick_surface_format(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
    dynarray_for_each(VkSurfaceFormat, &pSwapChainDetails->m_formats, format) {
        if (format->format     == VK_FORMAT_B8G8R8_SRGB &&
            format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { 
            pSwapChain->m_format = *format; 
            log_info("found preferred surface-format.");
            return GROUBIKS_SUCCESS; 
        }
    }
    log_info("failed to find preferred surface-format.");
    if (pSwapChainDetails->m_formats.size == 0)
    { log_error("failed to find any surface-format."); return -1; }
    pSwapChain->m_format = pSwapChainDetails->m_formats.data[0];
    log_info("picked first of available surface-formats.");
    return 0;
}

groubiks_result_t
vk_swapchain_pick_extent(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails,
    GLFWwindow* pWindow)
{
    if (pSwapChainDetails->m_capabilities.currentExtent.width != UINT32_MAX) { 
        pSwapChain->m_extent = pSwapChainDetails->m_capabilities.currentExtent;
        logf_info("set swapextent to currentextent of %dx%d.",
            pSwapChain->m_extent.width,
            pSwapChain->m_extent.height);
    }
    else {
        int width, height;

        glfwGetFramebufferSize(pWindow, &width, &height);
        VkExtent2D extent = {
            .width = width,
            .height = height
        };
        extent.width = clamp(extent.width, 
            pSwapChainDetails->m_capabilities.minImageExtent.width, 
            pSwapChainDetails->m_capabilities.maxImageExtent.width);
        extent.height = clamp(extent.height,
            pSwapChainDetails->m_capabilities.minImageExtent.height,
            pSwapChainDetails->m_capabilities.maxImageExtent.height);
        pSwapChain->m_extent = extent;
        logf_info("determined valid swapextent of %dx%d.", 
            extent.width, extent.height);
    }
    return GROUBIKS_SUCCESS;
}


groubiks_result_t 
vk_swapchain_pick_presentmode(struct vk_swapchain* pSwapChain,
    struct vk_swapchain_details* pSwapChainDetails)
{
    /* set present-mode to guaranteed options right away. other present modes may be checked here if desired */
    pSwapChain->m_mode = VK_PRESENT_MODE_FIFO_KHR;
    log_info("found viable present-mode for swapchain.");
    return GROUBIKS_SUCCESS;
}


groubiks_result_t
vk_swapchain_setup_imageviews(struct vk_swapchain* pSwapChain,
    VkDevice device)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;

    dynarray_resize(VkImageView, &pSwapChain->m_imageviews, pSwapChain->m_images.size, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    dynarray_for_each(VkImageView, &pSwapChain->m_imageviews, imageView) {
        uint32_t idx = dynarray_index(&pSwapChain->m_imageviews, imageView);
        VkImageViewCreateInfo createInfo;
        vk_fill_struct_imageview_createinfo(&createInfo, 
            pSwapChain->m_images.data[idx], 
            pSwapChain->m_format.format
        );
        vkErr = vkCreateImageView(device, &createInfo, NULL, imageView);
        check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    }
    logf_info(VK_SWAPCAHIN_IMAGEVIEW_SUCCESS_FSTR, pSwapChain->m_imageviews.size);
    return err;
    except(err,
        log_error(VK_SWAPCHAIN_IMAGEVIEW_FAIL_STR);
    )
}