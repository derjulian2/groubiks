
#ifndef GROUBIKS_HPP
#define GROUBIKS_HPP

extern "C" {
    #include <groubiks/renderer/vulkan_render_context.h>
}

#undef cleanup
#undef raise
#undef check
#undef except

#include <groubiks/cube.hpp>
#include <groubiks/gui.hpp>

namespace groubiks {

    using result_type = int;

    class application {

    public:

        gui ui;
        cube main_cube;

        GLFWwindow*       window = NULL;
        vk_context        vulkan_context = vk_context_null;
        vk_device_context device_context = vk_device_context_null;
        vk_render_context render_context = vk_render_context_null;

        groubiks_result_t initialize();
        void execute();
        void cleanup();
    };

}

#endif