#include "common.h"
#include "cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */
#define MYDEBUG 1
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
#if MYDEBUG
	if(cache_1_find(&Cache_1, addr) == NULL) {
		printf("Miss!---------------\naddr: 0x%x\n", addr);
		uint8_t *new_ptr = cache_1_replace(&Cache_1, addr);
		hwaddr_t begin_addr = addr & (~((0xffffffffu) >> (32 - Cache_1_B_bit)));
		hwaddr_t tmp_addr = begin_addr;
		int i;
		for(i = 0; i < Cache_1_B_size; i++, begin_addr++) {
			uint8_t tmp_data = (uint8_t)(dram_read(begin_addr, 1) & 0xff);
			new_ptr[i] = tmp_data;
		}
		printf("copy:\n");
		for(i = 0; i < Cache_1_B_size; i++, tmp_addr++) {
			printf("addr: 0x%x    dram: 0x%x    cache: 0x%x\n",tmp_addr, dram_read(tmp_addr, 1) & 0xff, new_ptr[i]);
		}
		return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	}
	uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
	uint8_t *ptr = cache_1_find(&Cache_1, addr);
	uint32_t tmp = 0;
	uint32_t data = 0;
	int i;
	for(i = 0; i < len; i++, offset++) {
		if(offset >= Cache_1_B_size) {
			ptr = cache_1_find(&Cache_1, addr + i);
			offset = 0;
		}
		tmp = (uint32_t)(ptr[i]);
		tmp = tmp << (8 * i);
		data += tmp;
	}
	printf("Hit!----------------\naddr: 0x%x\ndata: %d\norigin: %d\n",addr , data, dram_read(addr, len) & (~0u >> ((4 - len) << 3)));
	return data;
#else
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
#endif
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
#if MYDEBUG
	dram_write(addr, len, data);
	uint8_t *write_ptr = cache_1_find(&Cache_1, addr);
	if(write_ptr != NULL) {
		printf("Write Cache-------------------------------------------------------------Write!!!!\naddr: 0x%x\n", addr);
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
		uint32_t tmp = 0;
		uint32_t data = 0;
		int i;
		for(i = 0; i < len; i++, offset++) {
			if(offset >= Cache_1_B_size) {
				write_ptr = cache_1_find(&Cache_1, addr + i);
				offset = 0;
			}
			tmp = data & 0xff;
			data = data >> 8;
			write_ptr[i] = (uint8_t)tmp;
		}
	}
#else
	dram_write(addr, len, data);
#endif
}
#undef MYDEBUG

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

