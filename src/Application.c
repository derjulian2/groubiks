
#include <groubiks/Application.h>

GroubiksResult_t Groubiks_Init(GroubiksApplication_t* app) {
    assert(app != NULL);
    const char* glfwErr;
    GroubiksResult_t err = 0;

    err = log_init();
    if (err != 0) 
    { fputs("failed to initialize logging-system.", stderr); return -1; }
    
    err = glfwInit();
    if (err != GLFW_TRUE)
    { log_error("failed to initialize GLFW."); return -1; }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    app->m_window = glfwCreateWindow(
        DEFAULT_WIN_WIDTH, 
        DEFAULT_WIN_HEIGHT, 
        DEFAULT_APPLICATION_NAME, 
        NULL, NULL);
    if (app->m_window == NULL) {
        glfwGetError(&glfwErr);
        logf_error("failed to create GLFW-window: %s", glfwErr);
        return -1;
    }

    app->m_vulkanctx = CreateVulkanContext();
    if (app->m_vulkanctx == NULL) 
    { log_error("failed to create Vulkan-Context."); return -1; }

    app->m_rndrctx = VulkanContext_AddRenderContext(app->m_vulkanctx, app->m_window);
    if (app->m_rndrctx == NULL)
    { log_error("failed to create Vulkan-Rendering-Context"); return -1; }

    return 0;
}

GroubiksResult_t Groubiks_Execute(GroubiksApplication_t* app) {
    assert(app != NULL);
    
    while (!glfwWindowShouldClose(app->m_window)) {
        glfwPollEvents();
        VulkanContext_Draw(app->m_vulkanctx, app->m_rndrctx);
    }
    vkDeviceWaitIdle(app->m_rndrctx->m_device->m_logical_device);

    return 0;
}

void Groubiks_Destroy(GroubiksApplication_t* app) {
    assert(app != NULL);
    DestroyVulkanContext(app->m_vulkanctx);
    glfwDestroyWindow(app->m_window);
    glfwTerminate();
    log_end();
}