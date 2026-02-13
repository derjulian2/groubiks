
#include <groubiks/groubiks.hpp>

void groubiks::application::initialize() {
    vk_extras extras        = vk_extras_null;
    vk_extras device_extras = vk_extras_null;
    u32 glfwExtCount = 0;
    const char** ppGlfwExtensions = nullptr;
    
    /* convenience conversion-lambda */
    auto to_cstr_vec = [](std::vector<std::string>& v) {
        std::vector<char*> res;
        res.reserve(v.size());
        for (std::string& s : v) {
            res.push_back(s.data());
        }
        return res;
    };

    std::vector<std::string> instValidationLayers;
    std::vector<std::string> instExtensions;
    // std::vector<std::string> deviceValidationLayers;
    std::vector<std::string> deviceExtensions;

    this->renderer = new vk_renderer();

    if (log_init() != 0) 
    { throw std::runtime_error("failed to initialize logging"); }
    
    if (glfwInit() != GLFW_TRUE)
    { throw std::runtime_error("failed to initialize GLFW"); }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(DEFAULT_WIN_WIDTH, 
        DEFAULT_WIN_HEIGHT, 
        DEFAULT_APPLICATION_NAME, 
        NULL, NULL
    );
    if (this->window == NULL) 
    { throw std::runtime_error("failed to initialize GLFW-window"); }

    ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    instValidationLayers.reserve(VK_NUM_VALIDATIONLAYERS);
    instExtensions.reserve(VK_NUM_EXTENSIONS + glfwExtCount);
    for (const char* layer : VK_VALIDATIONLAYERS) {
        instValidationLayers.push_back(layer);
    }
    for (const char* ext : VK_EXTENSIONS) {
        instExtensions.push_back(ext);
    }
    for (u32 i = 0; i < glfwExtCount; ++i) {
        instExtensions.push_back(ppGlfwExtensions[i]);
    }

    deviceExtensions.reserve(VK_NUM_DEVICE_EXTENSIONS);
    for (const char* ext : VK_DEVICE_EXTENSIONS) {
        deviceExtensions.push_back(ext);
    }

    std::vector<char*> v1 = to_cstr_vec(instValidationLayers);
    std::vector<char*> v2 = to_cstr_vec(instExtensions);
    std::vector<char*> v3 = to_cstr_vec(deviceExtensions);

    extras.m_validationlayers = assign_dynarray(str, 
        v1.data(),
        instValidationLayers.size()
    );
    extras.m_extensions = assign_dynarray(str, 
        v2.data(), 
        instExtensions.size()
    );
    device_extras.m_extensions = assign_dynarray(str, 
        v3.data(), 
        deviceExtensions.size()
    );
    if (vk_renderer_create(this->renderer, 
        this->window, 
        vk_physical_device_flags_is_discrete_gpu,
        &extras, &device_extras
    ) != GROUBIKS_SUCCESS) 
    { throw std::runtime_error("failed to initialize vulkan-renderer"); }
}

void groubiks::application::execute() {
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();
        if (vk_renderer_draw_triangles(this->renderer) != GROUBIKS_SUCCESS)
        { throw std::runtime_error("renderer failed to draw"); }
    }
}

void groubiks::application::cleanup() {
    free_vk_renderer(this->renderer);
    delete this->renderer;
    glfwDestroyWindow(this->window);
    glfwTerminate();
    log_end();
}
