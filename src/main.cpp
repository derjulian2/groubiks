
#include <iostream>
#include <groubiks/groubiks.hpp>

int main(int argc, char** argv) {
    groubiks::application app;

    if (app.initialize() != GROUBIKS_SUCCESS) {
        std::cerr << "groubiks failed to initialize" << std::endl;
        return -1;
    }
    app.execute();
    app.cleanup();

    return 0;
}