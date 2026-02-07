
#include <groubiks/renderer/vulkan_graphics_pipeline.h>

define_dynarray(struct vk_shadermodule, shadermodule,
    (comp, NULL)
);


groubiks_result_t
vk_create_shadermodule_from_spirv(struct vk_shadermodule* pShaderMod,
    const char* const srcPath,
    VkDevice device)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    VkShaderModuleCreateInfo createInfo;
    int ioErr = 0;
    char* data = NULL; size_t size = 0;

    ioErr = read_file(srcPath, &data, &size);
    check(ioErr != 0, err = GROUBIKS_IO_ERROR);
    pShaderMod->m_data = assign_dynarray(u32, (u32*)data, size);

    vk_fill_struct_shadermod_createinfo(&createInfo, pShaderMod->m_data.size, pShaderMod->m_data.data);
    vkErr = vkCreateShaderModule(device, &createInfo, NULL, &pShaderMod->m_module);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_SHADERMOD_SETUP_SUCCESS_STR);
    return err;
    except(err,
        free_vk_shadermodule(pShaderMod, device);
        log_error(VK_SHADERMOD_SETUP_FAIL_STR);
    )
}


groubiks_result_t
vk_create_shadermodule_from_glsl(struct vk_shadermodule* pShaderMod,
    const char* const srcPath,
    VkDevice device,
    shaderc_compiler_t* pCompiler)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    VkShaderModuleCreateInfo createInfo;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    shaderc_compilation_result_t comp_result = NULL;
    int ioErr = 0;
    char* data = NULL; size_t size = 0;

    ioErr = read_file(srcPath, &data, &size);
    check(ioErr != 0, err = GROUBIKS_IO_ERROR);

    comp_result = shaderc_compile_into_spv_assembly(*pCompiler, 
        data, size, 
        pShaderMod->m_shadertype, 
        NULL, "main", NULL
    );
    check(shaderc_result_get_compilation_status(comp_result) == shaderc_compilation_status_success, err = GROUBIKS_SHADER_ERROR);

    pShaderMod->m_data = make_dynarray(u32, 
        (u32*)shaderc_result_get_bytes(comp_result), 
        shaderc_result_get_length(comp_result),
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    vk_fill_struct_shadermod_createinfo(&createInfo, pShaderMod->m_data.size, pShaderMod->m_data.data);
    vkErr = vkCreateShaderModule(device, &createInfo, NULL, &pShaderMod->m_module);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_SHADERMOD_SETUP_SUCCESS_STR);
    cleanup(
        if (comp_result != NULL) {
            shaderc_result_release(comp_result);
        }
        free(data);
    )
    return err;
    except(
        free_vk_shadermodule(pShaderMod, device);
        log_error(VK_SHADERMOD_SETUP_FAIL_STR);
    )
}


void
free_vk_shadermodule(struct vk_shadermodule* pShaderMod,
    VkDevice device)
{
    if (pShaderMod->m_data.data != NULL) {
        free_dynarray(u32, &pShaderMod->m_data);
    }
    if (pShaderMod->m_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, pShaderMod->m_module, NULL);
    }
}


