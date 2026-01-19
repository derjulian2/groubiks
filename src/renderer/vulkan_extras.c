
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
    check(ppGlfwExtensions, err = GROUBIKS_GLFW_ERROR);

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
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    log_info("retrieved glfw-extensions");
    return GROUBIKS_SUCCESS;
    except(err, log_error("failed to retrieve glfw-extensions."));
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
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkLayerProps, &layerProps, layerCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkExtProps, &extensionProps, extensionCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProps.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    err = vk_extras_check_layers(&pExt->m_validationlayers, &layerProps);
    check(err == GROUBIKS_SUCCESS);
    err = vk_extras_check_extensions(&pExt->m_extensions, &extensionProps);
    check(err == GROUBIKS_SUCCESS);

    cleanup (
        free_dynarray(VkLayerProps, &layerProps);
        free_dynarray(VkExtProps, &extensionProps);
    );
    log_info("matched vulkan-extras against instance.");
    log_info("all requested validationlayers and extensions were found.");
    return err;
    except ( 
        log_error("failed to match vulkan-extras against instance.");
        log_error("some requested validationlayers or extensions are unsupported.");
    );
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
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkLayerProps, &layerProps, layerCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumerateDeviceLayerProperties(device, &layerCount, layerProps.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkExtProps, &extensionProps, extensionCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, extensionProps.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    
    err = vk_extras_check_layers(&pExt->m_validationlayers, &layerProps);
    check(err == GROUBIKS_SUCCESS);
    err = vk_extras_check_extensions(&pExt->m_extensions, &extensionProps);
    check(err == GROUBIKS_SUCCESS);

    cleanup (
        free_dynarray(VkLayerProps, &layerProps);
        free_dynarray(VkExtProps, &extensionProps);
    );
    log_info("matched vulkan-extras against device.");
    log_info("all requested validationlayers and extensions were found.");
    return err;
    except (
        log_error("failed to match vulkan-extras against device:");
        log_error("some requested validationlayers or extensions are unsupported.");
    );
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