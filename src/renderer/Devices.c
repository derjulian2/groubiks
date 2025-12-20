
#include <groubiks/renderer/Devices.h>

define_vector(VkPhysicalDevice);

VulkanDevices CreateVulkanDevices(VkInstance instance, const char** extensionNames, uint32_t numExtensionsNames) {
    VulkanDevices dvcs = malloc(sizeof(VulkanDevices_t));
    if (dvcs == NULL)
    { return NULL; }
    memzero(*dvcs);
    dvcs->m_device_extensions = CreateVulkanExtensions(
            VK_VALIDATIONLAYERS, VK_NUM_VALIDATIONLAYERS,
            extensionNames, numExtensionsNames);
    if (dvcs->m_device_extensions == NULL ||
        _setupAvailableDevices(dvcs, instance))
    { goto error; }
    log_info("successfully retrieved physical devices.");
    return dvcs;
error:
    log_error("failed to retrieve physical devices.");
    DestroyVulkanDevices(dvcs);
    return NULL;
}

GroubiksResult_t _setupAvailableDevices(VulkanDevices_t* dvcs, VkInstance instance) {
    assert(dvcs != NULL && instance != NULL);
    GroubiksResult_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    uint32_t deviceCount = 0;

    vkerr = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (vkerr != VK_SUCCESS)
    { return -1; }
    if (deviceCount == 0)
    { return -1; }
    dvcs->m_available_devices = make_vector(VkPhysicalDevice, NULL, deviceCount, &err);
    if (err != 0)
    { return -1; }
    vkerr = vkEnumeratePhysicalDevices(instance, &deviceCount, dvcs->m_available_devices.data);
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

GroubiksResult_t _deviceHasExtensions(VkPhysicalDevice dvc, VulkanExtensions ext) {
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
    for (uint32_t i = 0; i < ext->m_extensions.size; ++i) {
        bool hasExtension = false;
        for (uint32_t j = 0; j < extensionCount; ++j) {
            if (strcmp(ext->m_extensions.data[i], props[j].extensionName))
            { hasExtension = true; break; }
        }
        if (!hasExtension)
        { res = 0; goto cleanup;  }
    }
cleanup:
    free(props);
    return res;
}

void DestroyVulkanDevices(VulkanDevices dvcs) {
    if (dvcs == NULL)
    { return; }
    free_vector(&dvcs->m_available_devices);
    DestroyVulkanExtensions(dvcs->m_device_extensions);
}