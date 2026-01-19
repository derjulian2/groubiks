
#ifndef GROUBIKS_GRAPHICS_PIPELINE_H
#define GROUBIKS_GRAPHICS_PIPELINE_H

/**
 * @file vulkan_graphics_pipeline.h
 * @date 19/01/26
 * @author Julian Benzel
 * @brief interface to a vulkan-graphics-pipeline. 
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h>
#include <groubiks/utility/log.h>
#include <groubiks/utility/common.h>
#include <groubiks/renderer/vulkan_device_context.h>
#include <groubiks/renderer/vulkan_swapchain.h>
#include <groubiks/compile_config.h>

enum vk_shader_type {
    VERTEX,
    FRAGMENT
};

struct vk_graphics_pipeline {
    VkPipeline       m_pipeline;
    VkPipelineLayout m_layout;
    VkRenderPass     m_renderpass;
};

#define vk_graphics_pipeline_null \
(struct vk_graphics_pipeline) { \
    .m_pipeline   = VK_NULL_HANDLE, \
    .m_layout     = VK_NULL_HANDLE, \
    .m_renderpass = VK_NULL_HANDLE \
}

groubiks_result_t
vk_graphics_pipeline_create(struct vk_graphics_pipeline* pPipeline, 
    struct vk_swapchain* pSwapChain
);

groubiks_result_t
vk_graphics_pipeline_setup_renderpass(struct vk_graphics_pipeline* pPipeline,
    struct vk_swapchain* pSwapChain,
    VkDevice device
);

groubiks_result_t
vk_graphics_pipeline_compile_shadermodule(struct vk_graphics_pipeline* pPipeline);

groubiks_result_t
vk_graphics_pipeline_load_shadermodule(struct vk_graphics_pipeline* pPipeline);

void
free_vk_graphics_pipeline(struct vk_graphics_pipeline* pPipeline, 
    VkDevice device
);

#endif