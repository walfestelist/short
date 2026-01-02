#ifndef VM_H

#define VM_H

#include <stdint.h>

typedef struct {
    uint64_t pc;
    uint8_t *mem;
    uint64_t mem_alloc;
    uint64_t *label_mem;
    uint64_t label_mem_alloc;
    const char *code;

    char fl_equal;
    char fl_not_equal;
    char fl_bigger;
    char fl_less;
    char fl_eq_bigger;
    char fl_eq_less;
} ShortVM;

ShortVM init_vm(const char *code);

#endif
