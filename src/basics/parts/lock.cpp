/* =======================================================================
 * @brief  : 锁相关的封装
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */

#include "lock.h"

namespace gdres {

/*==================================================================
 * FuncName : RdWrLock::
 * description : 读写锁，传入自动锁时，默认锁的是写锁
 * author : guoshuaiwei  
 *==================================================================*/
void RdWrLock::wrlock()
{
    writeMtx.lock();
}

void RdWrLock::wrunlock()
{
    writeMtx.unlock();
}

void RdWrLock::rdlock()
{
    readMtx.lock();
    if (++count == 1) {  // 第一个进入读的时候上写锁
        writeMtx.lock();
    }
    readMtx.unlock();
}

void RdWrLock::rdunlock()
{
    readMtx.lock();
    if (--count == 0) {  // 没有读的时候将写锁解锁
        writeMtx.unlock();
    }
    readMtx.unlock();
}

/*==================================================================
 * FuncName : SpinLock::
 * description : 自旋锁
 * author : guoshuaiwei  
 *==================================================================*/
void SpinLock::lock()
{
    bool expect = false;
    while (!flag.compare_exchange_weak(expect, true)) {
        expect = false;
    } 
}

void SpinLock::unlock()
{
    flag.store(false);
}

/*==================================================================
 * FuncName : Semaphore::
 * description : 信号量
 * author : guoshuaiwei  
 *==================================================================*/
void Semaphore::signal()
{
    std::unique_lock<std::mutex> lck(mtx);
    count++;
    cond.notify_one();

}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lck(mtx);    
    while (count <= 0) {
        cond.wait(lck);
    }
    count--;
}









}
