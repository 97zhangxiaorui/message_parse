#include "malloc.h"

void * Malloc(int nMallocSize)
{
	//return calloc(0, nMallocSize);
	return malloc(nMallocSize);
}

void Free(void *pMemory)
{
	free(pMemory);
}