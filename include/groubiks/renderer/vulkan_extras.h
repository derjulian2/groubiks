
#ifndef GROUBIKS_VK_EXTRAS_H
#define GROUBIKS_VK_EXTRAS_H

/**
 * @file vulkan_extras.h
 * @date 19/01/26
 * @author Julian Benzel
 * @brief data-structure to handle vulkan-validationlayers and
 *        extensions for instances and devices.
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

declare_dynarray(VkLayerProperties, VkLayerProps);
declare_dynarray(VkExtensionProperties, VkExtProps);

struct vk_extras {
    struct dynarray(str) m_validationlayers;
    struct dynarray(str) m_extensions;
};

#define vk_extras_null \
(struct vk_extras) { \
    .m_validationlayers = null_dynarray(str), \
    .m_extensions       = null_dynarray(str)  \
}

groubiks_result_t 
vk_extras_get_glfw(struct vk_extras* pExt);

groubiks_result_t 
vk_extras_match_instance(const struct vk_extras* pExt);

groubiks_result_t 
vk_extras_match_device(const struct vk_extras* pExt, 
    VkPhysicalDevice device
); 

groubiks_result_t 
vk_extras_check_layers(const struct dynarray(str)* pRequestedLayers, 
    const struct dynarray(VkLayerProps)* pLayers
);

groubiks_result_t 
vk_extras_check_extensions(const struct dynarray(str)* pRequestedExtensions, 
    const struct dynarray(VkExtProps)* pExtensions
);

#endif