
#include <groubiks/renderer/VulkanContext.h>

GroubiksResult_t CreateVulkanContext(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    memzero(*ctx);
    if (_setupExtensions(ctx)    ||
        _setupVulkanInstance(ctx) ||
        _setupDebugMessenger(ctx) ||
        _setupDevices(ctx))
    { return -1; }
    return 0;
}

GroubiksResult_t _setupExtensions(VulkanContext_t* ctx) {
    GroubiksResult_t err = 0;
    /* setup extensions and validationlayers via initmanager */
    err = CreateVulkanExtensions(&ctx->m_extensions,
        VK_VALIDATIONLAYERS,
        VK_NUM_VALIDATIONLAYERS,
        VK_EXTENSIONS,
        VK_NUM_EXTENSIONS);
    if (err != 0) 
    { log_error("failed to initialize VulkanExtensions."); return -1; }
    err = _setupGLFWExtensions(&ctx->m_extensions);
    if (err != 0)
    { log_error("failed to setup Vulkan GLFW-extensions"); return -1; }
    err = VulkanExtensions_VerifyValidationLayers(&ctx->m_extensions);
    if (err != 0) 
    { log_error("failed to verify Vulkan-validation-layers."); return -1; }
    return 0;
}

GroubiksResult_t _setupVulkanInstance(VulkanContext_t* ctx) {
    GroubiksResult_t err = 0;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo instanceCreateInfo;
    VkResult vkerr = VK_SUCCESS;
    /* instance-creation */
    memzero(appInfo);
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = DEFAULT_APPLICATION_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    memzero(instanceCreateInfo);
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.ppEnabledLayerNames = (const char*const*)ctx->m_extensions.m_validationlayers.data;
    instanceCreateInfo.enabledLayerCount = ctx->m_extensions.m_validationlayers.size;
    instanceCreateInfo.ppEnabledExtensionNames = (const char*const*)ctx->m_extensions.m_extensions.data;
    instanceCreateInfo.enabledExtensionCount = ctx->m_extensions.m_extensions.size;
    
    vkerr = vkCreateInstance(&instanceCreateInfo, NULL, &ctx->m_instance);
    if (vkerr != VK_SUCCESS) 
    { log_error("failed to create Vulkan-instance."); return -1; }
    return 0;
}

GroubiksResult_t _setupDebugMessenger(VulkanContext_t* ctx) {
    /* initialize debugmessenger */
    GroubiksResult_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;

    memzero(messengerCreateInfo);
    messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerCreateInfo.messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messengerCreateInfo.messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messengerCreateInfo.pfnUserCallback = &Vulkan_GroubiksDebugCallback;

    PFN_vkCreateDebugUtilsMessengerEXT func = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ctx->m_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == NULL)
    { log_error("Vulkan-DebugUtils-extension not loaded."); return -1; }

    vkerr = func(ctx->m_instance, &messengerCreateInfo, NULL, &ctx->m_debug_messenger);
    if (vkerr != VK_SUCCESS)
    { log_error("failed to initialize Vulkan-DebugMessenger."); return -1; }
    return 0;
}

GroubiksResult_t _setupDevices(VulkanContext_t* ctx) {
    GroubiksResult_t err = 0;
    /* query devices and setup a picked device */
    err = CreateVulkanDevices(&ctx->m_devices, &ctx->m_instance);
    if (err != 0)
    { log_error("failed to initialize VulkanDeviceManager."); return -1; }
    err = _setupAvailableDevices(&ctx->m_devices, &ctx->m_instance);
    if (err != 0)
    { log_error("failed to query for physical devices."); return -1; }
    err = _setupActiveDevices(&ctx->m_devices, &ctx->m_instance);
    if (err != 0)
    { log_error("failed to pick a physical device."); return -1; }
    return 0;
}

void DestroyVulkanContext(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    DestroyVulkanDevices(&ctx->m_devices);
    _destroyDebugMessenger(ctx);
    DestroyVulkanExtensions(&ctx->m_extensions);
    vkDestroyInstance(ctx->m_instance, NULL);
}

void _destroyDebugMessenger(VulkanContext_t* ctx) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
    (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(ctx->m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    { func(ctx->m_instance, ctx->m_debug_messenger, NULL); }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_GroubiksDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    switch (messageSeverity)
    {
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT):
        log_info(pCallbackData->pMessage);
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT):
        log_warning(pCallbackData->pMessage);
    case (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT):
        log_error(pCallbackData->pMessage);
    default:
        log(INFO_LOG, pCallbackData->pMessage);
    }
    return VK_FALSE;
}