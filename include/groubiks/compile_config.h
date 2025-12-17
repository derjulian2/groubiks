
#ifndef GROUBIKS_COMPILETIME_CONFIG_H
#define GROUBIKS_COMPILETIME_CONFIG_H

/**
 * @file config.h
 * @author Julian Benzel
 * @brief compile-time options for groubiks bundled in one file.
 */

#include <vulkan/vulkan_core.h>
#define RENDERER_LOG 4

#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480
#define DEFAULT_APPLICATION_NAME "groubiks - rubiks-cube simulator"

#define VK_VALIDATIONLAYERS (const char*[]) { "VK_LAYER_KHRONOS_validation" }
#define VK_EXTENSIONS (const char*[]) { VK_EXT_DEBUG_UTILS_EXTENSION_NAME }

#define VK_NUM_VALIDATIONLAYERS 1
#define VK_NUM_EXTENSIONS 1

#define VK_DEFAULT_DEBUGMESSAGE_CALLBACK &VulkanContext_debug_callback

static const char* logs_init_fail_str    = "[ERROR] failed to initialize logging-system\n";
static const char* glfw_init_fail_str    = "[ERROR] failed to initialize GLFW\n";
static const char* glfw_win_fail_str     = "[ERROR] failed to initialize GLFWwindow\n";
static const char* vk_context_fail_str   = "[ERROR] failed to initialize Vulkan-Context\n";
static const char* vk_vlayers_fail_str   = "[ERROR] Vulkan validation-layers unavailable\n"; 
static const char* vk_instance_fail_str  = "[ERROR] Vulkan-instance could not be created\n";
static const char* vk_getdevice_fail_str = "[ERROR] Vulkan failed to retrieve available devices\n";

#endif