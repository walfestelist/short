#ifndef UTILS_H

#define UTILS_H

#include <stddef.h>
#include <stdint.h>

void* safe_recalloc(void *ptr, size_t old_size, size_t new_size);

void* safe_malloc(size_t size);
void* safe_calloc(size_t size);
void* safe_realloc(void *ptr, size_t size);

uint64_t uint8_8_to_uint64(uint8_t bytes[8]);
uint32_t uint8_4_to_uint32(uint8_t bytes[4]);
uint32_t uint8_3_to_uint24(uint8_t bytes[3]);
uint16_t uint8_2_to_uint16(uint8_t bytes[2]);

void uint64_to_uint8_8(uint8_t bytes[8], uint64_t num);

int isdigit_in_hex(char c);
int isdigit_in_bin(char c);
int isdigit_in_oct(char c);

int utf8_char_len(uint8_t first_byte);

#endif
