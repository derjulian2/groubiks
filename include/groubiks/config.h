
#ifndef GROUBIKS_COMPILETIME_CONFIG_H
#define GROUBIKS_COMPILETIME_CONFIG_H

/**
 * @file config.h
 * @author Julian Benzel
 * @brief compile-time options for groubiks bundled in one file.
 */

#define RENDERER_LOG 4

static const int glfw_window_width  = 640;
static const int glfw_window_height = 480;

static const char* vk_validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
static const int vk_num_validationLayers = sizeof(vk_validationLayers) / sizeof(const char*);

static const char* application_title   = "groubiks - cube simulator";

static const char* logs_init_fail_str    = "[ERROR] failed to initialize logging-system\n";
static const char* glfw_init_fail_str    = "[ERROR] failed to initialize GLFW\n";
static const char* glfw_win_fail_str     = "[ERROR] failed to initialize GLFWwindow\n";
static const char* vk_context_fail_str   = "[ERROR] failed to initialize Vulkan-Context\n";
static const char* vk_vlayers_fail_str   = "[ERROR] Vulkan validation-layers unavailable\n"; 
static const char* vk_instance_fail_str  = "[ERROR] Vulkan-instance could not be created\n";
static const char* vk_getdevice_fail_str = "[ERROR] Vulkan failed to retrieve available devices\n";

#endif