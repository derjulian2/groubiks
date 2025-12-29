
#ifndef GROUBIKS_HPP
#define GROUBIKS_HPP

extern "C" {
    #include <groubiks/renderer/vulkan_context.h>
}

#include <groubiks/cube.hpp>
#include <groubiks/gui.hpp>

namespace groubiks {

    using result_type = int;

    class application {

    public:

        // rendercontext renderer;
        gui ui;
        cube main_cube;

        result_type initialize();
        result_type execute();
        void cleanup();
    };

}

#endif