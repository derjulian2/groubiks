
#ifndef GROUBIKS_VK_EXTRAS_H
#define GROUBIKS_VK_EXTRAS_H

/**
 * @file vulkan_extras.h
 * @date 22/12/25
 * @author Julian Benzel
 * @brief data-structure to handle vulkan-validationlayers and
 *        extensions for instances and devices.
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>

declare_vector(VkLayerProperties);
declare_vector(VkExtensionProperties);

struct vk_extras {
    vector_t(cstring_t) m_validationlayers;
    vector_t(cstring_t) m_extensions;
};

groubiks_result_t vk_extras_get_glfw(struct vk_extras* pExt);

groubiks_result_t vk_extras_match_instance(const struct vk_extras* pExt);
groubiks_result_t vk_extras_match_device(const struct vk_extras* pExt, VkPhysicalDevice device);

groubiks_result_t vk_extras_check_layers(const vector_t(cstring_t)* pRequestedLayers, const vector_t(VkLayerProperties)* pLayers);
groubiks_result_t vk_extras_check_extensions(const vector_t(cstring_t)* pRequestedExtensions, const vector_t(VkExtensionProperties)* pExtensions);

#endif