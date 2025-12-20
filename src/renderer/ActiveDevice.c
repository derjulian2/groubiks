
#include <groubiks/renderer/ActiveDevice.h>

define_vector(VkQueue);
define_vector(VkDeviceQueueCreateInfo);

bool VulkanQueueFamilyIndices_Complete(VulkanQueueFamilyIndices_t* idx) {
    return idx->m_graphics_family.has_value && idx->m_present_family.has_value;
}

VulkanActiveDevice CreateVulkanActiveDevice(VkPhysicalDevice phdvc, VulkanExtensions ext, VkSurfaceKHR surface) {
    VulkanActiveDevice advc = malloc(sizeof(VulkanActiveDevice_t));
    if (advc == NULL)
    { return NULL; }
    memzero(*advc);
    advc->m_physical_device = phdvc;
    if (_setupQueueFamilyIndices(advc, surface) ||
        _setupLogicalDevice(advc, ext) ||
        _retrieveQueueHandles(advc))
    { goto error; }
    return advc;
error:
    DestroyVulkanActiveDevice(advc);
    free(advc);
    return NULL;
}

GroubiksResult_t _setupQueueFamilyIndices(VulkanActiveDevice_t* advc, VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    VkQueueFamilyProperties* props = NULL;
    vkGetPhysicalDeviceQueueFamilyProperties(advc->m_physical_device, &queueFamilyCount, NULL);
    if (queueFamilyCount == 0)
    { log_error("could not find any queue-families with suitable device."); return -1; }
    props = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    if (props == NULL)
    { log_error("bad malloc when setting up queuefamilyindices."); return -1; }
    vkGetPhysicalDeviceQueueFamilyProperties(advc->m_physical_device, &queueFamilyCount, props);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            advc->m_family_indices.m_graphics_family = make_optional(uint32_t, i);
        }
        VkBool32 presentSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(advc->m_physical_device, i, surface, &presentSupported);
        if (presentSupported)
        { advc->m_family_indices.m_present_family = make_optional(uint32_t, i); }
        if (VulkanQueueFamilyIndices_Complete(&advc->m_family_indices))
        { break; }
    }
    if (!VulkanQueueFamilyIndices_Complete(&advc->m_family_indices))
    { log_error("suitable device does not support required queue-families."); goto error; }
    log_info("successfully setup queuefamilyindices.");
    return 0;
error:
    free(props);
    return -1;
}

GroubiksResult_t _setupLogicalDevice(VulkanActiveDevice_t* advc, VulkanExtensions_t* ext) {
    VkDeviceCreateInfo deviceCreateInfo;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkResult vkerr = VK_SUCCESS;
    GroubiksResult_t err = 0;
    float queuePrioriy = 1.0f;

    vector_t(VkDeviceQueueCreateInfo) createInfos = null_vector(VkDeviceQueueCreateInfo);
    err = _setupQueues(advc, &queuePrioriy, &createInfos);
    if (err != 0)
    { goto error; } 

    memzero(deviceFeatures);
    memzero(deviceCreateInfo);
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = createInfos.data;
    deviceCreateInfo.queueCreateInfoCount = createInfos.size;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    /* set device-specific validationlayers (not required for newer versions) and extensions */
    deviceCreateInfo.enabledExtensionCount = ext->m_extensions.size;
    deviceCreateInfo.ppEnabledExtensionNames = (const char**)ext->m_extensions.data;
    deviceCreateInfo.enabledLayerCount = ext->m_validationlayers.size;
    deviceCreateInfo.ppEnabledLayerNames = (const char**)ext->m_validationlayers.data;
    
    vkerr = vkCreateDevice(advc->m_physical_device, &deviceCreateInfo, NULL, &advc->m_logical_device);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    log_info("successfully created logical-device");
    return 0;
error:
    log_error("failed to create logical-device.");
    free_vector(&createInfos);
    return -1;
}

GroubiksResult_t _setupQueues(VulkanActiveDevice_t* advc, float* priority, vector_t(VkDeviceQueueCreateInfo)* vec) {
    GroubiksResult_t err = 0;
    
    vector_t(uint32_t) uniqueQueueFamilies = null_vector(uint32_t);
    /* find unique queue-families amongs every queue-family index */
    /* i don't have std::set, so this is a bit messy */
    if (!vector_contains(uint32_t, &uniqueQueueFamilies, advc->m_family_indices.m_graphics_family.value))
    { vector_push_back(uint32_t, &uniqueQueueFamilies, advc->m_family_indices.m_graphics_family.value, &err); }
    if (err != 0)
    { goto error; }
    if (!vector_contains(uint32_t, &uniqueQueueFamilies, advc->m_family_indices.m_present_family.value))
    { vector_push_back(uint32_t, &uniqueQueueFamilies, advc->m_family_indices.m_present_family.value, &err); }
    if (err != 0)
    { goto error; }

    vector_for_each(uint32_t, &uniqueQueueFamilies, queueFamily)
    {
        VkDeviceQueueCreateInfo queueCreateInfo;
        memzero(queueCreateInfo);
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = *queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = priority;
        vector_push_back(VkDeviceQueueCreateInfo, vec, queueCreateInfo, &err);
        if (err != 0)
        { goto error; }
    }
    return 0;
error:
    free_vector(&uniqueQueueFamilies);
    return -1;
}

GroubiksResult_t _retrieveQueueHandles(VulkanActiveDevice_t* advc) {
    GroubiksResult_t err = 0;
    advc->m_queues = make_vector(VkQueue, NULL, 2, &err);
    if (err != 0)
    { log_error("failed to retrieve queue-handles."); return -1; }
    vkGetDeviceQueue(advc->m_logical_device, 
            advc->m_family_indices.m_graphics_family.value, 
            0, 
            vector_at(VkQueue, &advc->m_queues, GRAPHICS_QUEUE_INDEX));
    vkGetDeviceQueue(advc->m_logical_device,
            advc->m_family_indices.m_present_family.value,
            0,
            vector_at(VkQueue, &advc->m_queues, PRESENT_QUEUE_INDEX));
    log_info("successfully retrieved queue-handles.");
    return 0;   
}

void DestroyVulkanActiveDevice(VulkanActiveDevice advc) {
    if (advc == NULL)
    { return; }
    if (advc->m_logical_device != VK_NULL_HANDLE)
    { vkDestroyDevice(advc->m_logical_device, NULL); }
    free_vector(&advc->m_queues);
    free(advc);
}
