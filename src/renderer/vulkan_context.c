
#include <groubiks/renderer/vulkan_context.h>

Renderer_result_t CreateVulkanContext(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    Renderer_result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo instanceCreateInfo;

    memzero(*ctx);
    memzero(appInfo);
    memzero(instanceCreateInfo);

    /* create member-managers */
    err = CreateVulkanDeviceManager(&ctx->m_device_mngr);
    if (err != 0) {
        return err;
    }
    err = CreateVulkanExtensionManager(&ctx->m_ext_mngr);
    if (err != 0) {
        return err;
    }

    /* setup instance-creation */
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = DEFAULT_APPLICATION_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.ppEnabledLayerNames = (const char*const*)ctx->m_ext_mngr.m_validationlayers.data;
    instanceCreateInfo.enabledLayerCount = ctx->m_ext_mngr.m_validationlayers.size;
    instanceCreateInfo.ppEnabledExtensionNames = (const char*const*)ctx->m_ext_mngr.m_extensions.data;
    instanceCreateInfo.enabledExtensionCount = ctx->m_ext_mngr.m_extensions.size;
    
    vkerr = vkCreateInstance(&instanceCreateInfo, NULL, &ctx->m_instance);
    if (vkerr != VK_SUCCESS) {
        err = -1; return err;
    }
    return err;
}

Renderer_result_t VulkanContext_GetDevices(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    Renderer_result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    uint32_t device_count = 0;

    /* get physical devices*/
    vkerr = vkEnumeratePhysicalDevices(ctx->m_instance, &device_count, NULL);
    if (vkerr != VK_SUCCESS) {
        err = -1; return err;
    }
    ctx->m_device_mngr.m_available_devices = make_vector(VkPhysicalDevice, NULL, device_count, &err);
    if (err != 0) {
        return err;
    }
    vkerr = vkEnumeratePhysicalDevices(ctx->m_instance, &device_count, ctx->m_device_mngr.m_available_devices.data);
    if (vkerr != VK_SUCCESS) {
        err = -1; return err;
    }
    /* get logical devices */
    return err;
}

Renderer_result_t CreateVulkanDeviceManager(VulkanDeviceManager_t* dvc_mngr) {
    assert(dvc_mngr != NULL);
    Renderer_result_t err = 0;

    dvc_mngr->m_available_devices = null_vector;
    dvc_mngr->m_logical_devices = null_vector;

    return err;
}

Renderer_result_t CreateVulkanExtensionManager(VulkanExtensionManager_t* ext_mngr) {
    assert(ext_mngr != NULL);
    Renderer_result_t err = 0;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = NULL;
    const char** vkExtensions = VK_EXTENSIONS;
    const char** vkLayers = VK_VALIDATIONLAYERS;

    /* initialize validationlayers */
    ext_mngr->m_validationlayers = make_vector(char*, NULL, VK_NUM_VALIDATIONLAYERS, &err);
    if (err != 0) {
        return err;
    }
    for (int i = 0; i < VK_NUM_VALIDATIONLAYERS; ++i) {
        char** str = vector_at(char*, &ext_mngr->m_validationlayers, i);
        *str = strdup(vkLayers[i]);
        if (*str == NULL) {
            err = -1; return err;
        }
    }
    err = VulkanExtensionManager_VerifyValidationLayers(ext_mngr);
    if (err != 0) {
        return err;
    }
    /* initialize extensions */
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (glfwExtensions == NULL) {
        err = -1; return err;
    }
    ext_mngr->m_extensions = make_vector(char*, NULL, glfwExtensionCount + VK_NUM_EXTENSIONS, &err);
    if (err != 0) {
        return err;
    }
    for (int i = 0; i < glfwExtensionCount; ++i) {
        char** str = vector_at(char*, &ext_mngr->m_extensions, i);
        *str = strdup(glfwExtensions[i]);
        if (*str == NULL) {
            err = -1; return err;
        }
    }
    for (int i = 0; i < VK_NUM_EXTENSIONS; ++i) {
        char** str = vector_at(char*, &ext_mngr->m_extensions, glfwExtensionCount + i);
        *str = strdup(vkExtensions[i]);
        if (*str == NULL) {
            err = -1; return err;
        }
    }
    /* initialize debugmessenger */

    return err;
}

Renderer_result_t VulkanExtensionManager_VerifyValidationLayers(VulkanExtensionManager_t* ext_mngr) {
    assert(ext_mngr != NULL);
    Renderer_result_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    uint32_t layerCount = 0;
    VkLayerProperties* layerProps = NULL;

    /* retrieve available validationlayers */
    vkerr = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if (vkerr != VK_SUCCESS)
    { err = -1; goto skip; }
    layerProps = malloc(sizeof(VkLayerProperties) * layerCount);
    if (layerProps == NULL)
    { err = -1; goto skip; }
    vkerr = vkEnumerateInstanceLayerProperties(&layerCount, layerProps);
    if (vkerr != VK_SUCCESS )
    { err = -1; goto skip; }
    /* compare with set validationlayers */
    for (int i = 0; i < ext_mngr->m_validationlayers.size; ++i) {
        int layerFound = 0;
        for (int j = 0; j < layerCount; ++j) {
            if (strcmp(*vector_at(char*, &ext_mngr->m_validationlayers, i), layerProps[j].layerName) == 0) {
                layerFound = 1;
                break;
            }
        }
        if (!layerFound) 
        { err = -1; goto skip; }
    }
skip:
    free(layerProps);
    return err;
}

Renderer_result_t VulkanExtensionManager_VerifyExtensions(VulkanExtensionManager_t* ext_mngr) {
    assert(ext_mngr != NULL);
    Renderer_result_t err = 0;
    VkResult vkerr = VK_SUCCESS;

    /* n/a */
    // logf(RENDERER_LOG, "%s() at %s exited with code: %d", __FUNCTION__, __FILE__, err);
    return err;
}

void DestroyVulkanContext(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    DestroyVulkanExtensionManager(&ctx->m_ext_mngr);
    DestroyVulkanDeviceManager(&ctx->m_device_mngr);
    vkDestroyInstance(ctx->m_instance, NULL);
}

void DestroyVulkanDeviceManager(VulkanDeviceManager_t* dvc_mngr) {
    assert(dvc_mngr != NULL);
    free_vector(&dvc_mngr->m_logical_devices);
    free_vector(&dvc_mngr->m_available_devices);
}

void DestroyVulkanExtensionManager(VulkanExtensionManager_t* ext_mngr) {
    assert(ext_mngr != NULL);
    free_vector(&ext_mngr->m_extensions);
    free_vector(&ext_mngr->m_validationlayers);
}


// static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext_debug_callback(
//     VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//     VkDebugUtilsMessageTypeFlagsEXT messageType,
//     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//     void* pUserData) {
//     switch (messageSeverity)
//     {
//     case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT):
//         log_info(pCallbackData->pMessage);
//     case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT):
//         log_warning(pCallbackData->pMessage);
//     case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT):
//         log_error(pCallbackData->pMessage);
//     default:
//         log(RENDERER_LOG, pCallbackData->pMessage);
//     }
//     return VK_FALSE;
// }