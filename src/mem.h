#ifndef MEM_H

#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 512

void init_mem();
uint8_t getbyte_mem(size_t n);
void setbyte_mem(size_t n, uint8_t value);
uint64_t getvar_mem(size_t n);
void setvar_mem(size_t n, uint64_t value);

uint64_t get_label_addr(uint64_t num);
void set_label_addr(uint64_t num, uint64_t value);

void addbyte_mem(size_t n, uint8_t value);
void subbyte_mem(size_t n, uint8_t value);
void mulbyte_mem(size_t n, uint8_t value);
void divbyte_mem(size_t n, uint8_t value);
void rembyte_mem(size_t n, uint8_t value);

void addvar_mem(size_t n, uint64_t value);
void subvar_mem(size_t n, uint64_t value);
void mulvar_mem(size_t n, uint64_t value);
void divvar_mem(size_t n, uint64_t value);
void remvar_mem(size_t n, uint64_t value);

void orbyte_mem(size_t n, uint8_t value);
void xorbyte_mem(size_t n, uint8_t value);
void andbyte_mem(size_t n, uint8_t value);
void rightbyte_mem(size_t n, uint8_t value);
void leftbyte_mem(size_t n, uint8_t value);

void orvar_mem(size_t n, uint64_t value);
void xorvar_mem(size_t n, uint64_t value);
void andvar_mem(size_t n, uint64_t value);
void rightvar_mem(size_t n, uint64_t value);
void leftvar_mem(size_t n, uint64_t value);

void scanbytes_mem(size_t n);
void scanvars_mem(size_t n);
void scanlabels(size_t n);

#endif
