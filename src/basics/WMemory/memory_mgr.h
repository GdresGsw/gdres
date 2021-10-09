#ifndef _W_MEMORY_MANAGER_H_
#define _W_MEMORY_MANAGER_H_
#include <stdlib.h>
#include <math.h>

class MemoryPool;
// 内存块 内存池最小单元
class MemoryBlock
{
public:

public:
    int id;              // 块编号
    int refCnt;          // 引用次数
    MemoryPool* memPool; // 所属内存池
    MemoryBlock* next;   // 下一块位置
    bool isInPool;         // 是否在内存池中
};

#define MEM_BLOCK_SIZE sizeof(MemoryBlock);

// 内存池
class MemoryPool
{
public:
    MemoryPool(int bSize, int bNum) : poolAddr(nullptr), headBlock(nullptr)
    {
        if ()
    }

    // 申请内存
    void* Malloc(size_t size) {
        if (!poolAddr) {
            Init();
        }

        MemoryBlock* ptrRet = nullptr;
        if (!headBlock) {
            // 内存块被使用完了
            ptrRet = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
            ptrRet->id = 0;
            ptrRet->refCnt = 0;
            ptrRet->isInPool = false;
            ptrRet->memPool = this;
            ptrRet->next = nullptr;
        } else {
            ptrRet = headBlock;
            headBlock = headBlock->next;
            ptrRet->refCnt = 1;
        }
        return ((char*)ptrRet + sizeof(MemoryBlock));
    }

    // 释放内存
    void Free(void* ptr) {
        MemoryBlock* block = (MemoryBlock*)((char*)ptr - sizeof(MemoryBlock));
        if (block->isInPool) {
            if (--block->refCnt != 0) {
                // 此内存块被多次引用，其中一个引用被释放，则不做处理
                return;
            }
            // 如果要释放的内存属于此内存池，则将内存区域还给此内存池
            block->next = headBlock;
            headBlock = block;
            return;
        }

        // 如果不在内存池中，直接free还给操作系统
        free(block);
    }

    // 初始化
    void Init()
    {
        if (!poolAddr) {
            return;
        }
        // 申请内存池内存
        poolAddr = (char*)malloc(blockSize * blockNum);
        // 初始化内存单元
        headBlock = nullptr;
        for (int i = blockNum - 1; i >= 0; i--) {
            MemoryBlock* mem = (MemoryBlock*)(poolAddr + (i * blockSize));
            mem->id = 0;
            mem->refCnt = 0;
            mem->isInPool = true;
            mem->memPool = this;
            mem->next = headBlock;
            headBlock = mem;
        }
    }
private:
    char* poolAddr;          // 内存池地址
    MemoryBlock* headBlock;  // 头部内存单元
    size_t blockSize;        // 内存块的大小（实际可用单元大小）
    size_t blockNum;         // 内存块的数量
};



// 内存管理  单例模式
class MemoryMgr
{
private:
    MemoryMgr() {}
public:
    static MemoryMgr& GetInstance()
    {
        static MemoryMgr mgr;
        return mgr;
    }
    // 申请内存
    void* Malloc(size_t size) {
        return malloc(size);
    }

    // 释放内存
    void Free(void* ptr) {
        free(ptr);
    }
};










#endif
