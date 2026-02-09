
#include <groubiks/renderer/vulkan_render_context.h>

define_dynarray(VkFramebuffer, VkFramebuffer, 
    (comp, NULL)
);


groubiks_result_t
vk_render_context_create(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext,
    struct vk_context* pContext)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;

    shaderc_compiler_t shaderComp = NULL;

    *pRenderContext = vk_render_context_null;
    pRenderContext->m_surface = pContext->m_surface;

    shaderComp = shaderc_compiler_initialize();
    check(shaderComp != NULL, err = GROUBIKS_SHADER_ERROR);
    
    struct dynarray(shadermodule) shaderModules = null_dynarray(shadermodule);
    dynarray_resize(shadermodule, &shaderModules, 2, &dynarrayErr);
    check(dynarrayErr != DYNARRAY_ERROR, err = GROUBIKS_BAD_ALLOC);

    struct vk_shadermodule vertMod = vk_shadermodule_null, fragMod = vk_shadermodule_null;
    vertMod.m_shadertype = shaderc_vertex_shader, fragMod.m_shadertype = shaderc_fragment_shader;
    err = vk_create_shadermodule_from_glsl(&vertMod, 
        GROUBIKS_VERTEX_SHADER_SOURCE_PATH, 
        pDeviceContext->m_logical_device, 
        shaderComp
    );
    check(err == GROUBIKS_SUCCESS);
    err = vk_create_shadermodule_from_glsl(&fragMod, 
        GROUBIKS_FRAGMENT_SHADER_SOURCE_PATH, 
        pDeviceContext->m_logical_device, 
        shaderComp
    );
    check(err == GROUBIKS_SUCCESS);
    *dynarray_at(&shaderModules, 0) = vertMod;
    *dynarray_at(&shaderModules, 1) = fragMod;

    err = vk_swapchain_create(&pRenderContext->m_swapchain,
        pDeviceContext,
        pRenderContext->m_surface,
        pContext->m_window
    );
    check(err == GROUBIKS_SUCCESS);

    err = vk_graphics_pipeline_create(&pRenderContext->m_graphics_pipeline, 
        pDeviceContext,
        &pRenderContext->m_swapchain, 
        &shaderModules
    );
    check(err == GROUBIKS_SUCCESS);

    err = vk_render_context_setup_framebuffers(pRenderContext, pDeviceContext);
    check(err == GROUBIKS_SUCCESS);

    err = vk_command_context_create(&pRenderContext->m_command_context, pDeviceContext);
    check(err == GROUBIKS_SUCCESS);

    log_info(VK_RENDER_CONTEXT_CREATE_SUCCESS_STR);
    cleanup(
        dynarray_for_each(shadermodule, &shaderModules, mod) {
            free_vk_shadermodule(mod, pDeviceContext->m_logical_device);
        }
        free_dynarray(shadermodule, &shaderModules);
        shaderc_compiler_release(shaderComp);
    )
    return err;
    except(
        free_vk_render_context(pRenderContext, pDeviceContext, pContext);
        log_error(VK_RENDER_CONTEXT_CREATE_FAIL_STR);
    )
}


void
free_vk_render_context(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext,
    struct vk_context* pContext)
{
    dynarray_for_each(VkFramebuffer, &pRenderContext->m_framebuffers, frameBuf) {
        vkDestroyFramebuffer(pDeviceContext->m_logical_device, *frameBuf, NULL);
    }
    free_dynarray(VkFramebuffer, &pRenderContext->m_framebuffers);
    free_vk_command_context(&pRenderContext->m_command_context, pDeviceContext);
    free_vk_swapchain(&pRenderContext->m_swapchain, pDeviceContext->m_logical_device);
    free_vk_graphics_pipeline(&pRenderContext->m_graphics_pipeline, pDeviceContext->m_logical_device);
    vkDestroySurfaceKHR(pContext->m_instance, pRenderContext->m_surface, NULL);
}


