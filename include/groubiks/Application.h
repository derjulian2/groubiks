
#ifndef GROUBIKS_APPLICATION_H
#define GROUBIKS_APPLICATION_H

#include <GLFW/glfw3.h>
#include <groubiks/renderer/VulkanContext.h>
#include <groubiks/compile_config.h>
#include <groubiks/utility/log.h>

#include <assert.h>

typedef struct {
    VulkanContext m_vulkanctx;
    RenderContext m_rndrctx;
    GLFWwindow* m_window;
} GroubiksApplication_t;

GroubiksResult_t Groubiks_Init(GroubiksApplication_t* app);
GroubiksResult_t Groubiks_Execute(GroubiksApplication_t* app);
void Groubiks_Destroy(GroubiksApplication_t* app);

#endif