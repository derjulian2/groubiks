
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
#include <shaderc/shaderc.h>
#include <groubiks/utility/log.h>
#include <groubiks/utility/common.h>
#include <groubiks/renderer/vulkan_fill_structs.h>
#include <groubiks/renderer/vulkan_device_context.h>
#include <groubiks/renderer/vulkan_swapchain.h>
#include <groubiks/compile_config.h>

struct vk_shadermodule {
    shaderc_shader_kind  m_shadertype;
    VkShaderModule       m_module;
    struct dynarray(u32) m_data;
};

declare_dynarray(struct vk_shadermodule, shadermodule)

struct vk_graphics_pipeline {
    VkPipeline       m_pipeline;
    VkPipelineLayout m_layout;
    VkRenderPass     m_renderpass;
};

#define vk_shadermodule_null \
(struct vk_shadermodule) { \
    .m_shadertype = (shaderc_shader_kind)0, \
    .m_module     = VK_NULL_HANDLE, \
    .m_data       = null_dynarray(u32) \
}

#define vk_graphics_pipeline_null \
(struct vk_graphics_pipeline) { \
    .m_pipeline   = VK_NULL_HANDLE, \
    .m_layout     = VK_NULL_HANDLE, \
    .m_renderpass = VK_NULL_HANDLE \
}

groubiks_result_t
vk_create_shadermodule_from_spirv(struct vk_shadermodule* pShaderMod,
    const char* const srcPath,
    VkDevice device
);

groubiks_result_t
vk_create_shadermodule_from_glsl(struct vk_shadermodule* pShaderMod,
    const char* const srcPath,
    VkDevice device,
    shaderc_compiler_t* pCompiler
);

void
free_vk_shadermodule(struct vk_shadermodule* pShaderMod,
    VkDevice device
);

groubiks_result_t
vk_graphics_pipeline_create(struct vk_graphics_pipeline* pPipeline, 
    struct vk_device_context* pDeviceContext,
    struct vk_swapchain* pSwapChain,
    struct dynarray(shadermodule)* pShaderModules
);

groubiks_result_t
vk_graphics_pipeline_setup_renderpass(struct vk_graphics_pipeline* pPipeline,
    struct vk_swapchain* pSwapChain,
    VkDevice device
);

void
free_vk_graphics_pipeline(struct vk_graphics_pipeline* pPipeline, 
    VkDevice device
);

#endif