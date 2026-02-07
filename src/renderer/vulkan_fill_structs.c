
#include <groubiks/renderer/vulkan_fill_structs.h>

void
vk_fill_struct_instance_createinfo(VkInstanceCreateInfo* pCreateInfo,
    VkApplicationInfo* pAppInfo,
    const char*const* ppEnabledLayerNames,
    u32 numLayers,
    const char*const* ppEnabledExtensionNames,
    u32 numExtensions)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    pCreateInfo->pApplicationInfo = pAppInfo;
    pCreateInfo->ppEnabledLayerNames = ppEnabledLayerNames;
    pCreateInfo->enabledLayerCount = numLayers;
    pCreateInfo->ppEnabledExtensionNames = ppEnabledExtensionNames;
    pCreateInfo->enabledExtensionCount = numExtensions;
}


void 
vk_fill_struct_instance_appinfo(VkApplicationInfo* pAppInfo)
{
    memzero(*pAppInfo);
    pAppInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    pAppInfo->pApplicationName = DEFAULT_APPLICATION_NAME;
    pAppInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    pAppInfo->pEngineName = "No Engine";
    pAppInfo->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    pAppInfo->apiVersion = VK_API_VERSION_1_0;
}


void
vk_fill_struct_debugmessenger_createinfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    pCreateInfo->messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    pCreateInfo->messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    pCreateInfo->pfnUserCallback = callback;
}


void
vk_fill_struct_color_attachment(VkAttachmentDescription* pColorAttachment,
    VkFormat format,
    VkAttachmentReference* pAttachmentRef)
{
    memzero(*pColorAttachment);
    pColorAttachment->format = format;
    pColorAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
    pColorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    pColorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    pColorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    pColorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    pColorAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    pColorAttachment->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    memzero(*pAttachmentRef);
    pAttachmentRef->attachment = 0;
    pAttachmentRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}


void
vk_fill_struct_subpass(VkSubpassDescription* pSubPassDescr,
    VkSubpassDependency* pSubPassDep,
    VkAttachmentReference* pAttachmentRef)
{
    memzero(*pSubPassDescr);
    pSubPassDescr->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    pSubPassDescr->colorAttachmentCount = 1;
    pSubPassDescr->pColorAttachments = pAttachmentRef;

    memzero(*pSubPassDep);
    pSubPassDep->srcSubpass = VK_SUBPASS_EXTERNAL;
    pSubPassDep->dstSubpass = 0;
    pSubPassDep->srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    pSubPassDep->srcAccessMask = 0;
    pSubPassDep->dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    pSubPassDep->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
}


void
vk_fill_struct_renderpass_createinfo(VkRenderPassCreateInfo* pCreateInfo,
    VkSubpassDescription* pSubPassDescr,
    u32 subPassCount,
    VkSubpassDependency* pSubPassDep,
    u32 depCount,
    VkAttachmentDescription* pColorAttachment,
    u32 attachmentCount)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    pCreateInfo->attachmentCount = attachmentCount;
    pCreateInfo->pAttachments = pColorAttachment;
    pCreateInfo->subpassCount = subPassCount;
    pCreateInfo->pSubpasses = pSubPassDescr;
    pCreateInfo->dependencyCount = depCount;
    pCreateInfo->pDependencies = pSubPassDep;
}


void
vk_fill_struct_shaderstage_createinfo(VkPipelineShaderStageCreateInfo* pCreateInfo,
    VkShaderStageFlagBits stageBit,
    VkShaderModule shaderMod)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pCreateInfo->stage = stageBit;
    pCreateInfo->module = shaderMod;
    pCreateInfo->pName = "main";
}

void
vk_fill_struct_viewportstate_createinfo(VkPipelineViewportStateCreateInfo* pCreateInfo,
    u32 viewportCount, VkViewport* pViewports,
    u32 scissorCount, VkRect2D* pScissors)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pCreateInfo->viewportCount = viewportCount;
    pCreateInfo->pViewports = pViewports;
    pCreateInfo->scissorCount = scissorCount;
    pCreateInfo->pScissors = pScissors;
}

