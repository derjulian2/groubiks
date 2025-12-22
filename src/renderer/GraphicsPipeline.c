
#include <groubiks/renderer/GraphicsPipeline.h>

VulkanGraphicsPipeline CreateGraphicsPipeline(VkDevice device, VulkanSwapChain swapchain) {
    result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    VkShaderModule vertexMod = VK_NULL_HANDLE, fragmentMod = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo, fragShaderStageCreateInfo;
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo;
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
    VkPipelineLayoutCreateInfo layoutCreateInfo;
    VkGraphicsPipelineCreateInfo pipelineCreateInfo;
    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo;
    VkViewport viewport;
    VkRect2D scissors;
    
    VulkanGraphicsPipeline pipeln = malloc(sizeof(VulkanGraphicsPipeline_t));
    if (pipeln == NULL)
    { err = -1; goto cleanup; }
    memzero(*pipeln);
    /* setup shader-modules */
    if ((vertexMod = _loadShaderModule(GROUBIKS_DEFAULT_VERTEX_SHADER_PATH, device)) == VK_NULL_HANDLE ||
        (fragmentMod = _loadShaderModule(GROUBIKS_DEFAULT_FRAGMENT_SHADER_PATH, device)) == VK_NULL_HANDLE)
    { err = -1; goto cleanup; }
    log_info("successfully loaded shaders.");
    
    err = _setupRenderPass(pipeln, device, swapchain);
    if (err != 0)
    { goto cleanup; }

    /* setup shaderstages */
    memzero(vertShaderStageCreateInfo);
    memzero(fragShaderStageCreateInfo);
    vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vertShaderStageCreateInfo.module = vertexMod;
    fragShaderStageCreateInfo.module = fragmentMod;
    vertShaderStageCreateInfo.pName = "main";
    fragShaderStageCreateInfo.pName = "main";
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

    /* setup dynamic states */
    VkDynamicState dynamicStates[] = { 
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    memzero(dynamicStateCreateInfo);
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = sizeof(dynamicStates)/sizeof(VkDynamicState);
    dynamicStateCreateInfo.pDynamicStates = &dynamicStates[0];

    /* set viewport and scissors */
    memzero(viewport);
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapchain->m_swapchain_extent.width;
    viewport.height = (float) swapchain->m_swapchain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    memzero(scissors);
    scissors.offset = (VkOffset2D){0, 0};
    scissors.extent = swapchain->m_swapchain_extent;

    memzero(viewportStateCreateInfo);
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissors;

    /* setup vertex-input state */
    memzero(vertexInputCreateInfo);
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;

    /* setup inputassembly */
    memzero(inputAssemblyCreateInfo);
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    /* setup rasterizer */
    memzero(rasterizerCreateInfo);
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1.0f;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

    /* setup multisampling */
    memzero(multisamplingCreateInfo);
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    /* set color-blending */
    memzero(colorBlendAttachmentState);
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_FALSE;

    memzero(colorBlendingCreateInfo);
    colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendingCreateInfo.attachmentCount = 1;
    colorBlendingCreateInfo.pAttachments = &colorBlendAttachmentState;

    /* setup pipeline-layout */
    memzero(layoutCreateInfo);
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    vkerr = vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, &pipeln->m_layout);
    if (vkerr != VK_SUCCESS)
    { err = -1; goto cleanup; }

    /* finally create the pipeline */
    memzero(pipelineCreateInfo);
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = &shaderStageCreateInfos[0];
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    pipelineCreateInfo.pDepthStencilState = NULL;
    pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

    pipelineCreateInfo.layout = pipeln->m_layout;
    pipelineCreateInfo.renderPass = pipeln->m_renderpass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    vkerr = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeln->m_pipeline);
    if (vkerr != VK_SUCCESS)
    { err = -1; goto cleanup; }

cleanup:
    if (vertexMod != VK_NULL_HANDLE)
    { vkDestroyShaderModule(device, vertexMod, NULL); }
    if (fragmentMod != VK_NULL_HANDLE)
    { vkDestroyShaderModule(device, fragmentMod, NULL); }
    if (err != 0)
    { goto error; }

    log_info("successfully created graphics-pipeline.");
    return pipeln;
error:
    log_error("failed to create graphics-pipeline.");
    if (pipeln->m_layout != VK_NULL_HANDLE)
    { vkDestroyPipelineLayout(device, pipeln->m_layout, NULL); }
    if (pipeln->m_renderpass != VK_NULL_HANDLE)
    { vkDestroyRenderPass(device, pipeln->m_renderpass, NULL); }
    free(pipeln);
    return NULL;
}

VkShaderModule _loadShaderModule(const char* codePath, VkDevice device) {
    result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    VkShaderModule res = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo createInfo;
    size_t codeSize;
    char* codeData;
    err = readFile(codePath, &codeData, &codeSize);
    if (err != 0)
    { goto error; }

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = (uint32_t*)codeData; // watch out for alignment here

    vkerr = vkCreateShaderModule(device, &createInfo, NULL, &res);
    if (vkerr != VK_SUCCESS)
    { goto error; }

    logf_info("successfully setup shader-module from %s", codePath);
    free(codeData);
    return res;
error:
    logf_error("failed to load shader-module from %s.", codePath);
    free(codeData);
    return VK_NULL_HANDLE;
}

result_t _setupRenderPass(VulkanGraphicsPipeline pipeln, VkDevice device, VulkanSwapChain swapchain) {
    VkResult vkerr = VK_SUCCESS;
    VkAttachmentDescription colorAttachement;
    VkAttachmentReference colorAttachementRef;
    VkSubpassDescription subpassDescr;
    VkRenderPassCreateInfo renderpassCreateInfo;
    VkSubpassDependency subpassDep;

    /* fill in a bunch of structs */
    memzero(colorAttachement);
    colorAttachement.format = swapchain->m_swapchain_format.format;
    colorAttachement.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachement.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachement.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachement.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachement.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachement.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    memzero(colorAttachementRef);
    colorAttachementRef.attachment = 0;
    colorAttachementRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    memzero(subpassDescr);
    subpassDescr.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescr.colorAttachmentCount = 1;
    subpassDescr.pColorAttachments = &colorAttachementRef;

    memzero(subpassDep);
    subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDep.dstSubpass = 0;
    subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDep.srcAccessMask = 0;
    subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    memzero(renderpassCreateInfo);
    renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassCreateInfo.attachmentCount = 1;
    renderpassCreateInfo.pAttachments = &colorAttachement;
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = &subpassDescr;
    renderpassCreateInfo.dependencyCount = 1;
    renderpassCreateInfo.pDependencies = &subpassDep;
    /* create the renderpass */
    vkerr = vkCreateRenderPass(device, &renderpassCreateInfo, NULL, &pipeln->m_renderpass);
    if (vkerr != VK_SUCCESS)
    { log_error("failed to create renderpass."); return -1; }
    log_info("successfully created renderpass.");
    return 0;
}

void DestroyVulkanGraphicsPipeline(VulkanGraphicsPipeline pipeln, VkDevice device) {
    vkDestroyRenderPass(device, pipeln->m_renderpass, NULL);
    vkDestroyPipeline(device, pipeln->m_pipeline, NULL);
    vkDestroyPipelineLayout(device, pipeln->m_layout, NULL);
}   