#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/mem.h"
#include "src/read.h"
#include "src/run.h"
#include "src/debug.h"
#include "src/vm.h"

const static double version = 0.02;

char *src = NULL;

int main(int argc, char **argv) {
    int version_asked = 0;
    char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) && !version_asked) {
            version_asked = 1;
            printf("Short Programming Language\n");
            printf("Version v%g\n", version);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "--version") && strcmp(argv[i], "-v")) {
            filename = argv[i];
            break;
        }
    }

    src = read_file(filename);
    if (!filename) printf_error("File was not mentioned");
    ShortVM vm = init_vm(src);

    run_code(&vm);
    // scanbytes_mem(100, &vm);

    free(src);
}
