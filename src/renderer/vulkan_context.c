
#include <groubiks/renderer/vulkan_context.h>

define_dynarray(VkPhysicalDevice, VkPhysicalDevice, 
    (comp, NULL)
);


groubiks_result_t 
vk_context_create(struct vk_context* pVulkanContext, 
    struct vk_extras* pExtras,
    GLFWwindow* pWindow)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;

    *pVulkanContext = vk_context_null;
    pVulkanContext->m_window = pWindow;

    err = vk_context_setup_instance(pVulkanContext, pExtras);
    check(err == GROUBIKS_SUCCESS);

    err = vk_context_setup_windowsurface(pVulkanContext);
    check(err == GROUBIKS_SUCCESS);

    err = vk_context_setup_devices(pVulkanContext);
    check(err == GROUBIKS_SUCCESS);

    err = vk_context_setup_debug_messenger(pVulkanContext);
    check(err == GROUBIKS_SUCCESS);

    log_info(VK_CONTEXT_CREATE_SUCCESS_STR);
    return err;
    except(err,
        free_vk_context(pVulkanContext);
        log_error(VK_CONTEXT_CREATE_FAIL_STR);
    )
}


void 
free_vk_context(struct vk_context* pVulkanContext)
{
    if (pVulkanContext->m_debug_messenger != VK_NULL_HANDLE) {
        PFN_vkDestroyDebugUtilsMessengerEXT destroyMessengerFunc = 
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pVulkanContext->m_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroyMessengerFunc == NULL || pVulkanContext->m_instance == VK_NULL_HANDLE) {
            log_error(VK_CONTEXT_DEBUG_MESSENGER_CLEANUP_FAIL_STR);
        }
        else {
            destroyMessengerFunc(pVulkanContext->m_instance, pVulkanContext->m_debug_messenger, NULL);
        }
    }
    if (pVulkanContext->m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(pVulkanContext->m_instance, NULL);
    }
}


groubiks_result_t
vk_context_setup_windowsurface(struct vk_context *pVulkanContext)
{
    VkResult vkErr = VK_SUCCESS;

    vkErr = glfwCreateWindowSurface(pVulkanContext->m_instance, 
        pVulkanContext->m_window, 
        NULL,
        &pVulkanContext->m_surface 
    );
    if (vkErr == VK_SUCCESS) {
        log_info("setup windowsurface");
        return GROUBIKS_SUCCESS;
    }
    else {
        log_error("failed to setup windowsurface");
        return GROUBIKS_VULKAN_ERROR;
    }
}


groubiks_result_t 
vk_context_setup_instance(struct vk_context* pVulkanContext,
    struct vk_extras* pExtras)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr = VK_SUCCESS;

    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;

    err = vk_extras_match_instance(pExtras);
    check(err == GROUBIKS_SUCCESS);

    vk_fill_struct_instance_appinfo(&appInfo);
    vk_fill_struct_instance_createinfo(&createInfo, 
        &appInfo, 
        (const char*const*)pExtras->m_validationlayers.data, pExtras->m_validationlayers.size, 
        (const char*const*)pExtras->m_extensions.data, pExtras->m_extensions.size
    );
    
    vkErr = vkCreateInstance(&createInfo, NULL, &pVulkanContext->m_instance);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_CONTEXT_INSTANCE_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_INSTANCE_SETUP_FAIL_STR);
    )
}


groubiks_result_t 
vk_context_setup_debug_messenger(struct vk_context* pVulkanContext)
{
    groubiks_result_t err = GROUBIKS_SUCCESS;
    VkResult vkErr        = VK_SUCCESS; 

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    
    vk_fill_struct_debugmessenger_createinfo(&createInfo, &vk_groubiks_debug_callback);

    PFN_vkCreateDebugUtilsMessengerEXT createMessengerFunc = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(pVulkanContext->m_instance, "vkCreateDebugUtilsMessengerEXT");
    check(createMessengerFunc != NULL, err = GROUBIKS_VULKAN_ERROR);
     
    vkErr = createMessengerFunc(pVulkanContext->m_instance, &createInfo, NULL, &pVulkanContext->m_debug_messenger);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    log_info(VK_CONTEXT_DEBUG_MESSENGER_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_DEBUG_MESSENGER_SETUP_FAIL_STR)
    )
}


groubiks_result_t 
vk_context_setup_devices(struct vk_context* pVulkanContext)
{
    groubiks_result_t err         = GROUBIKS_SUCCESS; 
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;
    VkResult vkErr                = VK_SUCCESS;
    u32 deviceCount = 0;

    vkErr = vkEnumeratePhysicalDevices(pVulkanContext->m_instance, &deviceCount, NULL);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);
    dynarray_resize(VkPhysicalDevice, &pVulkanContext->m_physical_devices, deviceCount, &dynarrayErr);
    check(dynarrayErr == DYNARRAY_SUCCESS, err = GROUBIKS_BAD_ALLOC);
    vkErr = vkEnumeratePhysicalDevices(pVulkanContext->m_instance, &deviceCount, pVulkanContext->m_physical_devices.data);
    check(vkErr == VK_SUCCESS, err = GROUBIKS_VULKAN_ERROR);

    VkPhysicalDeviceProperties props;
    memzero(props);
    dynarray_for_each(VkPhysicalDevice, &pVulkanContext->m_physical_devices, device) {
        vkGetPhysicalDeviceProperties(*device, &props);
        logf_info("found device: %s", props.deviceName);
    }

    log_info(VK_CONTEXT_DEVICE_SETUP_SUCCESS_STR);
    return err;
    except(err,
        log_error(VK_CONTEXT_DEVICE_SETUP_FAIL_STR);
    )
}