groubiks_result_t
vk_render_context_setup_framebuffers(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext)
{
    groubiks_result_t err         = GROUBIKS_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkFramebufferCreateInfo frameBufCreateInfo;

    dynarray_resize(VkFramebuffer, 
        &pRenderContext->m_framebuffers, 
        pRenderContext->m_swapchain.m_imageviews.size, 
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    dynarray_for_each(VkImageView, &pRenderContext->m_swapchain.m_imageviews, imageView) {
        dynarray_index_t idx = dynarray_index(&pRenderContext->m_swapchain.m_imageviews, imageView);
        vk_fill_struct_framebuffer_createinfo(&frameBufCreateInfo, 
            pRenderContext->m_graphics_pipeline.m_renderpass, 
            1, imageView, 
            pRenderContext->m_swapchain.m_extent
        );
        vkErr = vkCreateFramebuffer(pDeviceContext->m_logical_device, 
            &frameBufCreateInfo, 
            NULL,
            dynarray_at(&pRenderContext->m_framebuffers, idx)
        );
        check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    }

    log_info(VK_RENDER_CONTEXT_FRAMEBUF_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_RENDER_CONTEXT_FRAMEBUF_FAIL_STR);
    )
}


groubiks_result_t
vk_render_context_record_commandbuffer(struct vk_render_context* pRenderContext, u32 imageIdx)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;

    VkCommandBufferBeginInfo commandBufferBeginInfo;
    VkRenderPassBeginInfo renderPassBeginInfo;
    VkViewport viewport;
    VkRect2D scissors;

    VkClearValue clearValue = { {{ 0.0f, 0.0f, 0.0f, 1.0f } } };

    vk_fill_struct_commandbuffer_begininfo(&commandBufferBeginInfo);
    vkErr = vkBeginCommandBuffer(pRenderContext->m_command_context.m_command_buffer, &commandBufferBeginInfo);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    vk_fill_struct_renderpass_begininfo(&renderPassBeginInfo, 
        pRenderContext->m_graphics_pipeline.m_renderpass, 
        pRenderContext->m_framebuffers.data[imageIdx], 
        (VkOffset2D){ 0, 0 }, 
        pRenderContext->m_swapchain.m_extent, 
        1, &clearValue
    );
    vk_fill_struct_viewport_and_scissors(&viewport, &scissors, &pRenderContext->m_swapchain.m_extent);

    vkCmdBeginRenderPass(pRenderContext->m_command_context.m_command_buffer, 
        &renderPassBeginInfo,
        VK_SUBPASS_CONTENTS_INLINE
    );
    vkCmdBindPipeline(pRenderContext->m_command_context.m_command_buffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        pRenderContext->m_graphics_pipeline.m_pipeline
    );
    vkCmdSetViewport(pRenderContext->m_command_context.m_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(pRenderContext->m_command_context.m_command_buffer, 0, 1, &scissors);
    vkCmdDraw(pRenderContext->m_command_context.m_command_buffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(pRenderContext->m_command_context.m_command_buffer);

    vkErr = vkEndCommandBuffer(pRenderContext->m_command_context.m_command_buffer);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    return err;
    except(err,
        log_error(VK_RENDER_CONTEXT_RECORD_FAIL_STR);
    )
}

groubiks_result_t
vk_render_context_draw(struct vk_render_context *pRenderContext,
    struct vk_device_context* pDeviceContext) 
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;

    VkSubmitInfo submitInfo;
    VkPresentInfoKHR presentInfo;
    VkSwapchainKHR swapChains[] = { pRenderContext->m_swapchain.m_swapchain };
    VkSemaphore waitSemaphores[] = { pRenderContext->m_command_context.m_image_available_sem };
    VkSemaphore signalSemaphores[] = { pRenderContext->m_command_context.m_render_finished_sem };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    uint32_t imageIndex;

    vkErr = vkWaitForFences(pDeviceContext->m_logical_device, 1, &pRenderContext->m_command_context.m_inflight_fen, VK_TRUE, UINT64_MAX);
    vkErr |= vkResetFences(pDeviceContext->m_logical_device, 1, &pRenderContext->m_command_context.m_inflight_fen);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    vkErr = vkAcquireNextImageKHR(pDeviceContext->m_logical_device, 
        pRenderContext->m_swapchain.m_swapchain, 
        UINT64_MAX, 
        pRenderContext->m_command_context.m_image_available_sem, 
        VK_NULL_HANDLE, 
        &imageIndex
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    vkErr = vkResetCommandBuffer(pRenderContext->m_command_context.m_command_buffer, 0);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    err = vk_render_context_record_commandbuffer(pRenderContext, imageIndex);
    check(err == GROUBIKS_SUCCESS);

    vk_fill_struct_submitinfo(&submitInfo,
        1, &waitSemaphores[0], &waitStages[0],
        1, &signalSemaphores[0],
        1, &pRenderContext->m_command_context.m_command_buffer
    );
    vkErr = vkQueueSubmit(pDeviceContext->m_queues.m_graphics_queue, 
        1, &submitInfo, 
        pRenderContext->m_command_context.m_inflight_fen
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    vk_fill_struct_presentinfo(&presentInfo,
        1, &signalSemaphores[0],
        1, &swapChains[0],
        &imageIndex
    );
    vkErr = vkQueuePresentKHR(pDeviceContext->m_queues.m_graphics_queue, &presentInfo);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    return err;
    except(err,
        log_error(VK_RENDER_CONTEXT_DRAW_FAIL_STR);
    )
}

groubiks_result_t
vk_command_context_create(struct vk_command_context* pCommandContext,
    struct vk_device_context* pDeviceContext)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    VkCommandBufferAllocateInfo commandBufferAllocInfo;
    VkSemaphoreCreateInfo semCreateInfo;
    VkFenceCreateInfo fenCreateInfo;

    *pCommandContext = vk_command_context_null;

    vk_fill_struct_commandpool_createinfo(&commandPoolCreateInfo, pDeviceContext->m_qfis.m_graphics_family.value);
    vkErr = vkCreateCommandPool(pDeviceContext->m_logical_device, &commandPoolCreateInfo, NULL, &pCommandContext->m_command_pool);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    vk_fill_struct_commandbuffer_allocinfo(&commandBufferAllocInfo, pCommandContext->m_command_pool, 1);
    vkErr = vkAllocateCommandBuffers(pDeviceContext->m_logical_device, &commandBufferAllocInfo, &pCommandContext->m_command_buffer);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    vk_fill_struct_sem_createinfo(&semCreateInfo);
    vk_fill_struct_fen_createinfo(&fenCreateInfo);
    vkErr = vkCreateSemaphore(pDeviceContext->m_logical_device, &semCreateInfo, NULL, &pCommandContext->m_image_available_sem);
    vkErr |= vkCreateSemaphore(pDeviceContext->m_logical_device, &semCreateInfo, NULL, &pCommandContext->m_render_finished_sem);
    vkErr |= vkCreateFence(pDeviceContext->m_logical_device, &fenCreateInfo, NULL, &pCommandContext->m_inflight_fen);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_CMD_CONTEXT_CREATE_SUCCESS_STR);
    return err;
    except(err,
        free_vk_command_context(pCommandContext, pDeviceContext);
        log_error(VK_CMD_CONTEXT_CREATE_FAIL_STR)
    )
}


void
free_vk_command_context(struct vk_command_context* pCommandContext,
    struct vk_device_context* pDeviceContext)
{
    if (pCommandContext->m_command_pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(pDeviceContext->m_logical_device, pCommandContext->m_command_pool, NULL);
    }
    if (pCommandContext->m_image_available_sem != VK_NULL_HANDLE) {
        vkDestroySemaphore(pDeviceContext->m_logical_device, pCommandContext->m_image_available_sem, NULL);
    }
    if (pCommandContext->m_render_finished_sem != VK_NULL_HANDLE) {
        vkDestroySemaphore(pDeviceContext->m_logical_device, pCommandContext->m_render_finished_sem, NULL);    
    }
    if (pCommandContext->m_inflight_fen != VK_NULL_HANDLE) {
        vkDestroyFence(pDeviceContext->m_logical_device, pCommandContext->m_inflight_fen, NULL);
    }
}