#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define WBWA 0
#define WTWNA 1


/*
	Create your datastructure here
	DO NOT CHANGE THE NAME
*/


typedef struct{
	int valid;
	int tag;
	int LRU;
	int dirty;
} block;

typedef struct {
	block* blocks;
} set;


typedef struct {
	set* sets;
}student_cache_t;
/*
	you may change the name here if you want an alias for it
	typedef student_cache_t also_student_cache_t;
*/
int num_of_set;
int index_bit_num;
int tag_bit_num;
int block_bit_num;
int write_policy;
int block_per_set;

/*
	Used for tracking statistics
	You may add aditional fields
	but only the ones already 
	implemented compared for correctness
*/
typedef struct {
	long accesses;
	long misses;
	long reads;
	long writes;
	long read_hit;
	long write_hit;
	long read_miss;
	long write_miss;
	long mem_write_bytes;
	long mem_read_bytes;
	double read_hit_rate;
	double hit_rate;
	long total_bits;
	double AAT;
} stat_t;

/*
	an address
*/
typedef int address_t;
/*
	Read or write the specified address
	return 0 if it was a miss
	return non-0 for a hit
*/
int student_read(address_t addr, student_cache_t *cache, stat_t *stats);
int student_write(address_t addr, student_cache_t *cache, stat_t *stats);

/*
	Allocate and return a pointer to a cache
	to be used for the duration of the simulation

	you may also initialize any fields of the statistics structure

	2^C is the total size in bytes of the cache
	2^B is the size in bytes of each block
	2^S is the number of blocks in each set

	WP = {WTWNA: write through, write no allocate; 
	      WBWA: write back, write allocate}

	
*/
student_cache_t *allocate_cache(int C, int B, int S, int WP, stat_t* statistics);

/*
	Do anything you need to to make 
	all of the values ready for printing
*/
void finalize_stats(student_cache_t *cache, stat_t *statistics);


int addr_index(int addr);

int addr_tag(int addr);

int addr_block(int addr);
