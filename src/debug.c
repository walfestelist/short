#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

extern char *src;
extern uint8_t *memory;

void printf_error(const char *format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    fprintf(stderr, RED_BOLD "[ERROR]" RESET_COL " ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    if (src) free(src);
    if (memory) free(memory);
    exit(EXIT_FAILURE);
}