void
vk_fill_struct_colors_createinfo(VkPipelineMultisampleStateCreateInfo* pCreateInfo,
    VkPipelineColorBlendStateCreateInfo* pColorBlendCreateInfo,
    VkPipelineColorBlendAttachmentState* pColorBlendAttachmentState
)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pCreateInfo->sampleShadingEnable = VK_FALSE;
    pCreateInfo->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    memzero(*pColorBlendAttachmentState);
    pColorBlendAttachmentState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pColorBlendAttachmentState->blendEnable = VK_FALSE;

    memzero(*pColorBlendCreateInfo);
    pColorBlendCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pColorBlendCreateInfo->logicOpEnable = VK_FALSE;
    pColorBlendCreateInfo->logicOp = VK_LOGIC_OP_COPY;
    pColorBlendCreateInfo->attachmentCount = 1;
    pColorBlendCreateInfo->pAttachments = pColorBlendAttachmentState;
}

void
vk_fill_struct_inputassembly_createinfo(VkPipelineInputAssemblyStateCreateInfo* pCreateInfo) {
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pCreateInfo->primitiveRestartEnable = VK_FALSE;
    pCreateInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void
vk_fill_struct_vertexinput_createinfo(VkPipelineVertexInputStateCreateInfo* pCreateInfo) {
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pCreateInfo->vertexBindingDescriptionCount = 0;
    pCreateInfo->vertexAttributeDescriptionCount = 0;
}

void
vk_fill_struct_rasterizer_createInfo(VkPipelineRasterizationStateCreateInfo* pCreateInfo) {
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pCreateInfo->depthClampEnable = VK_FALSE;
    pCreateInfo->rasterizerDiscardEnable = VK_FALSE;
    pCreateInfo->polygonMode = VK_POLYGON_MODE_FILL;
    pCreateInfo->lineWidth = 1.0f;
    pCreateInfo->cullMode = VK_CULL_MODE_BACK_BIT;
    pCreateInfo->frontFace = VK_FRONT_FACE_CLOCKWISE;
    pCreateInfo->depthBiasEnable = VK_FALSE;
}

void
vk_fill_struct_viewport_and_scissors(VkViewport* pViewport, 
    VkRect2D* pScissors,
    VkExtent2D* pExtent)
{
    memzero(*pViewport);
    pViewport->x = 0.0f;
    pViewport->y = 0.0f;
    pViewport->width = (float) pExtent->width;
    pViewport->height = (float) pExtent->height;
    pViewport->minDepth = 0.0f;
    pViewport->maxDepth = 1.0f;

    memzero(*pScissors);
    pScissors->offset = (VkOffset2D){0, 0};
    pScissors->extent = *pExtent;
}

void
vk_fill_struct_dynamicstate_createinfo(VkPipelineDynamicStateCreateInfo* pCreateInfo,
    u32 dynamicStateCount, VkDynamicState* pDynamicStates)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pCreateInfo->dynamicStateCount = dynamicStateCount;
    pCreateInfo->pDynamicStates = pDynamicStates;
}

void
vk_fill_struct_pipeline_layout_createinfo(VkPipelineLayoutCreateInfo *pCreateInfo) {
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}

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
    VkRenderPass renderPass)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pCreateInfo->stageCount = numShaderStages;
    pCreateInfo->pStages = pShaderStageCreateInfos;
    pCreateInfo->pVertexInputState = pVertexInputStateCreateInfo;
    pCreateInfo->pInputAssemblyState = pInputAssemblyStateCreateInfo;
    pCreateInfo->pViewportState = pViewportStateCreateInfo;
    pCreateInfo->pRasterizationState = pRasterizerCreateInfo;
    pCreateInfo->pMultisampleState = pMultiSamplingCreateInfo;
    pCreateInfo->pDepthStencilState = NULL;
    pCreateInfo->pColorBlendState = pColorBlendStateCreateInfo;
    pCreateInfo->pDynamicState = pDynamicStateCreateInfo;

    pCreateInfo->layout = layout;
    pCreateInfo->renderPass = renderPass;
    pCreateInfo->subpass = 0;
    pCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
    pCreateInfo->basePipelineIndex = -1;
}


