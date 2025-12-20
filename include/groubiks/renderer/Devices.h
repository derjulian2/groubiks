
#ifndef GROUBIKS_DEVICEMANAGER_H
#define GROUBIKS_DEVICEMANAGER_H

#include <GLFW/glfw3.h>
#include <groubiks/renderer/Extensions.h>
#include <groubiks/utility/vector.h>
#include <groubiks/utility/common.h>

declare_vector(VkPhysicalDevice);

typedef struct {
    VulkanExtensions m_device_extensions; // should hold for all devices that get added to active_devices
    vector_t(VkPhysicalDevice) m_available_devices;
} VulkanDevices_t;
typedef VulkanDevices_t* VulkanDevices;

VulkanDevices CreateVulkanDevices(VkInstance instance, const char** extensionNames, uint32_t numExtensionsNames);
void DestroyVulkanDevices(VulkanDevices dvcs);

GroubiksResult_t _setupAvailableDevices(VulkanDevices_t* dvcs, VkInstance instance);
GroubiksResult_t _isDeviceSuitable(VkPhysicalDevice dvc);
GroubiksResult_t _deviceHasExtensions(VkPhysicalDevice dvc, VulkanExtensions ext);

#endif