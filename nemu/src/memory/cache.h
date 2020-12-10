#ifndef __CACHE_H__
#define __CACHE_H__

#define Cache_bit 16
#define Cache_size (1 << Cache_bit)
#define Cache_1_B_bit 6
#define Cache_1_B_size (1 << Cache_1_B_bit)
#define B_bit Cache_1_B_bit
#define B_size Cache_1_B_size
#define E_bit 3
#define E_size (1 << E_bit)
#define S_bit (Cache_bit - B_bit - E_bit)
#define S_size (1 << S_bit)
#define level 1
#include "cache_template.h"
#undef Cache_bit
#undef Cache_size
#undef B_bit
#undef B_size
#undef E_bit
#undef E_size
#undef S_bit
#undef S_size
#undef level
cache_1 Cache_1;
uint64_t memtime;

#endif