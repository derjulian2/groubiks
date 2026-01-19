
#ifndef GROUBIKS_COMMANDS_H
#define GROUBIKS_COMMANDS_H

#include <GLFW/glfw3.h>
#include <groubiks/renderer/ActiveDevice.h>

typedef struct {
    VkCommandPool m_commandpool;
    VkCommandBuffer m_commandbuffer;
    VkSemaphore m_imageavailable_sem;
    VkSemaphore m_renderfinished_sem;
    VkFence m_inflight_fen;
} VulkanCommands_t;
typedef VulkanCommands_t* VulkanCommands;

VulkanCommands CreateVulkanCommands(VulkanActiveDevice device);
void DestroyVulkanCommands(VulkanCommands commands, VulkanActiveDevice device);


declare_vector(VkFramebuffer);

typedef struct {
    VulkanActiveDevice m_device;
    VkSurfaceKHR m_surface;
    VulkanGraphicsPipeline m_pipeline;
    VulkanSwapChain m_swapchain;
    VulkanCommands m_commands;
    vector_t(VkFramebuffer) m_framebuffers;
} RenderContext_t;
typedef RenderContext_t* RenderContext;

declare_vector(RenderContext);

RenderContext CreateRenderContext(GLFWwindow* win, VkInstance instance, VulkanDevices dvcs);
result_t _setupSurface(RenderContext_t* rndr_ctx, GLFWwindow* win, VkInstance instance);
result_t _setupActiveDevice(RenderContext rndr_ctx, VkInstance instance, VulkanDevices dvcs);
result_t _setupFrameBuffers(RenderContext rndr_ctx);

void DestroyRenderContext(RenderContext_t* rndr_ctx, VkInstance instance);

result_t RecordCommandBuffer(RenderContext rndr_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex);
#endif