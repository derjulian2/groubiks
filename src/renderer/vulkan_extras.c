
#include <groubiks/renderer/vulkan_extras.h>

/* dynarrays for non-comparable types. */
define_dynarray(VkLayerProperties, VkLayerProps, 
    (comp, NULL)
);
define_dynarray(VkExtensionProperties, VkExtProps, 
    (comp, NULL)
);

groubiks_result_t 
vk_extras_get_glfw(struct vk_extras* pExt) 
{
    groubiks_result_t err         = GROUBIKS_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS; 

    const char** ppGlfwExtensions = NULL; 
    u32 glfwExtensionCount = 0;
    size_t oldSize = pExt->m_extensions.size;

    ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (ppGlfwExtensions == NULL) { 
        err = GROUBIKS_GLFW_ERROR;
        goto error; 
    }

    logf_info("found %d glfw-extensions:", glfwExtensionCount);
    for (u32 i = 0; i < glfwExtensionCount; ++i) {
        logf_info("%d. %s", i, ppGlfwExtensions[i]);
    }

    dynarray_insert_range(str, 
        &pExt->m_extensions, 
        pExt->m_extensions.size, 
        ppGlfwExtensions, 
        glfwExtensionCount, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { 
        err = GROUBIKS_BAD_ALLOC;
        goto error; 
    }

    log_info("retrieved glfw-extensions");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to retrieve glfw-extensions.");
    return err;
}

groubiks_result_t 
vk_extras_match_instance(const struct vk_extras* pExt) 
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;

    dynarray_t(VkLayerProps) layerProps   = null_dynarray(VkLayerProps); 
    dynarray_t(VkExtProps) extensionProps = null_dynarray(VkExtProps);
    u32 layerCount     = 0;
    u32 extensionCount = 0;

    logf_info("requested %d instance-validationlayers:", pExt->m_validationlayers.size);
    dynarray_for_each(str, &pExt->m_validationlayers, layer) {
        logf_info("%d. %s", dynarray_index(&pExt->m_validationlayers, layer), layer);
    }
    logf_info("requested %d instance-extensions:", pExt->m_extensions.size);
    dynarray_for_each(str, &pExt->m_extensions, extension) {
        logf_info("%d. %s", dynarray_index(&pExt->m_extensions, extension), extension);
    }

    vkErr = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    dynarray_resize(VkLayerProps, &layerProps, layerCount, &dynarrayErr);
    if (dynarrayErr != DYNARRAY_SUCCESS) { 
        err = GROUBIKS_BAD_ALLOC; 
        goto cleanup; 
    }
    vkErr = vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    dynarray_resize(VkExtProps, &extensionProps, extensionCount, &dynarrayErr);
    if (dynarrayErr != DYNARRAY_SUCCESS) { 
        err = GROUBIKS_BAD_ALLOC; 
        goto cleanup; 
    }
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProps.data);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    
    err = vk_extras_check_layers(&pExt->m_validationlayers, &layerProps);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }
    err = vk_extras_check_extensions(&pExt->m_extensions, &extensionProps);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

cleanup:
    free_dynarray(VkLayerProps, &layerProps);
    free_dynarray(VkExtProps, &extensionProps);
    if (dynarrayErr != DYNARRAY_SUCCESS || 
        vkErr       != VK_SUCCESS || 
        err         != GROUBIKS_SUCCESS)
    { goto error; }

    log_info("matched vulkan-extras against instance.");
    log_info("all requested validationlayers and extensions were found.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to match vulkan-extras against instance.");
    log_error("some requested validationlayers or extensions are unsupported.");
    return err;
}

groubiks_result_t 
vk_extras_match_device(const struct vk_extras* pExt, 
    VkPhysicalDevice device) 
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;
    
    dynarray_t(VkLayerProps) layerProps   = null_dynarray(VkLayerProps); 
    dynarray_t(VkExtProps) extensionProps = null_dynarray(VkExtProps);
    u32 layerCount     = 0;
    u32 extensionCount = 0;

    logf_info("requested %d device-validationlayers:", pExt->m_validationlayers.size);
    dynarray_for_each(str, &pExt->m_validationlayers, layer) {
        logf_info("%d. %s", dynarray_index(&pExt->m_validationlayers, layer), layer);
    }
    logf_info("requested %d device-extensions:", pExt->m_extensions.size);
    dynarray_for_each(str, &pExt->m_extensions, extension) {
        logf_info("%d. %s", dynarray_index(&pExt->m_extensions, extension), extension);
    }

    vkErr = vkEnumerateDeviceLayerProperties(device, &layerCount, NULL);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    dynarray_resize(VkLayerProps, &layerProps, layerCount, &dynarrayErr);
    if (dynarrayErr != DYNARRAY_SUCCESS) { 
        err = GROUBIKS_BAD_ALLOC; 
        goto cleanup; 
    }
    vkErr = vkEnumerateDeviceLayerProperties(device, &layerCount, layerProps.data);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    dynarray_resize(VkExtProps, &extensionProps, extensionCount, &dynarrayErr);
    if (dynarrayErr != DYNARRAY_SUCCESS) { 
        err = GROUBIKS_BAD_ALLOC; 
        goto cleanup; 
    }
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, extensionProps.data);
    if (vkErr != VK_SUCCESS) { 
        err = GROUBIKS_VULKAN_ERROR;
        goto cleanup; 
    }
    
    err = vk_extras_check_layers(&pExt->m_validationlayers, &layerProps);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }
    err = vk_extras_check_extensions(&pExt->m_extensions, &extensionProps);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

cleanup:
    free_dynarray(VkLayerProps, &layerProps);
    free_dynarray(VkExtProps, &extensionProps);
    if (dynarrayErr != DYNARRAY_SUCCESS || 
        vkErr       != VK_SUCCESS || 
        err         != GROUBIKS_SUCCESS)
    { goto error; }

    log_info("matched vulkan-extras against device.");
    log_info("all requested validationlayers and extensions were found.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to match vulkan-extras against device:");
    log_error("some requested validationlayers or extensions are unsupported.");
    return err;
}

groubiks_result_t 
vk_extras_check_layers(const struct dynarray(str)* pRequestedLayers, 
    const struct dynarray(VkLayerProps)* pLayers) 
{
    dynarray_for_each(str, pRequestedLayers, pRequestedLayer) {
        bool found = false;
        dynarray_for_each(VkLayerProps, pLayers, pLayer) {
            if (strcmp(*pRequestedLayer, pLayer->layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            logf_error("requested layer not found: %s", *pRequestedLayer);
            return GROUBIKS_VULKAN_ERROR;
        }
    }
    return GROUBIKS_SUCCESS;
}

groubiks_result_t 
vk_extras_check_extensions(const struct dynarray(str)* pRequestedExtensions, 
    const struct dynarray(VkExtProps)* pExtensions) 
{
    dynarray_for_each(str, pRequestedExtensions, pRequestedExtension) {
        bool found = false;
        dynarray_for_each(VkExtProps, pExtensions, pExtension) {
            if (strcmp(*pRequestedExtension, pExtension->extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            logf_error("requested extension not found: %s", *pRequestedExtension);
            return GROUBIKS_VULKAN_ERROR;
        }
    }
    return GROUBIKS_SUCCESS;
}