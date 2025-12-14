
#include <groubiks/graphics/graphics.h>
#include <vulkan/vulkan_core.h>

define_vector(VkPhysicalDevice);

VkResult CreateVulkanContext(VulkanContext_t* ctx, const char* appName, const char** vlayers, const int vlayers_num) {
    assert(ctx != NULL);
    VkResult err = VK_SUCCESS;
    /* set appinfo parameters */
    memset(&ctx->m_appinfo, 0, sizeof(VkApplicationInfo));
    ctx->m_appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ctx->m_appinfo.pApplicationName = appName;
    ctx->m_appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ctx->m_appinfo.pEngineName = "No Engine";
    ctx->m_appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    ctx->m_appinfo.apiVersion = VK_API_VERSION_1_0;
    /* set createinfo parameters */
    VkInstanceCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkInstanceCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &ctx->m_appinfo;
    /* set glfw extension-parameters */
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    create_info.enabledExtensionCount = glfwExtensionCount;
    create_info.ppEnabledExtensionNames = glfwExtensions;
    /* set validationlayers */
#ifndef NDEBUG
    if ((err = VulkanContext_VerifyValidationLayers(ctx, vlayers, vlayers_num)) != VK_SUCCESS) 
    { return err; }
    create_info.ppEnabledLayerNames = vlayers;
    create_info.enabledLayerCount = vlayers_num;
#else
    create_info.enabledLayerCount = 0;
#endif
    /* initialize devicelist */
    ctx->m_devices = vector_default(VkPhysicalDevice);
    /* create the instance */
    if ((err = vkCreateInstance(&create_info, NULL, &ctx->m_instance)) != VK_SUCCESS) {
        return err;
    }
    return err;
}

void DestroyVulkanContext(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    if (ctx->m_devices.data != NULL) {
        vector_destroy(VkPhysicalDevice, &ctx->m_devices);
    }
    vkDestroyInstance(ctx->m_instance, NULL);
}

int CreateGLFWContext(GLFWContext_t* ctx) {

}

void DestoryGLFWContext(GLFWContext_t* ctx) {

}

VkResult VulkanContext_GetDevices(VulkanContext_t* ctx) {
    assert(ctx != NULL);
    assert(ctx->m_devices.capacity == 0);
    VkResult err = VK_SUCCESS;
    uint32_t device_count;
    if ((err = vkEnumeratePhysicalDevices(ctx->m_instance, &device_count, NULL)) != VK_SUCCESS) {
        return err;
    }
    if (vector_construct(VkPhysicalDevice, &ctx->m_devices, device_count) != 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if ((err = vkEnumeratePhysicalDevices(ctx->m_instance, &device_count, ctx->m_devices.data)) != VK_SUCCESS) {
        return err;
    }
    /* manual set of size bc data is pushed in from vulkan */
    ctx->m_devices.size = device_count; 
    return VK_SUCCESS;
}

VkResult VulkanContext_VerifyValidationLayers(VulkanContext_t* ctx, const char** vlayers, const int num) {
    assert(ctx);
    ctx->m_debugcontext.m_validationlayers = vlayers;
    ctx->m_debugcontext.m_validationlayer_count = num;
    VkResult err = VK_SUCCESS;
    /* retrieve available validationlayers */
    uint32_t layerCount;
    if ((err = vkEnumerateInstanceLayerProperties(&layerCount, NULL)) != VK_SUCCESS) {
        goto cleanup;
    }
    VkLayerProperties* layerProps = malloc(sizeof(VkLayerProperties) * layerCount);
    if (layerProps == NULL) {
        err = VK_ERROR_INITIALIZATION_FAILED;
        goto cleanup;
    }
    if ((err = vkEnumerateInstanceLayerProperties(&layerCount, layerProps)) != VK_SUCCESS) {
        goto cleanup;
    }
    /* compare with set validationlayers */
    for (int i = 0; i < num; ++i) {
        int layerFound = 0;
        for (int j = 0; j < layerCount; ++j) {
            if (strcmp(vlayers[i], layerProps[j].layerName) == 0) {
                layerFound = 1;
                break;
            }
        }
        if (!layerFound) {
            err = -2;
            goto cleanup;
        }
    }
cleanup:
    free(layerProps);
    return err;
}