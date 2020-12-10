#include "cache.h"
#include "macro.h"
#include "time.h"
#include "stdlib.h"
#include "stdint.h"
#include "common.h"
#define Cache_bit 16
#define Cache_size (1 << Cache_bit)
#define B_bit 6
#define B_size (1 << B_bit)
#define E_bit 3
#define E_size (1 << E_bit)
#define S_bit (Cache_bit - B_bit - E_bit)
#define S_size (1 << S_bit)
#define level 1

#define cache_line concat(cache_line_, level)
#define cache_set concat(cache_set_, level)
#define cache concat(cache_, level)
#define Cache concat(Cache_, level)
#define init concat(cache, _init)
#define find concat(cache, _find)
#define replace concat(cache, _replace)

void init(cache *cur) {
    int i, j;
    for(i = 0; i < S_size; i++) {
        for(j = 0; j < E_size; j++) {
            cur->set[i].line[j].valid_bit = 0x0;
            cur->set[i].line[j].dirty_bit = 0x0;
        }
    }
    return;
}

uint8_t *find(cache *cur, hwaddr_t addr) {
    uint32_t addr_tag = (addr >> (B_bit + S_bit)) & (0xffffffffu >> (B_bit + S_bit));
    uint32_t addr_set = (addr >> B_bit) & (0xffffffffu >> (32 - S_bit));
    uint32_t addr_offset = addr & (0xffffffffu >> (32 - B_bit));
    int i;
    for(i = 0; i < E_size; i++) {
        if(cur->set[addr_set].line[i].valid_bit == 0x1 && cur->set[addr_set].line[i].tag == addr_tag) {
            return &(cur->set[addr_set].line[i].block[addr_offset]);
        }
    }
    return NULL;
}

uint8_t *replace(cache *cur, hwaddr_t addr) {
    uint32_t addr_tag = (addr >> (B_bit + S_bit)) & (0xffffffffu >> (B_bit + S_bit));
    uint32_t addr_set = (addr >> B_bit) & (0xffffffffu >> (32 - S_bit));
    int i;
    for(i = 0; i < E_size; i++) {
        if(cur->set[addr_set].line[i].valid_bit == 0x0) {
            cur->set[addr_set].line[i].valid_bit = 1;
            cur->set[addr_set].line[i].tag = addr_tag;
            return cur->set[addr_set].line[i].block;
        }
    }
    srand((int)time(NULL));
    i = (uint32_t)rand() % E_size;
    cur->set[addr_set].line[i].tag = addr_tag;
    return cur->set[addr_set].line[i].block;
}


#undef cache_line
#undef cache_set
#undef cache
#undef Cache
#undef init
#undef find
#undef replace

#undef Cache_bit
#undef Cache_size
#undef B_bit
#undef B_size
#undef E_bit
#undef E_size
#undef S_bit
#undef S_size
#undef level