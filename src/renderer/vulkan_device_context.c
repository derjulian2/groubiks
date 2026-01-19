
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
    check(err == GROUBIKS_SUCCESS);
    err = vk_device_context_setup_logical_device(pDeviceContext, pExtras);
    check(err == GROUBIKS_SUCCESS);
    err = vk_device_context_retrieve_queues(pDeviceContext);
    check(err == GROUBIKS_SUCCESS);

    log_info("setup vulkan-device-context.");
    return GROUBIKS_SUCCESS;
    except(err,
        free_vk_device_context(pDeviceContext);
        log_error("failed to setup device-context.");
    );
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
    check(queueFamilyCount != 0, err = GROUBIKS_VULKAN_ERROR);
    dynarray_reserve(VkQFamilyProps, 
        &queueFamilyProps,
        queueFamilyCount, 
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
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
        check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

        if (presentSupport) { 
            pDeviceContext->m_qfis.m_present_family = make_optional(u32, idx); 
        }
    }
    log_info("retrieved all required queue-family-indices.");
    cleanup (
        free_dynarray(VkQFamilyProps, &queueFamilyProps);
    );
    return err;
    except (
        log_error("failed to retrieve queue-family-indices.")
    );
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
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    /* setup createinfos */
    queueCreateInfos = make_dynarray(VkDeviceQCreateInfo, 
        NULL, 
        uniqueQueueIndices.size, 
        &dynarrayErr
    );
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);

    dynarray_for_each(u32, &uniqueQueueIndices, qIdx) {
        vk_fill_struct_devicequeue_createinfo(
            dynarray_at(&queueCreateInfos, dynarray_index(&uniqueQueueIndices, qIdx)), 
            *qIdx, 
            &queuePriority
        );
    }

    /* check device for correct extra-support and setup logical-device */
    err = vk_extras_match_device(pExtras, pDeviceContext->m_physical_device);
    check(err == GROUBIKS_SUCCESS);

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
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);


    cleanup (
        free_dynarray(u32, &uniqueQueueIndices);
        free_dynarray(VkDeviceQCreateInfo, &queueCreateInfos);
    );

    log_info("setup logical-device.");
    return err;
    except (
        log_error("failed to setup logical-device.");
    );
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