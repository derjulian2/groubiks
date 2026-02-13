
#include <iostream>
#include <groubiks/groubiks.hpp>

int main(int argc, char** argv) {
    try {
        groubiks::application app;
        app.execute();
    }
    catch (const std::exception& e) {
        std::cerr << "groubiks encountered an error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}