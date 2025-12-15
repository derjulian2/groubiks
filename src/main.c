
#include <GLFW/glfw3.h>
#include <groubiks/cube.h>
#include <groubiks/graphics.h>
#include <groubiks/log.h>
#include <groubiks/config.h>

int main(int argc, char** argv) {
    FILE* logfile = fopen("renderer.log", "w");
    if (logfile == NULL) {
        return -1;
    }
#ifndef GROUBIKS_NO_LOGS
    if (log_init() != 0 || (log_new(logfile, "[RENDERER]", 0)) == -1) { 
        fputs(logs_init_fail_str, stderr);
        return -1;
    }
#endif

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
#ifndef GROUBIKS_NO_LOGS
    fclose(logfile);
    log_end();
#endif
    return 0;
}