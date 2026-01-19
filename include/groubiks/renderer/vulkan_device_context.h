
#ifndef GROUBIKS_DEVICE_CTX_H
#define GROUBIKS_DEVICE_CTX_H

/**
 * @file vulkan_device_context.h
 * @date 19/01/26
 * @author Julian Benzel
 * @brief data-structures to manage a link between the program
 *        and a vulkan physical/logical device and it's queues.
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/optional.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>
#include <groubiks/renderer/vulkan_extras.h>
#include <groubiks/renderer/vulkan_fill_structs.h>

declare_optional(u32);
declare_dynarray(VkQueueFamilyProperties, VkQFamilyProps);
declare_dynarray(VkDeviceQueueCreateInfo, VkDeviceQCreateInfo);

struct vk_queue_family_indices {
    optional_t(u32) m_graphics_family;
    optional_t(u32) m_present_family;
};

struct vk_queues {
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
};

struct vk_device_context {
    VkPhysicalDevice m_physical_device;
    VkDevice m_logical_device;
    struct vk_queue_family_indices m_qfis;
    struct vk_queues m_queues;
};

#define vk_queue_family_indices_null \
(struct vk_queue_family_indices) { \
    .m_graphics_family = nullopt(u32), \
    .m_present_family  = nullopt(u32)  \
}

#define vk_queues_null \
(struct vk_queues) { \
    .m_graphics_queue = VK_NULL_HANDLE, \
    .m_present_queue  = VK_NULL_HANDLE  \
}

#define vk_device_context_null \
(struct vk_device_context) { \
    .m_physical_device = VK_NULL_HANDLE, \
    .m_logical_device  = VK_NULL_HANDLE, \
    .m_qfis            = vk_queue_family_indices_null, \
    .m_queues          = vk_queues_null \
}

bool
vk_qfis_complete(const struct vk_queue_family_indices* qfis);

groubiks_result_t 
vk_device_context_create(struct vk_device_context* pDeviceContext, 
    VkPhysicalDevice physDevice,
    VkSurfaceKHR surface, 
    struct vk_extras* pExtras
);

void 
free_vk_device_context(struct vk_device_context* pDeviceContext);

groubiks_result_t 
vk_device_context_get_qfis(struct vk_device_context* pDeviceContext, 
    VkSurfaceKHR surface
);

groubiks_result_t 
vk_device_context_setup_logical_device(struct vk_device_context* pDeviceContext, 
    struct vk_extras* pExtras
);

groubiks_result_t 
vk_device_context_retrieve_queues(struct vk_device_context* pDeviceContext);

#endif