
#include <groubiks/renderer/vulkan_context.h>

define_dynarray(VkPhysicalDevice, VkPhysicalDevice, 
    (comp, NULL)
);


groubiks_result_t 
vk_context_create(struct vk_context* pVulkanContext, 
    struct vk_extras* pExtras)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;

    *pVulkanContext = vk_context_null;

    err = vk_context_setup_instance(pVulkanContext, pExtras);
    check(err == GROUBIKS_SUCCESS);

    err = vk_context_setup_devices(pVulkanContext);
    check(err == GROUBIKS_SUCCESS);

    err = vk_context_setup_debug_messenger(pVulkanContext);
    check(err == GROUBIKS_SUCCESS);

    log_info(VK_CONTEXT_CREATE_SUCCESS_STR);
    return err;
    except(err,
        free_vk_context(pVulkanContext);
        log_error(VK_CONTEXT_CREATE_FAIL_STR);
    )
}


void 
free_vk_context(struct vk_context* pVulkanContext)
{
    if (pVulkanContext->m_debug_messenger != VK_NULL_HANDLE) {
        PFN_vkDestroyDebugUtilsMessengerEXT destroyMessengerFunc = 
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pVulkanContext->m_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroyMessengerFunc == NULL || pVulkanContext->m_instance == VK_NULL_HANDLE) {
            log_error(VK_CONTEXT_DEBUG_MESSENGER_CLEANUP_FAIL_STR);
        }
        else {
            destroyMessengerFunc(pVulkanContext->m_instance, pVulkanContext->m_debug_messenger, NULL);
        }
    }
    if (pVulkanContext->m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(pVulkanContext->m_instance, NULL);
    }
}


groubiks_result_t 
vk_context_setup_instance(struct vk_context* pVulkanContext,
    struct vk_extras* pExtras)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;

    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;

    err = vk_extras_match_instance(pExtras);
    check(err == GROUBIKS_SUCCESS);

    vk_fill_struct_instance_appinfo(&appInfo);
    vk_fill_struct_instance_createinfo(&createInfo, 
        &appInfo, 
        (const char*const*)pExtras->m_validationlayers.data, pExtras->m_validationlayers.size, 
        (const char*const*)pExtras->m_extensions.data, pExtras->m_extensions.size
    );
    
    vkErr = vkCreateInstance(&createInfo, NULL, &pVulkanContext->m_instance);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_CONTEXT_INSTANCE_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_INSTANCE_SETUP_FAIL_STR);
    )
}


groubiks_result_t 
vk_context_setup_debug_messenger(struct vk_context* pVulkanContext)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS; 

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    
    vk_fill_struct_debugmessenger_createinfo(&createInfo, &vk_groubiks_debug_callback);

    PFN_vkCreateDebugUtilsMessengerEXT createMessengerFunc = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(pVulkanContext->m_instance, "vkCreateDebugUtilsMessengerEXT");
    check(createMessengerFunc != NULL, err = GROUBIKS_VULKAN_ERROR);
     
    vkErr = createMessengerFunc(pVulkanContext->m_instance, &createInfo, NULL, &pVulkanContext->m_debug_messenger);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_CONTEXT_DEBUG_MESSENGER_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_DEBUG_MESSENGER_SETUP_FAIL_STR)
    )
}


groubiks_result_t 
vk_context_setup_devices(struct vk_context* pVulkanContext)
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;
    u32 deviceCount = 0;

    vkErr = vkEnumeratePhysicalDevices(pVulkanContext->m_instance, &deviceCount, NULL);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkPhysicalDevice, &pVulkanContext->m_physical_devices, deviceCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumeratePhysicalDevices(pVulkanContext->m_instance, &deviceCount, pVulkanContext->m_physical_devices.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    VkPhysicalDeviceProperties props;
    memzero(props);
    dynarray_for_each(VkPhysicalDevice, &pVulkanContext->m_physical_devices, device) {
        vkGetPhysicalDeviceProperties(*device, &props);
        logf_info("found device: %s", props.deviceName);
    }

    log_info(VK_CONTEXT_DEVICE_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_DEVICE_SETUP_FAIL_STR);
    )
}

