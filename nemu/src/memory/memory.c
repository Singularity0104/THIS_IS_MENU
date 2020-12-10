#include "common.h"
#include "cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */
#define MYCODE 1
#define DEBUGIN 0
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	memread++;
#if MYCODE
	if(cache_1_find(&Cache_1, addr) == NULL) {
		cache_1_miss++;
		if(cache_2_find(&Cache_2, addr) == NULL) {
			cache_2_miss++;
			memtime += 200u;
			uint8_t *new_ptr = cache_1_replace(&Cache_1, addr);
			hwaddr_t begin_addr = addr & (~((0xffffffffu) >> (32 - Cache_1_B_bit)));	
			int i;
			for(i = 0; i < Cache_1_B_size; i++, begin_addr++) {
				uint8_t tmp_data = (uint8_t)(dram_read(begin_addr, 1) & 0xff);
				new_ptr[i] = tmp_data;
			}
			new_ptr = cache_2_replace(&Cache_2, addr);
			begin_addr = addr & (~((0xffffffffu) >> (32 - Cache_2_B_bit)));	
			for(i = 0; i < Cache_2_B_size; i++, begin_addr++) {
				uint8_t tmp_data = (uint8_t)(dram_read(begin_addr, 1) & 0xff);
				new_ptr[i] = tmp_data;
			}
			return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		}
		cache_2_hit++;
		memtime += 20u;
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_2_B_bit));
		uint8_t *ptr = cache_2_find(&Cache_2, addr);
		uint32_t tmp = 0;
		uint32_t data = 0;
		int i;
		for(i = 0; i < len; i++, offset++) {
			if(offset >= Cache_2_B_size) {		
				tmp = hwaddr_read(addr + i, len - i);
				tmp = tmp << (8 * i);
				data += tmp;
				break;
			}
			tmp = (uint32_t)(ptr[i]);
			tmp = tmp << (8 * i);
			data += tmp;
		}
		Assert(Cache_1_B_bit == Cache_2_B_bit, "block size not equal!\n");
		uint8_t *new_ptr = cache_1_replace(&Cache_1, addr);
		offset = addr & (0xffffffffu >> (32 - Cache_2_B_bit));
		ptr -= offset;
		for(i = 0; i < Cache_1_B_size; i++) {
			
			new_ptr[i] = ptr[i];
		}
		return data;
	}
	cache_1_hit++;
	memtime += 2u;
	uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
	uint8_t *ptr = cache_1_find(&Cache_1, addr);
	uint32_t tmp = 0;
	uint32_t data = 0;
	int i;
	for(i = 0; i < len; i++, offset++) {
		if(offset >= Cache_1_B_size) {		
			tmp = hwaddr_read(addr + i, len - i);
			tmp = tmp << (8 * i);
			data += tmp;
			break;
		}
		tmp = (uint32_t)(ptr[i]);
		tmp = tmp << (8 * i);
		data += tmp;
	}
	return data;
#else
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
#endif
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
#if MYCODE
	dram_write(addr, len, data);
	uint8_t *write_ptr = cache_1_find(&Cache_1, addr);
	if(write_ptr != NULL) {
#if DEBUGIN
		printf("Write Cache-------------------------------------------------------------Write!!!!\naddr: 0x%x\ndata: 0x%x\n", addr, data);
#endif
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
		uint32_t tmp = 0;
		uint32_t tmp_data = data;
		int i;
		for(i = 0; i < len; i++, offset++) {
			if(offset >= Cache_1_B_size) {
				write_ptr = cache_1_find(&Cache_1, addr + i);
				offset = 0;
			}
			tmp = tmp_data & 0xff;
			tmp_data = tmp_data >> 8;
			write_ptr[i] = (uint8_t)tmp;
		}
#if DEBUGIN
		printf("Check---cache: 0x%x    origin: 0x%x\n", hwaddr_read(addr, len), data);
#endif
	}
#else
	dram_write(addr, len, data);
#endif
}
#undef MYCODE
#undef DEBUGIN

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

