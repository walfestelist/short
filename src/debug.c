#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"
#include "vm.h"

extern char *src;

void printf_error(const char *format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    fprintf(stderr, RED_BOLD "[ERROR]" RESET_COL " ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    if (src) free(src);
    exit(EXIT_FAILURE);
}

void printf_error_vm(ShortVM *vm, const char *format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    fprintf(stderr, RED_BOLD "[ERROR]" RESET_COL " ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    if (src) free(src);

    if (vm) {
        if (vm->mem) free(vm->mem);
        if (vm->label_mem) free(vm->label_mem);
    }
    exit(EXIT_FAILURE);
}