VulkanDevices CreateVulkanDevices(VkInstance instance, const char** extensionNames, uint32_t numExtensionsNames) {
    VulkanDevices dvcs = malloc(sizeof(VulkanDevices_t));
    if (dvcs == NULL)
    { return NULL; }
    
    memzero(*dvcs);
    dvcs->m_device_extensions = make_extensions(
            VK_VALIDATIONLAYERS, VK_NUM_VALIDATIONLAYERS,
            extensionNames, numExtensionsNames);
    if (dvcs->m_device_extensions == NULL ||
        _setupAvailableDevices(dvcs, instance))
    { goto error; }
    log_info("successfully retrieved physical devices.");
    return dvcs;
error:
    log_error("failed to retrieve physical devices.");
    DestroyVulkanDevices(dvcs);
    return NULL;
}


result_t _isDeviceSuitable(VkPhysicalDevice dvc) {
    assert(dvc != NULL);
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    
    vkGetPhysicalDeviceProperties(dvc, &props);
    vkGetPhysicalDeviceFeatures(dvc, &features);
    return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

RenderContext VulkanContext_AddRenderContext(VulkanContext ctx, GLFWwindow* win) {
    result_t err = 0;
    RenderContext new_ctx = CreateRenderContext(win, ctx->m_instance, ctx->m_devices);
    if (new_ctx == NULL)
    { return NULL; }
    vector_push_back(RenderContext, &ctx->m_render_ctxs, new_ctx, &err);
    if (err != 0)
    { goto error; }
    return new_ctx;
error:
    DestroyRenderContext(new_ctx, ctx->m_instance);
    return NULL;
}

result_t VulkanContext_Draw(VulkanContext ctx, RenderContext rndr_ctx) {
    uint32_t imageIndex;
    result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    VkSubmitInfo submitInfo;
    VkPresentInfoKHR presentInfo;
    VkSwapchainKHR swapChains[] = { rndr_ctx->m_swapchain->m_swapchain };
    VkSemaphore waitSemaphores[] = { rndr_ctx->m_commands->m_imageavailable_sem };
    VkSemaphore signalSemaphores[] = { rndr_ctx->m_commands->m_renderfinished_sem };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    vkerr = vkWaitForFences(rndr_ctx->m_device->m_logical_device, 1, &rndr_ctx->m_commands->m_inflight_fen, VK_TRUE, UINT64_MAX);
    vkerr |= vkResetFences(rndr_ctx->m_device->m_logical_device, 1, &rndr_ctx->m_commands->m_inflight_fen);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    vkerr = vkAcquireNextImageKHR(
        rndr_ctx->m_device->m_logical_device, 
        rndr_ctx->m_swapchain->m_swapchain, 
        UINT64_MAX, 
        rndr_ctx->m_commands->m_imageavailable_sem, 
        VK_NULL_HANDLE, 
        &imageIndex);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    vkerr = vkResetCommandBuffer(rndr_ctx->m_commands->m_commandbuffer, 0);
    if (vkerr != VK_SUCCESS)
    { return -1; }
    err = RecordCommandBuffer(rndr_ctx, rndr_ctx->m_commands->m_commandbuffer, imageIndex);
    if (err != 0)
    { return -1; }

    memzero(submitInfo);
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &rndr_ctx->m_commands->m_commandbuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkerr = vkQueueSubmit(
        rndr_ctx->m_device->m_queues.data[GRAPHICS_QUEUE_INDEX], 
        1, 
        &submitInfo, 
        rndr_ctx->m_commands->m_inflight_fen);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    memzero(presentInfo);
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    vkerr = vkQueuePresentKHR(rndr_ctx->m_device->m_queues.data[GRAPHICS_QUEUE_INDEX], &presentInfo);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    return 0;
}