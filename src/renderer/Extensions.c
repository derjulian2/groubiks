
#include <groubiks/renderer/Extensions.h>

VulkanExtensions CreateVulkanExtensions(const char** validationLayers,
        uint32_t numValidationLayers,
        const char** extensionNames,
        uint32_t numExtensionsNames) {
    VulkanExtensions ext = malloc(sizeof(VulkanExtensions_t));
    if (ext == NULL)
    { return NULL; }
    GroubiksResult_t err = 0;
    int i = 0;

    /* initialize validationlayers */
    ext->m_validationlayers = make_vector(cstring_t, NULL, numValidationLayers, &err);
    if (err != 0) 
    { goto error; }
    vector_zero(cstring_t, &ext->m_validationlayers);
    vector_for_each(cstring_t, &ext->m_validationlayers, layer)
    {
        *layer = strdup(validationLayers[i++]);
        if (*layer == NULL) 
        { goto error; }
    }
    /* initialize extensions */
    ext->m_extensions = make_vector(cstring_t, NULL, numExtensionsNames, &err);
    if (err != 0)
    { goto error; }
    i = 0;
    vector_zero(cstring_t, &ext->m_extensions);
    vector_for_each(cstring_t, &ext->m_extensions, extension)
    {
        *extension = strdup(extensionNames[i++]);
        if (*extension == NULL) 
        { goto error; }
    }
    log_info("successfully setup vulkan-extensions");
    return ext;
error:
    log_error("failed to setup vulkan-extensions");
    DestroyVulkanExtensions(ext);
    return NULL;
}

GroubiksResult_t _setupGLFWExtensions(VulkanExtensions_t* ext) {
    GroubiksResult_t err = 0;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (glfwExtensions == NULL) 
    { return -1; }
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        cstring_t* new_str = vector_push_back(cstring_t, &ext->m_extensions, strdup(glfwExtensions[i]), &err);
        if (err != 0)
        { return -1; }
    }
    return 0;
}

void DestroyVulkanExtensions(VulkanExtensions ext) {
    if (ext == NULL)
    { return; }

    vector_for_each(cstring_t, &ext->m_validationlayers, layer)
    { free(*layer); }
    free_vector(&ext->m_validationlayers);
    
    vector_for_each(cstring_t, &ext->m_extensions, extension)
    { free(*extension); }
    free_vector(&ext->m_extensions);
}

GroubiksResult_t VulkanExtensions_VerifyValidationLayers(VulkanExtensions_t* ext) {
    assert(ext != NULL);
    GroubiksResult_t err = 0;
    VkResult vkerr = VK_SUCCESS;
    uint32_t layerCount = 0;
    VkLayerProperties* layerProps = NULL;

    /* retrieve available validationlayers */
    vkerr = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if (vkerr != VK_SUCCESS) 
    { return -1; }
    layerProps = malloc(sizeof(VkLayerProperties) * layerCount);
    if (layerProps == NULL)
    { err = -1; goto error;  }
    vkerr = vkEnumerateInstanceLayerProperties(&layerCount, layerProps);
    if (vkerr != VK_SUCCESS )
    { err = -1; goto error; }
    /* compare with set validationlayers */
    for (int i = 0; i < ext->m_validationlayers.size; ++i) {
        int layerFound = 0;
        for (int j = 0; j < layerCount; ++j) {
            if (strcmp(*vector_at(cstring_t, &ext->m_validationlayers, i), layerProps[j].layerName) == 0) {
                layerFound = 1;
                break;
            }
        }
        if (!layerFound) 
        { err = -1; goto error; }
    }
error:
    free(layerProps);
    return err;
    
}

GroubiksResult_t VulkanExtensions_VerifyExtensions(VulkanExtensions_t* ext) {
    assert(ext != NULL);
    GroubiksResult_t err = 0;
    
    /* optional: https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance#page_Checking-for-extension-support */

    return err;
}