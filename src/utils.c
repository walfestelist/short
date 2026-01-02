#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "debug.h"

void* safe_recalloc(void *ptr, size_t old_size, size_t new_size) {
    void *new_ptr = safe_realloc(ptr, new_size);

    if (new_size > old_size) memset((char*)new_ptr + old_size, 0, new_size - old_size);

    return new_ptr;
}

void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) printf_error("Not enough memory to allocate %zu bytes", size);
    return ptr;
}

void* safe_calloc(size_t size) {
    void *ptr = calloc(size, 1);
    if (!ptr) printf_error("Not enough memory to allocate %zu bytes", size);
    return ptr;
}

void* safe_realloc(void *ptr, size_t size) {
    if (!ptr) return NULL;

    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) printf_error("Not enough memory to allocate %zu bytes", size);

    return new_ptr;
}

uint64_t uint8_8_to_uint64(uint8_t bytes[8]) {
    uint64_t value = 0;

    for (int i = 0; i < 8; i++) {
        value = (value << 8) | bytes[i];
    }

    return value;
}

uint32_t uint8_4_to_uint32(uint8_t bytes[4]) {
    uint32_t value = 0;

    for (int i = 0; i < 4; i++) {
        value = (value << 8) | bytes[i];
    }

    return value;
}

uint32_t uint8_3_to_uint24(uint8_t bytes[3]) {
    uint32_t value = 0;

    for (int i = 0; i < 3; i++) {
        value = (value << 8) | bytes[i];
    }

    return value;
}

uint16_t uint8_2_to_uint16(uint8_t bytes[2]) {
    uint16_t value = 0;

    for (int i = 0; i < 2; i++) {
        value = (value << 8) | bytes[i];
    }

    return value;
}

void uint64_to_uint8_8(uint8_t bytes[8], uint64_t num) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)(num >> (8 * (7 - i)));
    }
}

int isdigit_in_hex(char c) {
    return
        (c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f');
}

int isdigit_in_bin(char c) {
    return c == '0' || c == '1';
}

int isdigit_in_oct(char c) {
    return c >= '0' && c <= '7';
}

int utf8_char_len(uint8_t first_byte) {
    if ((first_byte & 0x80) == 0x00) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 0;
}
