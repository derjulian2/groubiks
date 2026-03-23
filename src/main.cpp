
#include <Application.hpp>
#include <Logging.hpp>
#include <format>

int main(int argc, char** argv) {

    try {
        ng::Application app(
            NEOGROUBIKS_VERTEX_SHADER_PATH, 
            NEOGROUBIKS_FRAGMENT_SHADER_PATH
        );
        app.execute();
    }
    catch (const std::exception& e) {
        ng::log_error(std::format("neogroubiks encountered an error: {}", e.what()));
    }

    return 0;
}