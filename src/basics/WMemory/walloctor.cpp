#include <walloctor.h>
#include <memory_mgr.h>

void* operator new(size_t size)
{    
    return MemoryMgr::GetInstance().Malloc(size);
}

void operator delete(void* ptr)
{
    MemoryMgr::GetInstance().Free(ptr);
    return;
}

void* operator new[](size_t size)
{
    return MemoryMgr::GetInstance().Malloc(size);
}

void operator delete[](void* ptr)
{
    MemoryMgr::GetInstance().Free(ptr);
    return;
}

void* mem_alloc(size_t size)
{
    return malloc(size);
}

void mem_free(void* ptr)
{
    free(ptr);
}



