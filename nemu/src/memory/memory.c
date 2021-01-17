#include "common.h"
#include "cache.h"
#include "nemu.h"

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

hwaddr_t page_translate(lnaddr_t addr) {
	if(cpu.cr0.protect_enable == 0 || cpu.cr0.paging == 0) {
		return addr;
	}
	hwaddr_t tlb_find = ((addr >> 12) & 0xfffff);
	hwaddr_t page_dir_index = ((addr >> 22) & 0x3ff);
	hwaddr_t page_frame_index = ((addr >> 12) & 0x3ff);
	hwaddr_t offset = (addr & 0xfff);
	int i;
	for(i = 0; i < 64; i++) {
		if(cpu.tlb[i].vpage == tlb_find && cpu.tlb[i].vaild == 1) {
			return cpu.tlb[i].ppage + offset;
		}
	}
	hwaddr_t page_dir_base = (cpu.cr3.page_directory_base << 12);
	hwaddr_t page_frame_base = hwaddr_read(page_dir_base + 4 * page_dir_index, 4);
	assert((page_frame_base & 0x1) == 1);
	page_frame_base = (page_frame_base & 0xfffff000);
	hwaddr_t page_frame = hwaddr_read(page_frame_base + 4 * page_frame_index, 4);
	assert((page_frame & 0x1) == 1);
	page_frame = (page_frame & 0xfffff000);
	hwaddr_t hwaddr = page_frame + offset;
	cpu.tlb[cpu.tlb_index].vpage = tlb_find;
	cpu.tlb[cpu.tlb_index].ppage = page_frame;
	cpu.tlb[cpu.tlb_index].vaild = 1;
	cpu.tlb_index = (cpu.tlb_index + 1) % 64;
	return hwaddr;
}

// uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
// 	return hwaddr_read(addr, len);
// }

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	if((addr & 0xfff) + len - 1 > 0xfff) {
		uint32_t res = 0;
		int i;
		for(i = 0; i < len; i++) {
			res = res << 8;
			uint32_t tmp = (lnaddr_read(addr + i, 1) & 0xff);
			res += tmp;
		}
		return res;
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

// void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
// 	hwaddr_write(addr, len, data);
// }

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
	if(0) {
		assert(0);
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_write(hwaddr, len, data);
	}
}

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
		// printf("swaddr: 0x%x\n", addr);
	if(cpu.cr0.protect_enable == 0) {
		// printf("lnaddr: 0x%x\n", (int)addr);
		return (lnaddr_t)addr;
	}
	else {
		// printf("lnaddr: reg %d cache 0x%lx 0x%x\n", sreg, cpu.SRcache[sreg], (int)(cpu.SRcache[sreg] & 0xffffffff) + addr);
		return ((cpu.SRcache[sreg] & 0xffffffff) + addr);
	}
}

// uint32_t swaddr_read(swaddr_t addr, size_t len) {
// #ifdef DEBUG
// 	assert(len == 1 || len == 2 || len == 4);
// #endif
// 	return lnaddr_read(addr, len);
// }

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

// void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
// #ifdef DEBUG
// 	assert(len == 1 || len == 2 || len == 4);
// #endif
// 	lnaddr_write(addr, len, data);
// }

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}
