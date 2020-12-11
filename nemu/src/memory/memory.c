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
			Assert(Cache_1_B_bit == Cache_2_B_bit, "block size not equal!\n");
			cache_line_1 *new_ptr_1 = cache_1_replace(&Cache_1, addr);
			cache_line_2 *new_ptr_2 = cache_2_replace(&Cache_2, addr);
			if(new_ptr_2->dirty_bit == 1) {
				new_ptr_2->dirty_bit = 0;
				hwaddr_t old_addr = (new_ptr_2->tag << (Cache_2_B_bit + Cache_2_S_Bit)) + (((addr >> Cache_2_B_bit) & (0xffffffffu >> (32 - Cache_2_S_Bit))) << Cache_2_B_bit);
				int i;
				for(i = 0; i < Cache_1_B_size; i++) {
					dram_write(old_addr + i, 1, new_ptr_2->block[i]);
				}
			}
			hwaddr_t begin_addr = addr & (~((0xffffffffu) >> (32 - Cache_1_B_bit)));	
			int i;
			for(i = 0; i < Cache_1_B_size; i++, begin_addr++) {
				uint8_t tmp_data = (uint8_t)(dram_read(begin_addr, 1) & 0xff);
				new_ptr_1->block[i] = tmp_data;
				new_ptr_2->block[i] = tmp_data;
			}
			return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		}
		cache_2_hit++;
		memtime += 20u;
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_2_B_bit));
		cache_line_2 *ptr = cache_2_find(&Cache_2, addr);
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
			tmp = (uint32_t)(ptr->block[offset]);
			tmp = tmp << (8 * i);
			data += tmp;
		}
		Assert(Cache_1_B_bit == Cache_2_B_bit, "block size not equal!\n");
		cache_line_1 *new_ptr = cache_1_replace(&Cache_1, addr);
		for(i = 0; i < Cache_1_B_size; i++) {
			new_ptr->block[i] = ptr->block[i];
		}
		return data;
	}
	cache_1_hit++;
	memtime += 2u;
	uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
	cache_line_1 *ptr = cache_1_find(&Cache_1, addr);
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
		tmp = (uint32_t)(ptr->block[offset]);
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
	cache_line_1 *write_ptr_1 = cache_1_find(&Cache_1, addr);
	if(write_ptr_1 != NULL) {
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_1_B_bit));
		uint32_t tmp = 0;
		uint32_t tmp_data = data;
		int i;
		for(i = 0; i < len; i++, offset++) {
			if(offset >= Cache_1_B_size) {
				hwaddr_write(addr + i, len - i, tmp_data);
				break;
			}
			tmp = tmp_data & 0xff;
			tmp_data = tmp_data >> 8;
			write_ptr_1->block[offset] = (uint8_t)tmp;
		}
		dram_write(addr, len, data);
	}
	cache_line_2 *write_ptr_2 = cache_2_find(&Cache_2, addr);
	if(write_ptr_2 != NULL) {
		uint32_t offset = addr & (0xffffffffu >> (32 - Cache_2_B_bit));
		uint32_t tmp = 0;
		uint32_t tmp_data = data;
		int i;
		for(i = 0; i < len; i++, offset++) {
			if(offset >= Cache_2_B_size) {
				hwaddr_write(addr + i, len - i, tmp_data);
				break;
			}
			tmp = tmp_data & 0xff;
			tmp_data = tmp_data >> 8;
			write_ptr_2->block[offset] = (uint8_t)tmp;
		}
		write_ptr_2->dirty_bit = 1;
	}
	else {
		dram_write(addr, len, data);
		cache_line_2 *new_ptr_2 = cache_2_replace(&Cache_2, addr);
		if(new_ptr_2->dirty_bit == 1) {
			new_ptr_2->dirty_bit = 0;
			hwaddr_t old_addr = (new_ptr_2->tag << (Cache_2_B_bit + Cache_2_S_Bit)) + (((addr >> Cache_2_B_bit) & (0xffffffffu >> (32 - Cache_2_S_Bit))) << Cache_2_B_bit);
			int i;
			for(i = 0; i < Cache_1_B_size; i++) {
				dram_write(old_addr + i, 1, new_ptr_2->block[i]);
			}
		}
		hwaddr_t begin_addr = addr & (~((0xffffffffu) >> (32 - Cache_1_B_bit)));	
		int i;
		for(i = 0; i < Cache_1_B_size; i++, begin_addr++) {
			uint8_t tmp_data = (uint8_t)(dram_read(begin_addr, 1) & 0xff);
			new_ptr_2->block[i] = tmp_data;
		}
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

