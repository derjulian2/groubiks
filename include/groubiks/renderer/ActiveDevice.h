
#ifndef GROUBIKS_ACTIVEDEVICE_H
#define GROUBIKS_ACTIVEDEVICE_H

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/vector.h>
#include <groubiks/utility/optional.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>
#include <groubiks/renderer/Extensions.h>

declare_vector(VkQueue);
declare_optional(uint32_t);

typedef struct {
    optional_t(uint32_t) m_graphics_family;
    optional_t(uint32_t) m_present_family;
} VulkanQueueFamilyIndices_t;

typedef struct {
    VkPhysicalDevice m_physical_device;
    VkDevice m_logical_device;
    VulkanExtensions_t m_extensions;
    VulkanQueueFamilyIndices_t m_family_indices;
    vector_t(VkQueue) m_queues;
} VulkanActiveDevice_t;

declare_vector(VulkanActiveDevice_t);

GroubiksResult_t CreateVulkanActiveDevice(VulkanActiveDevice_t* advc, 
        VkPhysicalDevice phdvc, 
        const char** validationLayers,
        uint32_t numValidationLayers,
        const char** extensionNames,
        uint32_t numExtensionsNames);
GroubiksResult_t _setupQueueFamilyIndices(VulkanActiveDevice_t* advc);
GroubiksResult_t _setupLogicalDevice(VulkanActiveDevice_t* advc);
GroubiksResult_t _setupQueues(VulkanActiveDevice_t* advc);

void DestroyVulkanActiveDevice(VulkanActiveDevice_t* advc);



#endif