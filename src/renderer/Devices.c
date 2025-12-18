
#include <groubiks/renderer/Devices.h>

define_vector(VkPhysicalDevice);
define_vector(VulkanActiveDevice_t);

GroubiksResult_t CreateVulkanDevices(VulkanDevices_t* dvcs, VkInstance* instance) {
    assert(dvcs != NULL);
    GroubiksResult_t err = 0;
    if (_setupAvailableDevices(dvcs, instance) ||
        _setupActiveDevices(dvcs, instance))
    { log_error("failed to query and pick a device."); return -1; }
    return 0;
}

GroubiksResult_t _setupAvailableDevices(VulkanDevices_t* dvcs, VkInstance* instance) {
    assert(dvcs != NULL && instance != NULL);
    GroubiksResult_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    uint32_t deviceCount = 0;

    vkerr = vkEnumeratePhysicalDevices(*instance, &deviceCount, NULL);
    if (vkerr != VK_SUCCESS)
    { return -1; }
    if (deviceCount == 0)
    { log_warning("failed to find any devices with Vulkan-support."); return -1; }
    dvcs->m_available_devices = make_vector(VkPhysicalDevice, NULL, deviceCount, &err);
    if (err != 0)
    { return -1; }
    vkerr = vkEnumeratePhysicalDevices(*instance, &deviceCount, dvcs->m_available_devices.data);
    if (vkerr != VK_SUCCESS)
    { return -1; }
    return 0;
}

GroubiksResult_t _isDeviceSuitable(VkPhysicalDevice dvc) {
    assert(dvc != NULL);
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    
    vkGetPhysicalDeviceProperties(dvc, &props);
    vkGetPhysicalDeviceFeatures(dvc, &features);
    return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

GroubiksResult_t _deviceHasExtensions(VkPhysicalDevice dvc, const char** extensionNames, uint32_t numExtensionsNames) {
    GroubiksResult_t res = 1;
    uint32_t extensionCount = 0;
    VkResult vkerr = VK_SUCCESS;
    VkExtensionProperties* props;

    vkerr = vkEnumerateDeviceExtensionProperties(dvc, NULL, &extensionCount, NULL);
    if (vkerr != VK_SUCCESS)
    { return -1; }
    props = malloc(sizeof(VkExtensionProperties) * extensionCount);
    if (props == NULL) 
    { return -1; }
    vkerr = vkEnumerateDeviceExtensionProperties(dvc, NULL, &extensionCount, props);
    for (uint32_t i = 0; i < numExtensionsNames; ++i) {
        bool hasExtension = false;
        for (uint32_t j = 0; j < extensionCount; ++j) {
            if (strcmp(extensionNames[i], props[j].extensionName))
            { hasExtension = true; break; }
        }
        if (!hasExtension)
        { res = 0; goto cleanup;  }
    }
cleanup:
    free(props);
    return res;
}

GroubiksResult_t _setupActiveDevices(VulkanDevices_t* dvcs, VkInstance* instance) {
    assert(dvcs != NULL && instance != NULL);
    GroubiksResult_t err = 0;

    vector_for_each(VkPhysicalDevice, &dvcs->m_available_devices, device)
    {
        GroubiksResult_t hasExtensions = _deviceHasExtensions(*device, VK_DEVICE_EXTENSIONS, VK_NUM_DEVICE_EXTENSIONS);
        if (hasExtensions == -1)
        { return -1; }
        if (_isDeviceSuitable(*device) && hasExtensions)
        {
            VulkanActiveDevice_t active_device;
            err = CreateVulkanActiveDevice(&active_device, *device,
                VK_VALIDATIONLAYERS,
                VK_NUM_VALIDATIONLAYERS,
                VK_DEVICE_EXTENSIONS,
                VK_NUM_DEVICE_EXTENSIONS);
            if (err != 0)
            { return -1; }
            vector_push_back(VulkanActiveDevice_t, &dvcs->m_active_devices, active_device, &err);
            if (err != 0)
            { return -1; }
            break;
        }
    }
    return 0;
}

void DestroyVulkanDevices(VulkanDevices_t* dvcs) {
    assert(dvcs != NULL);
    free_vector(VkPhysicalDevice, &dvcs->m_available_devices);
    vector_for_each(VulkanActiveDevice_t, &dvcs->m_active_devices, active_device)
    { DestroyVulkanActiveDevice(active_device); }
    free_vector(VulkanActiveDevice_t, &dvcs->m_active_devices);
}