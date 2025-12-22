
#include <groubiks/renderer/vulkan_device_context.h>

bool vk_qfis_complete(const struct vk_queue_family_indices* qfis) {
    return qfis->m_graphics_family.has_value && qfis->m_present_family.has_value;
}

groubiks_result_t make_vk_device_ctx(struct vk_device_context* pDeviceContext, 
    VkPhysicalDevice physDevice,
    VkSurfaceKHR surface, 
    struct vk_extras* pExtras) {
    assert(pDeviceContext != NULL && physDevice != VK_NULL_HANDLE && surface != VK_NULL_HANDLE && pExtras != NULL);
    groubiks_result_t err = GROUBIKS_SUCCESS;

    memzero(*pDeviceContext);
    pDeviceContext->m_physical_device = physDevice;
    if (vk_device_ctx_find_qfis(pDeviceContext, surface) != GROUBIKS_SUCCESS ||
        vk_device_ctx_setup_logical_device(pDeviceContext, pExtras) != GROUBIKS_SUCCESS ||
        vk_device_ctx_get_queues(pDeviceContext) != GROUBIKS_SUCCESS)
    { goto error; }
    log_info("successfully setup vulkan-device-context.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to setup vulkan-device-context.");
    return GROUBIKS_ERROR;
}

void free_vk_device_ctx(struct vk_device_context* pDeviceContext) {
    assert(pDeviceContext != NULL);
    vkDestroyDevice(pDeviceContext->m_logical_device, NULL);
}

/* only required locally */
declare_vector(VkQueueFamilyProperties);
define_vector(VkQueueFamilyProperties);

groubiks_result_t vk_device_ctx_find_qfis(struct vk_device_context* pDeviceContext, VkSurfaceKHR surface) {
    groubiks_result_t err = GROUBIKS_SUCCESS; vector_result_t vectorErr = VECTOR_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    vector_t(VkQueueFamilyProperties) queueFamilyProps = null_vector(VkQueueFamilyProperties);
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(pDeviceContext->m_physical_device, &queueFamilyCount, NULL);
    if (queueFamilyCount == 0) { err = GROUBIKS_ERROR; goto cleanup; }
    vector_resize(VkQueueFamilyProperties, &queueFamilyProps, queueFamilyCount, &vectorErr);
    if (vectorErr != VECTOR_SUCCESS) { goto cleanup; }
    vkGetPhysicalDeviceQueueFamilyProperties(pDeviceContext->m_physical_device, &queueFamilyCount, queueFamilyProps.data);

    vector_for_each(VkQueueFamilyProperties, &queueFamilyProps, props) {
        VkBool32 presentSupport = false;
        uint32_t i = vector_make_index(&queueFamilyProps, props);
        if (props->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            pDeviceContext->m_qfis.m_graphics_family = make_optional(uint32_t, i);
        }
        vkErr = vkGetPhysicalDeviceSurfaceSupportKHR(pDeviceContext->m_physical_device, i, surface, &presentSupport);
        if (vkErr != VK_SUCCESS) { goto cleanup; }
        if (presentSupport) { pDeviceContext->m_qfis.m_present_family = make_optional(uint32_t, i); }
    }
    if (!vk_qfis_complete(&pDeviceContext->m_qfis)) { goto error; }
 
cleanup:
    free_vector(&queueFamilyProps);
    if (vectorErr != VECTOR_SUCCESS || vkErr != VK_SUCCESS || err != GROUBIKS_SUCCESS)
    { goto error; }

    log_info("successfully retrieved all required queue-family-indices.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to find required queue-family-indices");
    return GROUBIKS_ERROR;
}

/* only required locally */
declare_vector(VkDeviceQueueCreateInfo);
define_vector(VkDeviceQueueCreateInfo);

groubiks_result_t vk_device_ctx_setup_logical_device(struct vk_device_context* pDeviceContext, struct vk_extras* pExtras) {
    assert(pDeviceContext != NULL && pExtras != NULL && 
        pDeviceContext->m_qfis.m_graphics_family.has_value &&
        pDeviceContext->m_qfis.m_present_family.has_value);
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;
    VkDeviceCreateInfo deviceCreateInfo; VkPhysicalDeviceFeatures deviceFeatures;
    vector_t(VkDeviceQueueCreateInfo) queueCreateInfos = null_vector(VkDeviceQueueCreateInfo);
    vector_t(uint32_t) queueIndices = null_vector(uint32_t), uniqueQueueIndices = null_vector(uint32_t);
    uint32_t queueIndicesData[] = { 
        pDeviceContext->m_qfis.m_graphics_family.value, 
        pDeviceContext->m_qfis.m_present_family.value 
    };
    float queuePriority = 1.0f;

    /* find unique queuefamily-indices and setup createinfos */
    queueIndices = assign_vector(uint32_t, &queueIndicesData[0], 2);
    uniqueQueueIndices = vector_uniques(uint32_t, &queueIndices);
    vector_for_each(uint32_t, &uniqueQueueIndices, queueIndex) {
        VkDeviceQueueCreateInfo queueCreateInfo;
        memzero(queueCreateInfo);
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = *queueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        vector_push_back(VkDeviceQueueCreateInfo, &queueCreateInfos, queueCreateInfo, &err);
        if (err != VECTOR_SUCCESS) { goto error; }
    }

    /* check device for correct extra-support and setup logical-device */
    err = vk_extras_match_device(pExtras, pDeviceContext->m_physical_device);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }
    memzero(deviceCreateInfo);
    memzero(deviceFeatures);
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data;
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledLayerCount = pExtras->m_validationlayers.size;
    deviceCreateInfo.ppEnabledLayerNames = (const char**)pExtras->m_validationlayers.data;
    deviceCreateInfo.enabledExtensionCount = pExtras->m_extensions.size;
    deviceCreateInfo.ppEnabledExtensionNames = (const char**)pExtras->m_extensions.data;

    vkErr = vkCreateDevice(pDeviceContext->m_physical_device, &deviceCreateInfo, NULL, &pDeviceContext->m_logical_device);
    if (vkErr != VK_SUCCESS) { goto cleanup; }

cleanup:
    free_vector(&uniqueQueueIndices);
    free_vector(&queueCreateInfos);
    if (err != GROUBIKS_SUCCESS) { goto error; }

    log_info("successfully setup logical-device.");
    return GROUBIKS_SUCCESS;
error:
    log_error("failed to setup logical-device.");
    return GROUBIKS_ERROR;
}

groubiks_result_t vk_device_ctx_get_queues(struct vk_device_context* pDeviceContext) {
    assert(pDeviceContext != NULL && 
        pDeviceContext->m_qfis.m_graphics_family.has_value &&
        pDeviceContext->m_qfis.m_present_family.has_value);
    vkGetDeviceQueue(pDeviceContext->m_logical_device, 
        pDeviceContext->m_qfis.m_graphics_family.value, 
        0, 
        &pDeviceContext->m_queues.m_graphics_queue);
    vkGetDeviceQueue(pDeviceContext->m_logical_device,
        pDeviceContext->m_qfis.m_present_family.value,
        0,
        &pDeviceContext->m_queues.m_present_queue);
    log_info("successfully retrieved queue-handles.");
    return GROUBIKS_SUCCESS;
}