void 
vk_fill_struct_swapchain_createinfo(VkSwapchainCreateInfoKHR* pCreateInfo,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR* pFormat,
    VkPresentModeKHR mode,
    VkExtent2D* pExtent,
    VkSurfaceCapabilitiesKHR* pCapabilities,
    u32* pQfis,
    u32  qfiCount)
{
    u32 imageCount;

    memzero(*pCreateInfo);
    imageCount = pCapabilities->minImageCount + 1;
    if (pCapabilities->maxImageCount != 0 &&
        (pCapabilities->minImageCount + 1) > pCapabilities->maxImageCount)
    { imageCount = pCapabilities->maxImageCount; }

    pCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    pCreateInfo->surface = surface;
    pCreateInfo->minImageCount = imageCount;
    pCreateInfo->imageFormat = pFormat->format;
    pCreateInfo->imageColorSpace = pFormat->colorSpace;
    pCreateInfo->imageExtent = *pExtent;
    pCreateInfo->imageArrayLayers = 1;
    pCreateInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    pCreateInfo->preTransform = pCapabilities->currentTransform;
    pCreateInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    pCreateInfo->presentMode = mode;
    pCreateInfo->clipped = VK_TRUE;

    if (qfiCount == 2 && pQfis[0] != pQfis[1]) {
        pCreateInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        pCreateInfo->queueFamilyIndexCount = 2;
        pCreateInfo->pQueueFamilyIndices = pQfis;
    }
    else {
        pCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
}


void vk_fill_struct_shadermod_createinfo(VkShaderModuleCreateInfo* pCreateInfo,
    size_t codeSize,
    const u32* const pCode)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    pCreateInfo->codeSize = codeSize;
    pCreateInfo->pCode = pCode;
}


void
vk_fill_struct_imageview_createinfo(VkImageViewCreateInfo* pCreateInfo,
    VkImage image,
    VkFormat format)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    pCreateInfo->image = image;
    pCreateInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    pCreateInfo->format = format;
    pCreateInfo->components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    pCreateInfo->subresourceRange.baseMipLevel = 0;
    pCreateInfo->subresourceRange.levelCount = 1;
    pCreateInfo->subresourceRange.baseArrayLayer = 0;
    pCreateInfo->subresourceRange.layerCount = 1;
}


void
vk_fill_struct_device_createinfo(VkDeviceCreateInfo* pCreateInfo,
    const VkPhysicalDeviceFeatures* pFeatures,
    const VkDeviceQueueCreateInfo* pCreateInfos,
    u32 createInfoCount,
    const char* const* pLayers,
    u32 layerCount,
    const char* const* pExtensions,
    u32 extCount)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    pCreateInfo->pQueueCreateInfos = pCreateInfos;
    pCreateInfo->queueCreateInfoCount = createInfoCount;
    pCreateInfo->pEnabledFeatures = pFeatures;
    pCreateInfo->enabledLayerCount = layerCount;
    pCreateInfo->ppEnabledLayerNames = pLayers;
    pCreateInfo->enabledExtensionCount = extCount;
    pCreateInfo->ppEnabledExtensionNames = pExtensions;
}


void
vk_fill_struct_devicequeue_createinfo(VkDeviceQueueCreateInfo* pCreateInfo,
    u32 qFamilyIdx,
    const float* qPriority)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    pCreateInfo->queueFamilyIndex = qFamilyIdx;
    pCreateInfo->queueCount = 1;
    pCreateInfo->pQueuePriorities = qPriority;
}