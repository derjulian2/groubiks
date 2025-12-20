
#include <groubiks/renderer/RenderContext.h>

define_vector(VkImage);
define_vector(VkImageView);
define_vector(RenderContext);
define_vector(VkSurfaceFormatKHR);
define_vector(VkPresentModeKHR);
define_vector(VkFramebuffer);

RenderContext CreateRenderContext(GLFWwindow* win, VkInstance instance, VulkanDevices dvcs) {
    RenderContext new_ctx = malloc(sizeof(RenderContext_t));
    if (new_ctx == NULL)
    { return NULL; }

    memzero(*new_ctx);
    if (_setupSurface(new_ctx, win, instance) ||
        _setupActiveDevice(new_ctx, instance, dvcs) ||
        ((new_ctx->m_swapchain = CreateSwapChain(new_ctx->m_device, new_ctx->m_surface, win)) == NULL) ||
        ((new_ctx->m_pipeline = CreateGraphicsPipeline(new_ctx->m_device->m_logical_device, new_ctx->m_swapchain)) == NULL) ||
        _setupFrameBuffers(new_ctx) ||
        ((new_ctx->m_commands = CreateVulkanCommands(new_ctx->m_device)) == NULL))
    { goto error; }
    log_info("setup new render-context.");
    return new_ctx;
error:
    log_error("failed to setup new render-context.");
    DestroyRenderContext(new_ctx, instance);
    return NULL;
}

GroubiksResult_t _setupSurface(RenderContext_t* rndr_ctx, GLFWwindow* win, VkInstance instance) {
    VkResult vkerr = glfwCreateWindowSurface(instance, win, NULL, &rndr_ctx->m_surface);
    if (vkerr != VK_SUCCESS)
    { log_error("failed to create window-surface."); return -1; }
    log_info("successfully created window-surface.");
    return 0;
}

GroubiksResult_t _setupActiveDevice(RenderContext rndr_ctx, VkInstance instance, VulkanDevices dvcs) {
    assert(rndr_ctx != NULL && instance != VK_NULL_HANDLE && dvcs != NULL);
    GroubiksResult_t err = 0;

    vector_for_each(VkPhysicalDevice, &dvcs->m_available_devices, device)
    {
        GroubiksResult_t hasExtensions = _deviceHasExtensions(*device, dvcs->m_device_extensions);
        if (hasExtensions == -1)
        { log_error("failed to setup suitable device."); return -1; }
        if (_isDeviceSuitable(*device) && hasExtensions)
        {
            VulkanActiveDevice active_device = CreateVulkanActiveDevice(*device, 
                    dvcs->m_device_extensions, 
                    rndr_ctx->m_surface);
            if (active_device == NULL)
            { log_error("failed to setup suitable device."); return -1; }
            rndr_ctx->m_device = active_device;
            log_info("found and setup suitable device for render-context.");
            break;
        }
    }
    return 0;
}

GroubiksResult_t _setupFrameBuffers(RenderContext rndr_ctx) {
    GroubiksResult_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    rndr_ctx->m_framebuffers = make_vector(VkFramebuffer, NULL, rndr_ctx->m_swapchain->m_swapchain_imageviews.size, &err);
    if (err != 0)
    { goto error; }
    vector_zero(VkFramebuffer, &rndr_ctx->m_framebuffers);

    vector_for_each(VkImageView, &rndr_ctx->m_swapchain->m_swapchain_imageviews, imageView) {
        int idx = vector_make_index(VkImageView, &rndr_ctx->m_swapchain->m_swapchain_imageviews, imageView);
        
        VkFramebufferCreateInfo createInfo;
        memzero(createInfo);
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = rndr_ctx->m_pipeline->m_renderpass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = imageView;
        createInfo.width = rndr_ctx->m_swapchain->m_swapchain_extent.width;
        createInfo.height = rndr_ctx->m_swapchain->m_swapchain_extent.height;
        createInfo.layers = 1;

        vkerr = vkCreateFramebuffer(
            rndr_ctx->m_device->m_logical_device, 
            &createInfo, 
            NULL, 
            &rndr_ctx->m_framebuffers.data[idx]);
        if (vkerr != VK_SUCCESS)
        { goto error; }
    }

    log_info("successfully setup framebuffers.");
    return 0;
error:
    log_error("failed to setup framebuffers.");
    vector_for_each(VkFramebuffer, &rndr_ctx->m_framebuffers, framebuffer)
    { vkDestroyFramebuffer(rndr_ctx->m_device->m_logical_device, *framebuffer, NULL); }
    free_vector(&rndr_ctx->m_framebuffers);
    return -1;
}

void DestroyRenderContext(RenderContext_t* rndr_ctx, VkInstance instance) {
    DestroySwapChain(rndr_ctx->m_swapchain, rndr_ctx->m_device->m_logical_device);
    DestroyVulkanGraphicsPipeline(rndr_ctx->m_pipeline, rndr_ctx->m_device->m_logical_device);
    vkDestroySurfaceKHR(instance, rndr_ctx->m_surface, NULL);
    vector_for_each(VkFramebuffer, &rndr_ctx->m_framebuffers, framebuffer)
    { vkDestroyFramebuffer(rndr_ctx->m_device->m_logical_device, *framebuffer, NULL); }
    free_vector(&rndr_ctx->m_framebuffers);
    DestroyVulkanCommands(rndr_ctx->m_commands, rndr_ctx->m_device);
    DestroyVulkanActiveDevice(rndr_ctx->m_device);
}


GroubiksResult_t RecordCommandBuffer(RenderContext rndr_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    VkResult vkerr = VK_SUCCESS;
    VkRenderPassBeginInfo renderPassBeginInfo;
    VkClearValue clearValue = { {{ 0.0f, 0.0f, 0.0f, 1.0f } } };
    VkViewport viewport;
    VkRect2D scissors;

    memzero(commandBufferBeginInfo);
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkerr = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    memzero(renderPassBeginInfo);
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = rndr_ctx->m_pipeline->m_renderpass;
    renderPassBeginInfo.framebuffer = rndr_ctx->m_framebuffers.data[imageIndex];
    renderPassBeginInfo.renderArea.offset = (VkOffset2D){ 0, 0 };
    renderPassBeginInfo.renderArea.extent = rndr_ctx->m_swapchain->m_swapchain_extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    memzero(viewport);
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)rndr_ctx->m_swapchain->m_swapchain_extent.width;
    viewport.height = (float)rndr_ctx->m_swapchain->m_swapchain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    memzero(scissors);
    scissors.offset = (VkOffset2D){ 0, 0};
    scissors.extent = rndr_ctx->m_swapchain->m_swapchain_extent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rndr_ctx->m_pipeline->m_pipeline);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissors);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    vkerr = vkEndCommandBuffer(commandBuffer);
    if (vkerr != VK_SUCCESS)
    { return -1; }

    return 0;
}