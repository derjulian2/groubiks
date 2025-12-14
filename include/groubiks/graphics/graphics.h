
#ifndef GROUBIKS_GRAPHICS_H
#define GROUBIKS_GRAPHICS_H

/**
 * @file graphics.h
 * @brief created with the help of https://vulkan-tutorial.com/Introduction
 */

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <utility/vector.h>

/**
 * @brief struct to hold everything the application
 *        needs for interacting with GLFW.
 */
typedef struct {
    GLFWwindow* window;
} GLFWContext_t;
/**
 * @brief useful information and settings for
 *        debugging a Vulkan-context.
 */
typedef struct {
    const char**                m_validationlayers;
    int                         m_validationlayer_count;

    VkDebugUtilsMessengerEXT    m_debugmessenger;
} VulkanDebugContext_t;
/**
 * @brief Vector to hold available devices.
 */
declare_vector(VkPhysicalDevice);
typedef vector_t(VkPhysicalDevice) VulkanDeviceVector_t;
/**
 * @brief struct to hold everything the application
 *        needs for interacting with Vulkan.
 */
typedef struct {
    VkInstance              m_instance;
    VkApplicationInfo       m_appinfo;
    VulkanDeviceVector_t    m_devices;
#ifndef NDEBUG
    VulkanDebugContext_t    m_debugcontext;
#endif
} VulkanContext_t;

VkResult CreateVulkanContext(VulkanContext_t* ctx, const char* appName, const char** vlayers, const int vlayers_num);
void DestroyVulkanContext(VulkanContext_t* ctx);

VkResult CreateGLFWContext(GLFWContext_t* ctx);
void DestoryGLFWContext(GLFWContext_t* ctx);

VkResult VulkanContext_VerifyValidationLayers(VulkanContext_t* ctx, const char** vlayers, const int num);
VkResult VulkanContext_GetDevices(VulkanContext_t* ctx);

#endif