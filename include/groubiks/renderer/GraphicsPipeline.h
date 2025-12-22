
#ifndef GROUBIKS_GRAPHICSPIPELINE_H
#define GROUBIKS_GRAPHICSPIPELINE_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/log.h>
#include <groubiks/utility/common.h>
#include <groubiks/renderer/ActiveDevice.h>
#include <groubiks/renderer/Devices.h>
#include <groubiks/renderer/SwapChain.h>
#include <groubiks/compile_config.h>

/* this will only load a precompiled SPIR-V shader. */
/* i probably want compilation from within my program */
typedef struct {
    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
    VkRenderPass m_renderpass;
} VulkanGraphicsPipeline_t;
typedef VulkanGraphicsPipeline_t* VulkanGraphicsPipeline;

VulkanGraphicsPipeline CreateGraphicsPipeline(VkDevice device, VulkanSwapChain swapchain);
result_t _setupRenderPass(VulkanGraphicsPipeline pipeln, VkDevice device, VulkanSwapChain swapchain);
VkShaderModule _loadShaderModule(const char* codePath, VkDevice device);
void DestroyVulkanGraphicsPipeline(VulkanGraphicsPipeline pipeln, VkDevice device);

#endif