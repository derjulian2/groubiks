
#include <groubiks/renderer/Commands.h>

VulkanCommands CreateVulkanCommands(VulkanActiveDevice device) {
    VkResult vkerr = VK_SUCCESS;
    VulkanCommands commands = NULL;
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    VkCommandBufferAllocateInfo commandBufferAllocInfo;
    VkSemaphoreCreateInfo semCreateInfo;
    VkFenceCreateInfo fenCreateInfo;

    commands = malloc(sizeof(VulkanCommands_t));
    if (commands == NULL)
    { goto error; }
    memzero(*commands);

    /* setup commandpool */
    memzero(commandPoolCreateInfo);
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = device->m_family_indices.m_graphics_family.value;

    vkerr = vkCreateCommandPool(device->m_logical_device, &commandPoolCreateInfo, NULL, &commands->m_commandpool);
    if (vkerr != VK_SUCCESS)
    { goto error; }

    memzero(commandBufferAllocInfo);
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = commands->m_commandpool;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = 1;

    vkerr = vkAllocateCommandBuffers(device->m_logical_device, &commandBufferAllocInfo, &commands->m_commandbuffer);
    if (vkerr != VK_SUCCESS)
    { goto error; }

    memzero(semCreateInfo);
    memzero(fenCreateInfo);
    semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    fenCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkerr = vkCreateSemaphore(device->m_logical_device, &semCreateInfo, NULL, &commands->m_imageavailable_sem);
    vkerr |= vkCreateSemaphore(device->m_logical_device, &semCreateInfo, NULL, &commands->m_renderfinished_sem);
    vkerr |= vkCreateFence(device->m_logical_device, &fenCreateInfo, NULL, &commands->m_inflight_fen);
    if (vkerr != VK_SUCCESS)
    { goto error; }
    log_info("successfully created commands-object.");
    return commands;
error:
    log_error("failed to create commands-object.");
    if (commands->m_commandpool != VK_NULL_HANDLE)
    { vkDestroyCommandPool(device->m_logical_device, commands->m_commandpool, NULL); }
    if (commands->m_imageavailable_sem != VK_NULL_HANDLE)
    { vkDestroySemaphore(device->m_logical_device, commands->m_imageavailable_sem, NULL); }
    if (commands->m_renderfinished_sem != VK_NULL_HANDLE)
    { vkDestroySemaphore(device->m_logical_device, commands->m_renderfinished_sem, NULL); }
    if (commands->m_inflight_fen != VK_NULL_HANDLE)
    { vkDestroyFence(device->m_logical_device, commands->m_inflight_fen, NULL); }
    free(commands);
    return NULL;
}

void DestroyVulkanCommands(VulkanCommands commands, VulkanActiveDevice device) {
    vkDestroyCommandPool(device->m_logical_device, commands->m_commandpool, NULL);
    vkDestroySemaphore(device->m_logical_device, commands->m_imageavailable_sem, NULL);
    vkDestroySemaphore(device->m_logical_device, commands->m_renderfinished_sem, NULL);
    vkDestroyFence(device->m_logical_device, commands->m_inflight_fen, NULL);
}