#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/read.h"
#include "src/run.h"
#include "src/mem.h"
#include "src/debug.h"

const static double version = 0.02;

extern uint64_t pc;

char *src = NULL;

int main(int argc, char **argv) {
    init_mem();

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

    if (!filename) printf_error("File was not mentioned");

    src = read_file(filename);
    run_code(src);

    free(src);
}
