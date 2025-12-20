
#ifndef GROUBIKS_PRESENTATION_H
#define GROUBIKS_PRESENTATION_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/vector.h>
#include <groubiks/utility/log.h>
#include <groubiks/renderer/ActiveDevice.h>
#include <groubiks/renderer/Devices.h>
#include <groubiks/renderer/SwapChain.h>
#include <groubiks/renderer/GraphicsPipeline.h>
#include <groubiks/renderer/Commands.h>

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
GroubiksResult_t _setupSurface(RenderContext_t* rndr_ctx, GLFWwindow* win, VkInstance instance);
GroubiksResult_t _setupActiveDevice(RenderContext rndr_ctx, VkInstance instance, VulkanDevices dvcs);
GroubiksResult_t _setupFrameBuffers(RenderContext rndr_ctx);

void DestroyRenderContext(RenderContext_t* rndr_ctx, VkInstance instance);

GroubiksResult_t RecordCommandBuffer(RenderContext rndr_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex);

#endif