groubiks_result_t
vk_graphics_pipeline_create(struct vk_graphics_pipeline* pPipeline, 
    struct vk_device_context* pDeviceContext,
    struct vk_swapchain* pSwapChain,
    struct dynarray(shadermodule)* pShaderModules) 
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2]; // might switch this to dynarray for more shaders
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
    VkDynamicState dynamicStates[] = { 
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    *pPipeline = vk_graphics_pipeline_null;

    err = vk_graphics_pipeline_setup_renderpass(pPipeline, pSwapChain, pDeviceContext->m_logical_device);
    check(err == GROUBIKS_SUCCESS)

    vk_fill_struct_pipeline_layout_createinfo(&layoutCreateInfo);
    vkErr = vkCreatePipelineLayout(pDeviceContext->m_logical_device, &layoutCreateInfo, NULL, &pPipeline->m_layout);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    /* fill out many structs (i should probably group them together somehow, but it's very late and i wan't to sleep)*/
    dynarray_for_each(shadermodule, pShaderModules, mod) {
        dynarray_index_t idx = dynarray_index(pShaderModules, mod);
        switch (mod->m_shadertype) {
        case (shaderc_vertex_shader):
            vk_fill_struct_shaderstage_createinfo(&shaderStageCreateInfos[idx], 
                VK_SHADER_STAGE_VERTEX_BIT, 
                mod->m_module
            );
            break;
        case (shaderc_fragment_shader):
            vk_fill_struct_shaderstage_createinfo(&shaderStageCreateInfos[idx], 
                VK_SHADER_STAGE_FRAGMENT_BIT, 
                mod->m_module
            );
            break;
        default:
            raise(err = GROUBIKS_SHADER_ERROR)
        }
    }
    
    vk_fill_struct_viewport_and_scissors(&viewport, &scissors, &pSwapChain->m_extent);
    vk_fill_struct_viewportstate_createinfo(&viewportStateCreateInfo, 1, &viewport, 1, &scissors);
    
    vk_fill_struct_vertexinput_createinfo(&vertexInputCreateInfo);
    vk_fill_struct_inputassembly_createinfo(&inputAssemblyCreateInfo);
    vk_fill_struct_rasterizer_createInfo(&rasterizerCreateInfo);

    vk_fill_struct_colors_createinfo(&multisamplingCreateInfo, &colorBlendingCreateInfo, &colorBlendAttachmentState);
    vk_fill_struct_dynamicstate_createinfo(&dynamicStateCreateInfo, sizeof(dynamicStates)/sizeof(VkDynamicState), &dynamicStates[0]);

    vk_fill_struct_pipeline_createinfo(&pipelineCreateInfo, 
        2, &shaderStageCreateInfos[0], 
        &vertexInputCreateInfo,
        &inputAssemblyCreateInfo, 
        &viewportStateCreateInfo, 
        &rasterizerCreateInfo, 
        &multisamplingCreateInfo, 
        &colorBlendingCreateInfo, 
        &dynamicStateCreateInfo, 
        pPipeline->m_layout, 
        pPipeline->m_renderpass
    );

    vkErr = vkCreateGraphicsPipelines(pDeviceContext->m_logical_device,
        VK_NULL_HANDLE,
        1, &pipelineCreateInfo,
        NULL, &pPipeline->m_pipeline
    );
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_PIPELINE_CREATE_SUCCESS_STR);
    return err;
    except(err,
        free_vk_graphics_pipeline(pPipeline, pDeviceContext->m_logical_device);
        log_error(VK_PIPELINE_CREATE_FAIL_STR);
    )
}


void
free_vk_graphics_pipeline(struct vk_graphics_pipeline* pPipeline, 
    VkDevice device)
{
    if (pPipeline->m_renderpass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, pPipeline->m_renderpass, NULL);
    }
    if (pPipeline->m_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pPipeline->m_layout, NULL);
    }
    if (pPipeline->m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, pPipeline->m_pipeline, NULL);
    }
}


groubiks_result_t
vk_graphics_pipeline_setup_renderpass(struct vk_graphics_pipeline* pPipeline,
    struct vk_swapchain* pSwapChain,
    VkDevice device)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS;
    VkAttachmentDescription colorAttachement;
    VkAttachmentReference colorAttachementRef;
    VkSubpassDescription subpassDescr;
    VkRenderPassCreateInfo createInfo;
    VkSubpassDependency subpassDep;

    vk_fill_struct_color_attachment(&colorAttachement, pSwapChain->m_format.format, &colorAttachementRef);
    vk_fill_struct_subpass(&subpassDescr, &subpassDep, &colorAttachementRef);
    vk_fill_struct_renderpass_createinfo(&createInfo, 
        &subpassDescr, 1, 
        &subpassDep, 1, 
        &colorAttachement, 1
    );

    vkErr = vkCreateRenderPass(device, &createInfo, NULL, &pPipeline->m_renderpass);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    log_info(VK_PIPELINE_RENDERPASS_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_PIPELINE_RENDERPASS_SETUP_FAIL_STR);
    )
}