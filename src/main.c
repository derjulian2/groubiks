
#include <GLFW/glfw3.h>
#include <groubiks/cube.h>
#include <groubiks/graphics/graphics.h>

#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

const int glfw_window_width  = 640;
const int glfw_window_height = 480;

const char* vk_validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const int vk_num_validationLayers = sizeof(vk_validationLayers) / sizeof(const char*);

const char* application_title   = "groubiks - cube simulator";
const char* glfw_init_fail_str    = "[ERROR] failed to initialize GLFW\n";
const char* glfw_win_fail_str     = "[ERROR] failed to initialize GLFWwindow\n";
const char* vk_context_fail_str   = "[ERROR] failed to initialize Vulkan-Context\n";
const char* vk_vlayers_fail_str   = "[ERROR] Vulkan validation-layers unavailable\n"; 
const char* vk_instance_fail_str  = "[ERROR] Vulkan-instance could not be created\n";
const char* vk_getdevice_fail_str = "[ERROR] Vulkan failed to retrieve available devices\n";

int main(int argc, char** argv) {

    if (glfwInit() != GLFW_TRUE) {
        fputs(glfw_init_fail_str, stderr);
        return -1;
    }

    VulkanContext_t vulkan_ctx;
    if (CreateVulkanContext(&vulkan_ctx, application_title, vk_validationLayers, vk_num_validationLayers) != 0) {
        fputs(vk_context_fail_str, stderr);
        return -1;
    }

    if (VulkanContext_GetDevices(&vulkan_ctx) != 0) {
        return -1;
    }

    for (int i = 0; i < vulkan_ctx.m_devices.size; ++i) {
        VkPhysicalDeviceProperties p;
        vkGetPhysicalDeviceProperties(vulkan_ctx.m_devices.data[i], &p);
        printf("devicename: %s\n",p.deviceName);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(
        glfw_window_width,  
        glfw_window_height, 
        application_title,
        NULL,
        NULL
    );

    if (win == NULL) {
        fputs(glfw_win_fail_str, stderr);
        return -1;
    }

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
    }

    DestroyVulkanContext(&vulkan_ctx);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}