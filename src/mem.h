#ifndef MEM_H

#define MEM_H

#include <stddef.h>
#include <stdint.h>

#include "vm.h"

#define BLOCK_SIZE 512

void init_mem(ShortVM *vm);
uint8_t getbyte_mem(size_t n, ShortVM *vm);
void setbyte_mem(size_t n, uint8_t value, ShortVM *vm);
uint64_t getvar_mem(size_t n, ShortVM *vm);
void setvar_mem(size_t n, uint64_t value, ShortVM *vm);

uint64_t get_label_addr(uint64_t num, ShortVM *vm);
void set_label_addr(uint64_t num, uint64_t value, ShortVM *vm);

void addbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void subbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void mulbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void divbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void rembyte_mem(size_t n, uint8_t value, ShortVM *vm);

void addvar_mem(size_t n, uint64_t value, ShortVM *vm);
void subvar_mem(size_t n, uint64_t value, ShortVM *vm);
void mulvar_mem(size_t n, uint64_t value, ShortVM *vm);
void divvar_mem(size_t n, uint64_t value, ShortVM *vm);
void remvar_mem(size_t n, uint64_t value, ShortVM *vm);

void orbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void xorbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void andbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void rightbyte_mem(size_t n, uint8_t value, ShortVM *vm);
void leftbyte_mem(size_t n, uint8_t value, ShortVM *vm);

void orvar_mem(size_t n, uint64_t value, ShortVM *vm);
void xorvar_mem(size_t n, uint64_t value, ShortVM *vm);
void andvar_mem(size_t n, uint64_t value, ShortVM *vm);
void rightvar_mem(size_t n, uint64_t value, ShortVM *vm);
void leftvar_mem(size_t n, uint64_t value, ShortVM *vm);

void scanbytes_mem(size_t n, ShortVM *vm);
void scanvars_mem(size_t n, ShortVM *vm);
void scanlabels(size_t n, ShortVM *vm);

#endif
