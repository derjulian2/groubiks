
#ifndef GROUBIKS_HPP
#define GROUBIKS_HPP

extern "C" {
    #include <groubiks/renderer/vk_renderer.h>
}
/* undefs- for c++ compatibility */
#undef make_optional
#undef optional_t
#undef nullopt
#undef throw
#undef except
#undef log
#undef logf
#undef clamp

#include <groubiks/cube.hpp>
#include <groubiks/gui.hpp>
#include <vector>
#include <stdexcept>

namespace groubiks {

    using result_type = groubiks_result_t;

    class application {
        GLFWwindow* window    = nullptr;
        vk_renderer* renderer = nullptr;
    public:
        gui ui;
        cube main_cube;

        application() { initialize(); }
        application(const application&) = delete;
        application(const application&&) = delete;
        ~application() { cleanup(); }

        void initialize();
        void execute();
        void cleanup();
    };

}

#endif