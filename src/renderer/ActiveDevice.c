
#include <groubiks/renderer/ActiveDevice.h>

define_vector(VkQueue);

GroubiksResult_t CreateVulkanActiveDevice(VulkanActiveDevice_t* advc, 
        VkPhysicalDevice phdvc, 
        const char** validationLayers,
        uint32_t numValidationLayers,
        const char** extensionNames,
        uint32_t numExtensionsNames) {
    assert(advc != NULL);

    memzero(*advc);
    advc->m_physical_device = phdvc;
    if (CreateVulkanExtensions(&advc->m_extensions, 
        validationLayers, numValidationLayers, 
        extensionNames, numExtensionsNames) ||
        _setupQueueFamilyIndices(advc) ||
        _setupLogicalDevice(advc) ||
        _setupQueues(advc))
    { return -1; }

    return 0;
}

GroubiksResult_t _setupQueueFamilyIndices(VulkanActiveDevice_t* advc) {
    uint32_t queueFamilyCount = 0;
    VkQueueFamilyProperties* props = NULL;
    vkGetPhysicalDeviceQueueFamilyProperties(advc->m_physical_device, &queueFamilyCount, NULL);
    if (queueFamilyCount == 0)
    { log_warning("could not find any queue-families with suitable device."); return -1; }
    props = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    if (props == NULL)
    { return -1; }
    vkGetPhysicalDeviceQueueFamilyProperties(advc->m_physical_device, &queueFamilyCount, props);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            advc->m_family_indices.m_graphics_family = make_optional(uint32_t, i);
            break;
        }
    }
    if (!advc->m_family_indices.m_graphics_family.has_value)
    { log_warning("suitable device does not support graphics-queue-family."); goto error; }
error:
    free(props);
    return 0;
}

GroubiksResult_t _setupLogicalDevice(VulkanActiveDevice_t* advc) {
    VkDeviceQueueCreateInfo queueCreateInfo;
    VkDeviceCreateInfo deviceCreateInfo;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkResult vkerr = VK_SUCCESS;
    float queuePriority = 1.0f;

    memzero(queueCreateInfo);
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = advc->m_family_indices.m_graphics_family.value;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    memzero(deviceFeatures);
    memzero(deviceCreateInfo);
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    /* set device-specific validationlayers (not required for newer versions) and extensions */
    deviceCreateInfo.enabledExtensionCount = advc->m_extensions.m_extensions.size;
    deviceCreateInfo.ppEnabledExtensionNames = (const char**)advc->m_extensions.m_extensions.data;
    deviceCreateInfo.enabledLayerCount = advc->m_extensions.m_validationlayers.size;
    deviceCreateInfo.ppEnabledLayerNames = (const char**)advc->m_extensions.m_validationlayers.data;
    
    vkerr = vkCreateDevice(advc->m_physical_device, &deviceCreateInfo, NULL, &advc->m_logical_device);
    if (vkerr != VK_SUCCESS)
    { log_error("failed to create logical-device."); return -1; }
    return 0;
}

GroubiksResult_t _setupQueues(VulkanActiveDevice_t* advc) {
    GroubiksResult_t err = 0;
    advc->m_queues = make_vector(VkQueue, NULL, 1, &err);
    if (err != 0)
    { return -1; }
    vkGetDeviceQueue(advc->m_logical_device, 
            advc->m_family_indices.m_graphics_family.value, 
            0, 
            vector_at(VkQueue, &advc->m_queues, 0));
    return 0;
}

void DestroyVulkanActiveDevice(VulkanActiveDevice_t* advc) {
    assert(advc != NULL);
    DestroyVulkanExtensions(&advc->m_extensions);
    vkDestroyDevice(advc->m_logical_device, NULL);
    free_vector(VkQueue, &advc->m_queues);
}