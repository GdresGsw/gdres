#ifndef _W_MEMROY_ALLOCTOR_H_
#define _W_MEMROY_ALLOCTOR_H_

#include <stdlib.h>

void* operator new(size_t size);
void operator delete(void* ptr);
void* operator new[](size_t size);
void operator delete[](void* ptr);
void* mem_alloc(size_t size);
void mem_free(void* ptr);





#endif
