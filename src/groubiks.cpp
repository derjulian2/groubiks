
#include <groubiks/groubiks.hpp>

groubiks_result_t groubiks::application::initialize() {
    groubiks_result_t err = GROUBIKS_SUCCESS;
    dynarray_result_t dynarrayErr = DYNARRAY_SUCCESS;

    VkPhysicalDevice physDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    vk_extras extras = vk_extras_null;
    vk_extras device_extras = vk_extras_null;

    if (log_init() != 0) 
    { return GROUBIKS_VULKAN_ERROR; }
    
    if (glfwInit() != GLFW_TRUE)
    { return GROUBIKS_GLFW_ERROR; }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(DEFAULT_WIN_WIDTH, 
        DEFAULT_WIN_HEIGHT, 
        DEFAULT_APPLICATION_NAME, 
        NULL, NULL
    );
    if (this->window == NULL) {
        return GROUBIKS_GLFW_ERROR;
    }

    extras.m_validationlayers = make_dynarray(str, 
        VK_VALIDATIONLAYERS, 
        VK_NUM_VALIDATIONLAYERS, 
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { err = GROUBIKS_BAD_ALLOC; goto cleanup; }
    extras.m_extensions = make_dynarray(str,
        VK_EXTENSIONS,
        VK_NUM_EXTENSIONS,
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { err = GROUBIKS_BAD_ALLOC; goto cleanup; }
    err = vk_extras_get_glfw(&extras);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

    device_extras.m_extensions = make_dynarray(str,
        VK_DEVICE_EXTENSIONS,
        VK_NUM_DEVICE_EXTENSIONS,
        &dynarrayErr
    );
    if (dynarrayErr != DYNARRAY_SUCCESS) { err = GROUBIKS_BAD_ALLOC; goto cleanup; }

    err = vk_context_create(&this->vulkan_context, &extras, this->window);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

    dynarray_for_each(VkPhysicalDevice, &this->vulkan_context.m_physical_devices, dvc) {
        vkGetPhysicalDeviceProperties(*dvc, &props);
        vkGetPhysicalDeviceFeatures(*dvc, &features);
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physDevice = *dvc;
        }
    }

    if (physDevice == VK_NULL_HANDLE) { err = GROUBIKS_VULKAN_ERROR; goto cleanup; }
    err = vk_device_context_create(&this->device_context, 
        physDevice,
        this->vulkan_context.m_surface,
        &device_extras
    );
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }

    err = vk_render_context_create(&this->render_context, &this->device_context, &this->vulkan_context);
    if (err != GROUBIKS_SUCCESS) { goto cleanup; }
    log_info("initialization successful");
cleanup:
    free_dynarray(str, &extras.m_validationlayers);
    free_dynarray(str, &extras.m_extensions);    
    free_dynarray(str, &device_extras.m_validationlayers);
    free_dynarray(str, &device_extras.m_extensions);
    return err;
}

void groubiks::application::execute() {
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();
        vk_render_context_draw(&this->render_context, &this->device_context);
    }
    vkDeviceWaitIdle(this->device_context.m_logical_device);
}

void groubiks::application::cleanup() {
    free_vk_render_context(&this->render_context, &this->device_context, &this->vulkan_context);
    free_vk_device_context(&this->device_context);
    free_vk_context(&this->vulkan_context);
    glfwDestroyWindow(this->window);
    glfwTerminate();
    log_end();
}
