#ifndef __HASH_H__
#define __HASH_H__

#include "const.h"
#include "list.h"

//#include <pthread.h>

//常规算法的黄金分隔
#define VOICE_HASH_GOLDEN_INTERER   0x9e370001

#define u32 unsigned int
#define JHASH_GOLDEN_RATIO  0x9e3779b9 

//传统HASH
#define VOICE_HASH_INTERGER(uiKey,uiHash) \
{\
    uiHash = uiKey * (VOICE_HASH_GOLDEN_INTERER);\
    uiHash = uiHash >> 21;\
}

#pragma pack(push)
#pragma pack(1)
typedef struct __hash_node__
{
	u32 key;
	void *data;
}hash_node;
#pragma pack(pop)

/*mix three value*/
#define __jhash_mix(a, b, c) \
{\
    a -= b; a -= c; a ^= (c>>13);\
    b -= c; b -= a; b ^= (a<<8);\
    c -= a; c -= b; c ^= (b>>13);\
    a -= b; a -= c; a ^= (c>>12);\
    b -= c; b -= a; b ^= (a<<16);\
    c -= a; c -= b; c ^= (b>>5);\
    a -= b; a -= c; a ^= (c>>3);\
    b -= c; b -= a; b ^= (a<<10);\
    c -= a; c -= b; c ^= (b>>15);\
}

static inline u32 jhash_3words(u32 a, u32 b, u32 c, u32 initval)
{
    a += JHASH_GOLDEN_RATIO;
    b += JHASH_GOLDEN_RATIO;
    c += initval;

    __jhash_mix(a, b, c);

    return c;
}

static inline u32 jhash_1words(u32 a, u32 initval)
{       
    return jhash_3words(0, 0, a, initval);
}

//u32 create_hash(u32 value );
u32 create_hash(u32 value, u32 hash_size );
//int add_hash( ListPtr *hash_name, u32 index_value, void *data);
//int add_hash( ListPtr *hash_name, u32 index_value, void *data, unsigned int data_len);
int add_hash( ListPtr *hash_name, u32 index_value, void *data, unsigned int data_len, unsigned int hash_size);
//void *get_data_from_hash(ListPtr *hash_name, u32 index_value);
void *get_data_from_hash(ListPtr *hash_name, u32 index_value, unsigned int hash_size);
void destroy(ListPtr *hash_name, int hash_size);
int init_hash_null(ListObj** hash, int hash_size);

#endif