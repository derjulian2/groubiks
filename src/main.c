
#include <groubiks/compile_config.h>

#include <groubiks/cube.h>
#include <groubiks/utility/log.h>
#include <groubiks/renderer/vulkan_context.h>

int main(int argc, char** argv) {
    Renderer_result_t err = 0;
    FILE* logfile = fopen("renderer.log", "w");
    if (logfile == NULL) {
        return -1;
    }
#ifndef GROUBIKS_NO_LOGS
    if (log_init() != 0 || (log_new(logfile, "[RENDERER]", 0)) == -1) { 
        fputs(logs_init_fail_str, stderr);
        return -1;
    }
    log_redirect_all_to(logfile);
#endif

    if (glfwInit() != GLFW_TRUE) {
        fputs(glfw_init_fail_str, stderr);
        return -1;
    }

    VulkanContext_t vulkan_ctx;
    err = CreateVulkanContext(&vulkan_ctx);
    if (err != 0) {
        log_error("failed to initialize Vulkan-Context");
        return err;
    }
    err = VulkanContext_GetDevices(&vulkan_ctx);
    if (err != 0) {
        log_error("failed to get devices");
        return err;
    }
    for (int i = 0; i < vulkan_ctx.m_device_mngr.m_available_devices.size; ++i) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(*vector_at(VkPhysicalDevice, &vulkan_ctx.m_device_mngr.m_available_devices, i), &props);
        printf("device: %s\n", props.deviceName);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(
        DEFAULT_WIN_WIDTH,  
        DEFAULT_WIN_HEIGHT, 
        DEFAULT_APPLICATION_NAME,
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