
#ifndef GROUBIKS_INITMANAGER_H
#define GROUBIKS_INITMANAGER_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

/**
 * @brief data-structure to handle Vulkan-Validation-Layers and
 *        Extensions.
 * @todo  make this implementation more generic to be usable for
 *        regular but also device-specific extensions/validation-layers.
 */
typedef struct {
    vector_t(cstring_t) m_validationlayers;
    vector_t(cstring_t) m_extensions;
} VulkanExtensions_t;

GroubiksResult_t CreateVulkanExtensions(VulkanExtensions_t* ext, 
        const char** validationLayers,
        uint32_t numValidationLayers,
        const char** extensionNames,
        uint32_t numExtensionsNames);
GroubiksResult_t _setupGLFWExtensions(VulkanExtensions_t* ext);

void DestroyVulkanExtensions(VulkanExtensions_t* ext);

GroubiksResult_t VulkanExtensions_VerifyValidationLayers(VulkanExtensions_t* ext);
GroubiksResult_t VulkanExtensions_VerifyExtensions(VulkanExtensions_t* ext);

#endif