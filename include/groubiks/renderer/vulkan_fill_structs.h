
#ifndef GROUBIKS_VULKAN_FILL_STRUCTS_H
#define GROUBIKS_VULKAN_FILL_STRUCTS_H

/**
 * @file vulkan_fill_structs.h
 * @date 07/02/26
 * @author Julian Benzel
 * @brief utility-methods to make filling out
 *        vulkan-structs look less ugly in my code.
 *        credits to https://vulkan-tutorial.com
 */

#include <vulkan/vulkan.h>
#include <groubiks/compile_config.h>
#include <groubiks/utility/common.h>

void
vk_fill_struct_instance_createinfo(VkInstanceCreateInfo* pCreateInfo,
    VkApplicationInfo* pAppInfo,
    const char*const* ppEnabledLayerNames,
    u32 numLayers,
    const char*const* ppEnabledExtensionNames,
    u32 numExtensions
);

void 
vk_fill_struct_instance_appinfo(VkApplicationInfo* pAppInfo);

void
vk_fill_struct_debugmessenger_createinfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    PFN_vkDebugUtilsMessengerCallbackEXT callback
);

void 
vk_fill_struct_swapchain_createinfo(VkSwapchainCreateInfoKHR* pCreateInfo,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR* pFormat,
    VkPresentModeKHR mode,
    VkExtent2D* pExtent,
    VkSurfaceCapabilitiesKHR* pCapabilities,
    u32* pQfis,
    u32  qfiCount
);


void 
vk_fill_struct_shadermod_createinfo(VkShaderModuleCreateInfo* pCreateInfo,
    size_t codeSize,
    const u32* const pCode
);


void
vk_fill_struct_renderpass_createinfo(VkRenderPassCreateInfo* pCreateInfo,
    VkSubpassDescription* pSubPassDescr,
    u32 subPassCount,
    VkSubpassDependency* pSubPassDep,
    u32 depCount,
    VkAttachmentDescription* pColorAttachment,
    u32 attachmentCount
);

void
vk_fill_struct_color_attachment(VkAttachmentDescription* pColorAttachment,
    VkFormat format,
    VkAttachmentReference* pAttachmentRef
);

void
vk_fill_struct_subpass(VkSubpassDescription* pSubPassDescr,
    VkSubpassDependency* pSubPassDep,
    VkAttachmentReference* pAttachmentRef
);

void
vk_fill_struct_shaderstage_createinfo(VkPipelineShaderStageCreateInfo* pCreateInfo,
    VkShaderStageFlagBits stageBit,
    VkShaderModule shaderMod
);

void
vk_fill_struct_viewportstate_createinfo(VkPipelineViewportStateCreateInfo* pCreateInfo,
    u32 viewportCount, VkViewport* pViewports,
    u32 scissorCount, VkRect2D* pScissors
);

void
vk_fill_struct_colors_createinfo(VkPipelineMultisampleStateCreateInfo* pCreateInfo,
    VkPipelineColorBlendStateCreateInfo* pColorBlendCreateInfo,
    VkPipelineColorBlendAttachmentState* pColorBlendAttachmentState
);

void
vk_fill_struct_inputassembly_createinfo(VkPipelineInputAssemblyStateCreateInfo* pCreateInfo);

void
vk_fill_struct_vertexinput_createinfo(VkPipelineVertexInputStateCreateInfo* pCreateInfo);

void
vk_fill_struct_rasterizer_createInfo(VkPipelineRasterizationStateCreateInfo* pCreateInfo);

void
vk_fill_struct_viewport_and_scissors(VkViewport* pViewport, 
    VkRect2D* pScissors,
    VkExtent2D* pExtent
);

void
vk_fill_struct_dynamicstate_createinfo(VkPipelineDynamicStateCreateInfo* pCreateInfo,
    u32 dynamicStateCount, VkDynamicState* pDynamicStates
);

void
vk_fill_struct_pipeline_layout_createinfo(VkPipelineLayoutCreateInfo* pCreateInfo);

void
vk_fill_struct_pipeline_createinfo(VkGraphicsPipelineCreateInfo* pCreateInfo,
    u32 numShaderStages,
    VkPipelineShaderStageCreateInfo* pShaderStageCreateInfos,
    VkPipelineVertexInputStateCreateInfo* pVertexInputStateCreateInfo,
    VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyStateCreateInfo,
    VkPipelineViewportStateCreateInfo* pViewportStateCreateInfo,
    VkPipelineRasterizationStateCreateInfo* pRasterizerCreateInfo,
    VkPipelineMultisampleStateCreateInfo* pMultiSamplingCreateInfo,
    VkPipelineColorBlendStateCreateInfo* pColorBlendStateCreateInfo,
    VkPipelineDynamicStateCreateInfo* pDynamicStateCreateInfo,
    VkPipelineLayout layout,
    VkRenderPass renderPass
);


void
vk_fill_struct_imageview_createinfo(VkImageViewCreateInfo* pCreateInfo,
    VkImage image,
    VkFormat format
);

void
vk_fill_struct_device_createinfo(VkDeviceCreateInfo* pCreateInfo,
    const VkPhysicalDeviceFeatures* pFeatures,
    const VkDeviceQueueCreateInfo* pCreateInfos,
    u32 createInfoCount,
    const char* const* pLayers,
    u32 layerCount,
    const char* const* pExtensions,
    u32 extCount
);

void
vk_fill_struct_devicequeue_createinfo(VkDeviceQueueCreateInfo* pCreateInfo,
    u32 qFamilyIdx,
    const float* qPriority
);

#endif