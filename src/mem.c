#include <stdio.h>
#include <stdint.h>

#include "debug.h"
#include "utils.h"
#include "mem.h"
#include "vm.h"

void init_mem(ShortVM *vm) {
    vm->mem = safe_calloc(BLOCK_SIZE);
    vm->mem_alloc = BLOCK_SIZE;
    vm->label_mem = safe_calloc(BLOCK_SIZE);
    vm->label_mem_alloc = BLOCK_SIZE / 8;
    // printf("Memory initialized with block size %d\n", BLOCK_SIZE);
}

uint8_t getbyte_mem(size_t n, ShortVM *vm) {
    if (n >= vm->mem_alloc) return 0;
    return vm->mem[n];
}

void setbyte_mem(size_t n, uint8_t value, ShortVM *vm) {
    size_t new_size = ((n + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    if (n >= vm->mem_alloc) {
        vm->mem = safe_recalloc(vm->mem, vm->mem_alloc, new_size);
        vm->mem_alloc = new_size;
    }

    vm->mem[n] = value;
}

uint64_t getvar_mem(size_t n, ShortVM *vm) {
    size_t byte_n = n * 8;

    if (byte_n + 8 >= vm->mem_alloc) return 0;

    uint64_t value = uint8_8_to_uint64(vm->mem + byte_n);
    return value;
}

void setvar_mem(size_t n, uint64_t value, ShortVM *vm) {
    size_t byte_n = n * 8;
    size_t new_size = ((byte_n + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

    if (byte_n >= vm->mem_alloc) {
        vm->mem = safe_recalloc(vm->mem, vm->mem_alloc, new_size);
        vm->mem_alloc = new_size;
    }

    uint64_to_uint8_8(vm->mem + byte_n, value);
}

uint64_t get_label_addr(uint64_t num, ShortVM *vm) {
    if (num >= vm->label_mem_alloc) return 0;
    return vm->label_mem[num];
}

void set_label_addr(uint64_t num, uint64_t value, ShortVM *vm) {
    size_t new_size = ((num + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    if (num >= vm->label_mem_alloc) {
        vm->label_mem = safe_recalloc(vm->label_mem, vm->label_mem_alloc, new_size);
        vm->label_mem_alloc = new_size / 8;
    }

    vm->label_mem[num] = value;
}

void addbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) + value, vm); }
void subbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) - value, vm); }
void mulbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) * value, vm); }
void divbyte_mem(size_t n, uint8_t value, ShortVM *vm) {
    if (value == 0) {
        printf_error_vm(vm, "Division by zero");
    }
    setbyte_mem(n, getbyte_mem(n, vm) / value, vm);
}

void rembyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) % value, vm); }

void addvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) + value, vm); }
void subvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) - value, vm); }
void mulvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) * value, vm); }
void divvar_mem(size_t n, uint64_t value, ShortVM *vm) {
    if (value == 0) {
        printf_error_vm(vm, "Division by zero");
    }
    setvar_mem(n, getvar_mem(n, vm) / value, vm);
}

void remvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) % value, vm); }

void orbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) | value, vm); }
void xorbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) ^ value, vm); }
void andbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) & value, vm); }
void rightbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) >> value, vm); }
void leftbyte_mem(size_t n, uint8_t value, ShortVM *vm) { setbyte_mem(n, getbyte_mem(n, vm) << value, vm); }

void orvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) | value, vm); }
void xorvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) ^ value, vm); }
void andvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) & value, vm); }
void rightvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) >> value, vm); }
void leftvar_mem(size_t n, uint64_t value, ShortVM *vm) { setvar_mem(n, getvar_mem(n, vm) << value, vm); }

void scanbytes_mem(size_t n, ShortVM *vm) {
    for (size_t i = 0; i < n; i++) {
        printf("[b%zu]: 0x%02X\n", i, getbyte_mem(i, vm));
    }
}

void scanvars_mem(size_t n, ShortVM *vm) {
    for (size_t i = 0; i < n; i++) {
        printf("[v%zu]: 0x%02lX\n", i, getvar_mem(i, vm));
    }
}

void scanlabels(size_t n, ShortVM *vm) {
    for (size_t i = 0; i < n; i++) {
        printf("[l%zu]: 0x%02lX\n", i, get_label_addr(i, vm));
    }
}
