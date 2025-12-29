
#include <groubiks/renderer/vulkan_extras.h>

/* non-comparable types. */
define_vector(VkLayerProperties, VkLayerProps, NULL);
define_vector(VkExtensionProperties, VkExtProps, NULL);

groubiks_result_t vk_extras_get_glfw(struct vk_extras* pExt) {
    assert(pExt != NULL);
    vector_result_t vectorErr = VECTOR_SUCCESS; 
    const char** ppGlfwExtensions; uint32_t glfwExtensionCount = 0;
    size_t oldSize = pExt->m_extensions.size;

    ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (ppGlfwExtensions == NULL) { goto error; }

    logf_info("found %d glfw-extensions:", glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        logf_info("%d. %s", i, ppGlfwExtensions[i]);
    }

    vector_insert_range(str, &pExt->m_extensions, pExt->m_extensions.size, 
        ppGlfwExtensions, glfwExtensionCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto error; }

    log_info("successfully retrieved glfw-extensions");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to retrieve glfw-extensions.");
    return GROUBIKS_ERROR;
}

groubiks_result_t vk_extras_match_instance(const struct vk_extras* pExt) {
    assert(pExt != NULL);
    groubiks_result_t err = GROUBIKS_SUCCESS; vector_result_t vectorErr = VECTOR_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    vector_t(VkLayerProps) layerProps = null_vector(VkLayerProps); 
    vector_t(VkExtProps) extensionProps = null_vector(VkExtProps);
    uint32_t layerCount = 0; uint32_t extensionCount = 0;

    logf_info("requested %d instance-validationlayers:", pExt->m_validationlayers.size);
    vector_for_each(str, &pExt->m_validationlayers, layer) {
        logf_info("%d. %s", vector_index(&pExt->m_validationlayers, layer), layer);
    }
    logf_info("requested %d instance-extensions:", pExt->m_extensions.size);
    vector_for_each(str, &pExt->m_extensions, extension) {
        logf_info("%d. %s", vector_index(&pExt->m_extensions, extension), extension);
    }

    vkErr = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    vector_resize(VkLayerProps, &layerProps, layerCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto cleanup; }
    vkErr = vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    vector_resize(VkExtProps, &extensionProps, extensionCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto cleanup; }
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProps.data);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    
    if (vk_extras_check_layers(&pExt->m_validationlayers, &layerProps) != GROUBIKS_SUCCESS ||
        vk_extras_check_extensions(&pExt->m_extensions, &extensionProps) != GROUBIKS_SUCCESS)
    { err = GROUBIKS_ERROR; goto cleanup; }

cleanup:
    free_vector(VkLayerProps, &layerProps);
    free_vector(VkExtProps, &extensionProps);
    if (vectorErr != VECTOR_SUCCESS || vkErr != VK_SUCCESS || err != GROUBIKS_SUCCESS)
    { goto error; }

    log_info("successfully matched vulkan-extras against instance.");
    log_info("all requested validationlayers and extensions were found.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to match vulkan-extras against instance:");
    log_error("some requested validationlayers or extensions are unsupported.");
    return GROUBIKS_ERROR;
}

groubiks_result_t vk_extras_match_device(const struct vk_extras* pExt, VkPhysicalDevice device) {
    assert(pExt != NULL && device != VK_NULL_HANDLE);
    groubiks_result_t err = GROUBIKS_SUCCESS; vector_result_t vectorErr = VECTOR_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    vector_t(VkLayerProps) layerProps = null_vector(VkLayerProps); 
    vector_t(VkExtProps) extensionProps = null_vector(VkExtProps);
    uint32_t layerCount = 0; uint32_t extensionCount = 0;

    logf_info("requested %d device-validationlayers:", pExt->m_validationlayers.size);
    vector_for_each(str, &pExt->m_validationlayers, layer) {
        logf_info("%d. %s", vector_index(&pExt->m_validationlayers, layer), layer);
    }
    logf_info("requested %d device-extensions:", pExt->m_extensions.size);
    vector_for_each(str, &pExt->m_extensions, extension) {
        logf_info("%d. %s", vector_index(&pExt->m_extensions, extension), extension);
    }

    vkErr = vkEnumerateDeviceLayerProperties(device, &layerCount, NULL);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    vector_resize(VkLayerProps, &layerProps, layerCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto cleanup; }
    vkErr = vkEnumerateDeviceLayerProperties(device, &layerCount, layerProps.data);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    vector_resize(VkExtProps, &extensionProps, extensionCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto cleanup; }
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, extensionProps.data);
    if (vkErr != VK_SUCCESS) { goto cleanup; }
    
    if (vk_extras_check_layers(&pExt->m_validationlayers, &layerProps) != GROUBIKS_SUCCESS ||
        vk_extras_check_extensions(&pExt->m_extensions, &extensionProps) != GROUBIKS_SUCCESS)
    { err = GROUBIKS_ERROR; goto cleanup; }

cleanup:
    free_vector(VkLayerProps, &layerProps);
    free_vector(VkExtProps, &extensionProps);
    if (vectorErr != VECTOR_SUCCESS || vkErr != VK_SUCCESS || err != GROUBIKS_SUCCESS)
    { goto error; }

    log_info("successfully matched vulkan-extras against device.");
    log_info("all requested validationlayers and extensions were found.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to match vulkan-extras against device:");
    log_error("some requested validationlayers or extensions are unsupported.");
    return GROUBIKS_ERROR;
}

groubiks_result_t vk_extras_check_layers(const vector_t(str)* pRequestedLayers, 
    const vector_t(VkLayerProps)* pLayers) {
    assert(pRequestedLayers != NULL && pLayers != NULL);
    vector_for_each(str, pRequestedLayers, pRequestedLayer) {
        bool found = false;
        vector_for_each(VkLayerProps, pLayers, pLayer) {
            if (strcmp(*pRequestedLayer, pLayer->layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            logf_error("requested layer not found: %s", *pRequestedLayer);
            return GROUBIKS_ERROR;
        }
    }
    return GROUBIKS_SUCCESS;
}

groubiks_result_t vk_extras_check_extensions(const vector_t(str)* pRequestedExtensions, 
    const vector_t(VkExtProps)* pExtensions) {
    assert(pRequestedExtensions != NULL && pExtensions != NULL);
    vector_for_each(str, pRequestedExtensions, pRequestedExtension) {
        bool found = false;
        vector_for_each(VkExtProps, pExtensions, pExtension) {
            if (strcmp(*pRequestedExtension, pExtension->extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            logf_error("requested extension not found: %s", *pRequestedExtension);
            return GROUBIKS_ERROR;
        }
    }
    return GROUBIKS_SUCCESS;
}