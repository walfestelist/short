#include <stdio.h>
#include <stdint.h>

#include "debug.h"
#include "utils.h"
#include "mem.h"

uint8_t *memory;
size_t mem_alloced = 0;

void init_mem() {
    memory = safe_calloc(BLOCK_SIZE);
    mem_alloced = BLOCK_SIZE;
    // printf("Memory initialized with block size %d\n", BLOCK_SIZE);
}

uint8_t getbyte_mem(size_t n) {
    if (n >= mem_alloced) return 0;
    return memory[n];
}

void setbyte_mem(size_t n, uint8_t value) {
    if (n >= mem_alloced) memory = safe_recalloc(memory, mem_alloced, ((n + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE);

    memory[n] = value;
}

uint64_t getvar_mem(size_t n) {
    size_t byte_n = n * 8;

    if (byte_n + 8 >= mem_alloced) return 0;

    uint64_t value = uint8_8_to_uint64(memory + byte_n);
    return value;
}

void setvar_mem(size_t n, uint64_t value) {
    size_t byte_n = n * 8;

    if (byte_n >= mem_alloced) memory = safe_recalloc(memory, mem_alloced, ((byte_n + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE);

    uint64_to_uint8_8(memory + byte_n, value);
}

void addbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) + value); }
void subbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) - value); }
void mulbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) * value); }
void divbyte_mem(size_t n, uint8_t value) {
    if (value == 0) {
        printf_error("Division by zero");
    }
    setbyte_mem(n, getbyte_mem(n) / value);
}

void rembyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) % value); }

void addvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) + value); }
void subvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) - value); }
void mulvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) * value); }
void divvar_mem(size_t n, uint64_t value) {
    if (value == 0) {
        printf_error("Division by zero");
    }
    setvar_mem(n, getvar_mem(n) / value);
}

void remvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) % value); }

void orbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) | value); }
void xorbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) ^ value); }
void andbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) & value); }
void rightbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) >> value); }
void leftbyte_mem(size_t n, uint8_t value) { setbyte_mem(n, getbyte_mem(n) << value); }

void orvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) | value); }
void xorvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) ^ value); }
void andvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) & value); }
void rightvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) >> value); }
void leftvar_mem(size_t n, uint64_t value) { setvar_mem(n, getvar_mem(n) << value); }

void scanbytes_mem(size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("[b%zu]: 0x%02X\n", i, getbyte_mem(i));
    }
}

void scanvars_mem(size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("[v%zu]: 0x%02lX\n", i, getvar_mem(i));
    }
}
