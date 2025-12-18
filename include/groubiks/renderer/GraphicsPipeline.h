
#ifndef GROUBIKS_GRAPHICSPIPELINE_H
#define GROUBIKS_GRAPHICSPIPELINE_H

#include <GLFW/glfw3.h>

typedef struct {
    VkShaderModule m_vertex;
    VkShaderModule m_fragment;
    VkPipelineLayout m_layout;
} VulkanGraphicsPipeline_t;

#endif