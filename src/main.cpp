
#include <Application.hpp>
#include <Logging.hpp>
#include <format>

int main(int argc, char** argv) {

    try {
        ng::Application app;
        app.initialize(
            "/home/julian/Projects/neogroubiks/src/shaders/default.vert", 
            "/home/julian/Projects/neogroubiks/src/shaders/default.frag"
        );
        app.execute();
    }
    catch (const std::exception& e) {
        ng::log_error(std::format("neogroubiks encountered an error: {}", e.what()));
    }

    return 0;
}