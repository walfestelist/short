#ifndef DEBUG_H

#define DEBUG_H

#include "vm.h"

#define RED_BOLD "\x1b[22;31m"
#define RESET_COL "\x1b[0m"

void printf_error(const char *format, ...);
void printf_error_vm(ShortVM *vm, const char *format, ...);

#endif
