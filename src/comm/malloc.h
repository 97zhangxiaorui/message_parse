#ifndef __MALLOC_H_
#define __MALLOC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*funtion     : malloc nMallocSize memory for return pointer
*nMallocSize : malloc memory size
*return      : 
*/
void * Malloc(int nMallocSize);

/*
*funtion : free memory of the pMemory pointer
*pMemory : memory pointer
*return  : 
*/
void Free(void *pMemory);

#endif