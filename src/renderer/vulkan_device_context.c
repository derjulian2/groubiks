
#include <groubiks/renderer/vulkan_device_context.h>

define_dynarray(VkQueueFamilyProperties, VkQFamilyProps, 
    (comp, NULL)
);


define_dynarray(VkDeviceQueueCreateInfo, VkDeviceQCreateInfo, 
    (comp, NULL)
);


bool 
vk_qfis_complete(const struct vk_queue_family_indices* qfis) {
    return qfis->m_graphics_family.has_value && qfis->m_present_family.has_value;
}


groubiks_result_t 
vk_device_context_create(struct vk_device_context* pDeviceContext, 
    VkPhysicalDevice physDevice,
    VkSurfaceKHR surface, 
    struct vk_extras* pExtras) 
{
    groubiks_result_t err = GROUBIKS_SUCCESS;

    *pDeviceContext = vk_device_context_null;
    pDeviceContext->m_physical_device = physDevice;
    err = vk_device_context_get_qfis(pDeviceContext, surface);
    if (err != GROUBIKS_SUCCESS) {
        log_error("failed to retrieve vulkan-queuefamilyindices.");
        goto error;
    }

    err = vk_device_context_setup_logical_device(pDeviceContext, pExtras);
    if (err != GROUBIKS_SUCCESS) {
        log_error("failed to setup logical-device.");
        goto error;
    }

    err = vk_device_context_retrieve_queues(pDeviceContext);
    if (err != GROUBIKS_SUCCESS) {
        log_error("failed to retrieve queue-handles.");
        goto error;
    }

    log_info("setup vulkan-device-context.");
    return GROUBIKS_SUCCESS;
error:
    free_vk_device_context(pDeviceContext);
    log_error("failed to setup device-context.");
    return err;
}


void
free_vk_device_context(struct vk_device_context* pDeviceContext) {
    if (pDeviceContext->m_logical_device != VK_NULL_HANDLE) {
        vkDestroyDevice(pDeviceContext->m_logical_device, NULL);
    }
}


groubiks_result_t 
vk_device_context_get_qfis(struct vk_device_context* pDeviceContext, 
    VkSurfaceKHR surface) 
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;
    
    struct dynarray(VkQFamilyProps) queueFamilyProps = null_dynarray(VkQFamilyProps);
    u32 queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(pDeviceContext->m_physical_device,
        &queueFamilyCount, 
        NULL
    );
    if (queueFamilyCount == 0) {
        err = GROUBIKS_VULKAN_ERROR;
        log_error("failed retrieve vulkan-queuefamilyproperties.");
        goto cleanup;
    }

    dynarray_reserve(VkQFamilyProps, 
        &queueFamilyProps,
        queueFamilyCount, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) {
        err = GROUBIKS_BAD_ALLOC;
        log_error("failed to reserve space for vulkan-queuefamilyproperties.");
        goto cleanup;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(pDeviceContext->m_physical_device, 
        &queueFamilyCount, 
        queueFamilyProps.data
    );
    dynarray_for_each(VkQFamilyProps, &queueFamilyProps, props) {
        VkBool32 presentSupport = false;
        u32 idx = dynarray_index(&queueFamilyProps, props);
        
        if (props->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            pDeviceContext->m_qfis.m_graphics_family = make_optional(u32, idx);
        }
        
        vkErr = vkGetPhysicalDeviceSurfaceSupportKHR(pDeviceContext->m_physical_device, 
            idx, 
            surface, 
            &presentSupport
        );
        if (vkErr != VK_SUCCESS) {
            err = GROUBIKS_VULKAN_ERROR; 
            log_error("failed to query for device's surface-support.");
            goto cleanup;
        }

        if (presentSupport) { 
            pDeviceContext->m_qfis.m_present_family = make_optional(u32, idx); 
        }
    }
    log_info("retrieved all required queue-family-indices.");
cleanup:
    free_dynarray(VkQFamilyProps, &queueFamilyProps);    
    return err;
}


groubiks_result_t 
vk_device_context_setup_logical_device(struct vk_device_context* pDeviceContext, 
    struct vk_extras* pExtras) 
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;

    VkDeviceCreateInfo deviceCreateInfo; 
    VkPhysicalDeviceFeatures deviceFeatures;
    dynarray_t(VkDeviceQCreateInfo) queueCreateInfos = null_dynarray(VkDeviceQCreateInfo);
    dynarray_t(u32) uniqueQueueIndices = null_dynarray(u32);
    float queuePriority = 1.0f;
    u32 qfis[] = { 
        pDeviceContext->m_qfis.m_graphics_family.value,
        pDeviceContext->m_qfis.m_present_family.value 
    };

    /* find unique queuefamily-indices */
    uniqueQueueIndices = dynarray_uniques_from_range(u32, 
        &qfis[0], 
        sizeof(qfis)/sizeof(u32), 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) {
        err = GROUBIKS_BAD_ALLOC;
        log_error("failed to find unique queuefamilyindices.");
        goto cleanup;
    }

    /* setup createinfos */
    queueCreateInfos = make_dynarray(VkDeviceQCreateInfo, 
        NULL, 
        uniqueQueueIndices.size, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) {
        err = GROUBIKS_BAD_ALLOC;
        log_error("failed to find unique queuefamilyindices.");
        goto cleanup;
    }

    dynarray_for_each(u32, &uniqueQueueIndices, qIdx) {
        vk_fill_struct_devicequeue_createinfo(
            dynarray_at(&queueCreateInfos, dynarray_index(&uniqueQueueIndices, qIdx)), 
            *qIdx, 
            &queuePriority
        );
    }

    /* check device for correct extra-support and setup logical-device */
    err = vk_extras_match_device(pExtras, pDeviceContext->m_physical_device);
    if (err != GROUBIKS_SUCCESS) { 
        log_error("failed to match vulkan-extras against device.");
        goto cleanup; 
    }

    vk_fill_struct_device_createinfo(&deviceCreateInfo, 
        &deviceFeatures, 
        queueCreateInfos.data, 
        queueCreateInfos.size, 
        (const char* const*)pExtras->m_validationlayers.data, 
        pExtras->m_validationlayers.size, 
        (const char* const*)pExtras->m_extensions.data, 
        pExtras->m_extensions.size
    );
    vkErr = vkCreateDevice(pDeviceContext->m_physical_device, 
        &deviceCreateInfo, 
        NULL, 
        &pDeviceContext->m_logical_device
    );
    /* no need to check here, cleanup is directly after. */

cleanup:
    free_dynarray(u32, &uniqueQueueIndices);
    free_dynarray(VkDeviceQCreateInfo, &queueCreateInfos);
    if (err         != GROUBIKS_SUCCESS ||
        dynarrayErr != DYNARRAY_SUCCESS ||
        vkErr       != VK_SUCCESS) 
    { goto error; }

    log_info("setup logical-device.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to setup logical-device.");
    return err;
}

groubiks_result_t 
vk_device_context_retrieve_queues(struct vk_device_context* pDeviceContext) {
    vkGetDeviceQueue(pDeviceContext->m_logical_device, 
        pDeviceContext->m_qfis.m_graphics_family.value, 
        0, 
        &pDeviceContext->m_queues.m_graphics_queue
    );
    vkGetDeviceQueue(pDeviceContext->m_logical_device,
        pDeviceContext->m_qfis.m_present_family.value,
        0,
        &pDeviceContext->m_queues.m_present_queue
    );
    log_info("retrieved queue-handles.");
    return GROUBIKS_SUCCESS;
}