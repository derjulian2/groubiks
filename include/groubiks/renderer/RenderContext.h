
#ifndef GROUBIKS_PRESENTATION_H
#define GROUBIKS_PRESENTATION_H

#include <GLFW/glfw3.h>
#include <groubiks/utility/common.h>
#include <groubiks/utility/vector.h>

declare_vector(VkImage);

typedef struct {
    VkSurfaceKHR m_surface;
    
    VkSwapchainKHR m_swapchain;
    VkExtent2D m_swapchain_extent;
    VkFormat m_swapchain_format;
    vector_t(VkImage) m_swapchain_images;
} RenderContext_t;

GroubiksResult_t CreateRenderContext(RenderContext_t* rndr_ctx);
GroubiksResult_t _setupSwapChain(RenderContext_t* rndr_ctx);

void DestroyRenderContext(RenderContext_t* rndr_ctx);

#endif