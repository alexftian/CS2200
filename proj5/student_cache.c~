#include "student_cache.h"
#include <math.h>

int student_read(address_t addr, student_cache_t *cache, stat_t *stats){


	int hit = 0;
	int i = 0;
	int done = 0;
	int block_index = addr_index(addr) % (int)pow(2,index_bit_num);
	int block_tag = addr_tag(addr);
	int LRU_set_num = 0;
	int LRU_max = 0;

	stats->accesses++;	
	stats->reads++;
	for(i = 0; i < pow(2,num_of_set); i++) {
		cache->sets[i].blocks[block_index].LRU++;
		if(cache->sets[i].blocks[block_index].tag == block_tag &&
			cache->sets[i].blocks[block_index].valid) {
				hit = 1;
				stats-> read_hit++;	
		}
	}

	if (hit == 0) {	
		stats->read_miss++;
		stats->misses++;
		stats->mem_read_bytes = stats->mem_read_bytes + (int)pow(2,block_bit_num); 
	// check out if it differs from write policy
		
		for(i = 0; i < pow(2, num_of_set); i++) {
			if(cache->sets[i].blocks[block_index].valid == 0) {
				cache->sets[i].blocks[block_index].tag = block_tag;
				cache->sets[i].blocks[block_index].valid = 1;
				cache->sets[i].blocks[block_index].LRU = 0;
				done = 1;
				if(write_policy == WBWA && cache->sets[i].blocks[block_index].dirty) {
					cache->sets[i].blocks[block_index].dirty = 0;
					stats->mem_write_bytes = stats->mem_write_bytes + (int)pow(2,block_bit_num);
				}

				break;
			}
		}
		 
		if(done == 0) {
			for(i = 0; i < pow(2, num_of_set); i++) {
				if(cache->sets[i].blocks[block_index].LRU > LRU_max) {
					LRU_max = cache->sets[i].blocks[block_index].LRU;
					LRU_set_num = i;
				} 
			}
			cache->sets[LRU_set_num].blocks[block_index].valid = 1;
			cache->sets[LRU_set_num].blocks[block_index].tag = block_tag;
			cache->sets[LRU_set_num].blocks[block_index].LRU = 0;
			if(cache->sets[LRU_set_num].blocks[block_index].dirty && write_policy == WBWA) {
				stats->mem_write_bytes = stats->mem_write_bytes + (int)pow(2,block_bit_num); 
				
			}
 
		}		

	}


	return hit;
}

int student_write(address_t addr, student_cache_t *cache, stat_t *stats){

	
	int hit = 0;
	int i = 0;

	int block_index = addr_index(addr) % (int)pow(2,index_bit_num);
	int block_tag = addr_tag(addr);
	int LRU_set_num = 0;
	int LRU_max = 0;
	int done = 0;
	stats->accesses++;	
	stats->writes++;
	for(i = 0; i < pow(2,num_of_set); i++) {
		if(cache->sets[i].blocks[block_index].tag == block_tag &&
			cache->sets[i].blocks[block_index].valid) {
				hit = 1;
				stats-> write_hit++;	
			if(write_policy == WBWA) {
				cache->sets[i].blocks[block_index].dirty = 1;
				cache->sets[i].blocks[block_index].LRU = 0;
				cache->sets[i].blocks[block_index].valid = 1;
			} else {
				stats->mem_read_bytes = stats->mem_read_bytes + (int)pow(2,block_bit_num); 
				cache->sets[i].blocks[block_index].valid = 0;
				cache->sets[i].blocks[block_index].LRU = 0;
			}


		}
	}

	if (hit == 0) {	
		stats->write_miss++;
		stats->misses++;
		
		if(write_policy == WTWNA) {
			stats->mem_write_bytes = stats->mem_write_bytes + (int)pow(2,block_bit_num); 
		} else {

			for(i = 0; i < pow(2, num_of_set); i++) {
				if(cache->sets[i].blocks[block_index].valid == 0) {
					cache->sets[i].blocks[block_index].tag = block_tag;
					cache->sets[i].blocks[block_index].valid = 1;
					cache->sets[i].blocks[block_index].dirty = 1;
					done = 1;
					break;
				}
			}
			 
			if(done == 0) {
				for(i = 0; i < pow(2, num_of_set); i++) {
					if(cache->sets[i].blocks[block_index].LRU > LRU_max) {
						LRU_max = cache->sets[i].blocks[block_index].LRU;
						LRU_set_num = i;
					} 
					cache->sets[i].blocks[block_index].LRU++;
				}
				cache->sets[LRU_set_num].blocks[block_index].valid = 1;
				cache->sets[LRU_set_num].blocks[block_index].tag = block_tag;
				cache->sets[LRU_set_num].blocks[block_index].LRU = 0;
				if(cache->sets[LRU_set_num].blocks[block_index].dirty && write_policy == WBWA) {
					stats->mem_write_bytes = stats->mem_write_bytes + (int)pow(2,block_bit_num); 
				}
	 
			}		
		}
	}

	return hit;
}

student_cache_t *allocate_cache(int C, int B, int S, int WP, stat_t* statistics){
	int num_of_block = C - B;
	num_of_set = num_of_block - S;
	int i = 0;
	int j = 0;
	block_per_set = S;
	index_bit_num = num_of_block - num_of_set;
	block_bit_num = B;
	tag_bit_num = 32 - (index_bit_num + block_bit_num);
	write_policy = WP;	

	student_cache_t *cache = malloc(sizeof(student_cache_t));
	cache->sets = malloc(sizeof(set) * pow(2,num_of_set));
	
	for (i = 0; i < pow(2, num_of_set); i++){ 
		cache->sets[i].blocks = malloc(sizeof(block) * pow(2,S));
		for(j = 0; j < pow(2, index_bit_num); j++) {

			cache->sets[i].blocks[j].LRU = 0;
			cache->sets[i].blocks[j].valid = 0;
			cache->sets[i].blocks[j].tag = 0;
		}
	}
	statistics->accesses = 0;
	statistics->misses = 0;
	statistics->reads = 0;
	statistics->writes = 0;
	statistics->read_hit = 0;
	statistics->write_hit = 0;
	statistics->read_miss = 0;
	statistics->write_miss = 0;
	statistics->mem_write_bytes = 0;
	statistics->mem_read_bytes = 0;
	statistics->hit_rate = 0;
	statistics->AAT = 0;
	
	int block_meta = tag_bit_num + 1; 
	if(WP == WBWA) { 
		block_meta++; 
	} 
	statistics->total_bits += block_meta * pow(2,C - B); 
	int LRU_meta = pow(2,S) * S; 
	statistics->total_bits += LRU_meta * pow(2,index_bit_num); 
	statistics->total_bits += pow(2,C) * 8;


	return cache;
}

void finalize_stats(student_cache_t *cache, stat_t *statistics){ 
	statistics->read_hit_rate = (statistics->read_hit * 1.0)/(statistics->reads); 
	statistics->hit_rate = (statistics->read_hit + statistics->write_hit)*1.0 / statistics->accesses; 
	double hit_time = 2 + .2 * block_per_set; 
	double read_miss_rate = 1-statistics->read_hit_rate; 
	statistics->AAT = hit_time + read_miss_rate * 50;

}

int addr_index(int addr) {
	return addr >> (tag_bit_num + block_bit_num);
}

int addr_tag(int addr) {
	return (addr >> block_bit_num) & (int)((pow(2,tag_bit_num) - 1));
}

int addr_block(int addr) {
	return (addr & (int)(pow(2, block_bit_num) - 1)); 

}

