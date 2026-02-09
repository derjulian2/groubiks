
#ifndef GROUBIKS_COMMANDS_H
#define GROUBIKS_COMMANDS_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/dynarray.h>
#include <groubiks/renderer/vulkan_swapchain.h>
#include <groubiks/renderer/vulkan_device_context.h>
#include <groubiks/renderer/vulkan_graphics_pipeline.h>
#include <groubiks/renderer/vulkan_context.h>

declare_dynarray(VkFramebuffer, VkFramebuffer)

struct vk_command_context {
    VkCommandPool   m_command_pool;
    VkCommandBuffer m_command_buffer;
    VkSemaphore     m_image_available_sem;
    VkSemaphore     m_render_finished_sem;
    VkFence         m_inflight_fen;
};

struct vk_render_context {
    struct vk_command_context      m_command_context;
    struct vk_graphics_pipeline    m_graphics_pipeline;
    struct vk_swapchain            m_swapchain;
    VkSurfaceKHR                   m_surface;
    struct dynarray(VkFramebuffer) m_framebuffers;
};

#define vk_command_context_null \
(struct vk_command_context) { \
    .m_command_pool         = VK_NULL_HANDLE, \
    .m_command_buffer       = VK_NULL_HANDLE, \
    .m_image_available_sem  = VK_NULL_HANDLE, \
    .m_render_finished_sem  = VK_NULL_HANDLE, \
    .m_inflight_fen         = VK_NULL_HANDLE \
}

#define vk_render_context_null \
(struct vk_render_context) { \
    .m_command_context   = vk_command_context_null, \
    .m_graphics_pipeline = vk_graphics_pipeline_null, \
    .m_swapchain         = vk_swapchain_null, \
    .m_surface           = VK_NULL_HANDLE, \
    .m_framebuffers      = null_dynarray(VkFramebuffer) \
}

groubiks_result_t
vk_render_context_create(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext,
    struct vk_context* pContext
);

void
free_vk_render_context(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext,
    struct vk_context* pContext
);

groubiks_result_t
vk_render_context_setup_framebuffers(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext
);

groubiks_result_t
vk_render_context_record_commandbuffer(struct vk_render_context* pRenderContext, u32 imageIdx);

groubiks_result_t
vk_render_context_draw(struct vk_render_context* pRenderContext,
    struct vk_device_context* pDeviceContext
);

groubiks_result_t
vk_command_context_create(struct vk_command_context* pCommandContext,
    struct vk_device_context* pDeviceContext
);

void
free_vk_command_context(struct vk_command_context* pCommandContext, 
    struct vk_device_context* pDeviceContext
);

#endif