
#ifndef GROUBIKS_DEVICE_CTX_H
#define GROUBIKS_DEVICE_CTX_H

/**
 * @file vulkan_device_context.h
 * @date 22/12/25
 * @author Julian Benzel
 * @brief data-structures to manage a link between the program
 *        and a vulkan physical/logical device and it's queues.
 *        credits to https://vulkan-tutorial.com
 */

#include <GLFW/glfw3.h> // includes vulkan, GLFW_INCLUDE_VULKAN set via CMake
#include <groubiks/utility/vector.h>
#include <groubiks/utility/optional.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/log.h>
#include <groubiks/compile_config.h>
#include <groubiks/renderer/vulkan_extras.h>

declare_optional(uint32_t);

struct vk_queue_family_indices {
    optional_t(uint32_t) m_graphics_family;
    optional_t(uint32_t) m_present_family;
};

bool vk_qfis_complete(const struct vk_queue_family_indices* qfis);

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

groubiks_result_t make_vk_device_ctx(struct vk_device_context* pDeviceContext, 
    VkPhysicalDevice physDevice,
    VkSurfaceKHR surface, 
    struct vk_extras* pExtras);

void free_vk_device_ctx(struct vk_device_context* pDeviceContext);

groubiks_result_t vk_device_ctx_find_qfis(struct vk_device_context* pDeviceContext, VkSurfaceKHR surface);
groubiks_result_t vk_device_ctx_setup_logical_device(struct vk_device_context* pDeviceContext, struct vk_extras* pExtras);
groubiks_result_t vk_device_ctx_is_device_suitable();
groubiks_result_t vk_device_ctx_get_queues(struct vk_device_context* pDeviceContext);

#endif