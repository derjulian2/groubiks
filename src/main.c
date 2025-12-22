
#include <groubiks/compile_config.h>
#include <groubiks/Application.h>
#include <stdio.h>

int main(int argc, char** argv) {
    result_t err = 0;
    GroubiksApplication_t app;

    err = Groubiks_Init(&app);
    if (err != 0) {
        fputs("failed to initialize Groubiks. check log-files for further information.\n", stderr);
        return err;
    }

    err = Groubiks_Execute(&app);
    if (err != 0) {
        fputs("Groubiks encountered an error. check log-files for further information\n", stderr);
        return err;
    }

    Groubiks_Destroy(&app);
    return 0;
}