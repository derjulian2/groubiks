
#ifndef GROUBIKS_COMMANDS_H
#define GROUBIKS_COMMANDS_H

#include <GLFW/glfw3.h>
#include <groubiks/renderer/ActiveDevice.h>

typedef struct {
    VkCommandPool m_commandpool;
    VkCommandBuffer m_commandbuffer;
    VkSemaphore m_imageavailable_sem;
    VkSemaphore m_renderfinished_sem;
    VkFence m_inflight_fen;
} VulkanCommands_t;
typedef VulkanCommands_t* VulkanCommands;

VulkanCommands CreateVulkanCommands(VulkanActiveDevice device);
void DestroyVulkanCommands(VulkanCommands commands, VulkanActiveDevice device);



#endif