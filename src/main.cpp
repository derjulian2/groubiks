
#include <iostream>
#include <groubiks/groubiks.hpp>

int main(int argc, char** argv) {
    groubiks::application app;

    if (app.initialize() != GROUBIKS_SUCCESS) {
        std::cerr << "failed to initialize Groubiks. check log-files for further information.\n";
        return GROUBIKS_ERROR;
    }

    if (app.execute() != GROUBIKS_SUCCESS) {
        std::cerr << "Groubiks encountered a runtime-error. check log-files for further information\n";
        return GROUBIKS_ERROR;
    }

    app.cleanup();

    return GROUBIKS_SUCCESS;
}