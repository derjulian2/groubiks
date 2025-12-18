
#ifndef GROUBIKS_DEVICEMANAGER_H
#define GROUBIKS_DEVICEMANAGER_H

#include <GLFW/glfw3.h>
#include <groubiks/renderer/ActiveDevice.h>
#include <groubiks/utility/vector.h>
#include <groubiks/utility/common.h>

declare_vector(VkPhysicalDevice);

typedef struct {
    vector_t(VkPhysicalDevice) m_available_devices;
    vector_t(VulkanActiveDevice_t) m_active_devices;
} VulkanDevices_t;

GroubiksResult_t CreateVulkanDevices(VulkanDevices_t* dvcs, VkInstance* instance);
void DestroyVulkanDevices(VulkanDevices_t* dvcs);

GroubiksResult_t _setupAvailableDevices(VulkanDevices_t* dvcs, VkInstance* instance);
GroubiksResult_t _isDeviceSuitable(VkPhysicalDevice dvc);
GroubiksResult_t _deviceHasExtensions(VkPhysicalDevice dvc, const char** extensionNames, uint32_t numExtensionsNames);
GroubiksResult_t _checkQueueFamilies(VkPhysicalDevice dvc);
GroubiksResult_t _setupActiveDevices(VulkanDevices_t* dvcs, VkInstance* instance);

#endif