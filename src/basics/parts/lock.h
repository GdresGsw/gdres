#ifndef _W_LOCK_H_
#define _W_LOCK_H_

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace gdres {

// 锁基类
class WLock
{
public:
    virtual ~WLock() {}
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

// 自旋锁
class SpinLock : public WLock
{
public:
    SpinLock() : flag(false) {}

    virtual void lock() override;
    virtual void unlock() override;

private:
    std::atomic<bool> flag;
};

// 自动锁
class WrapLock
{
public:
    WrapLock(WLock &lk) : lck(lk) {lck.lock();}
    ~WrapLock() {lck.unlock();}
private:
    WLock &lck;
};




// 读写锁
class RdWrLock
{
public:
    RdWrLock() : count(0) {}

    void wrlock();
    void wrunlock();
    void rdlock();
    void rdunlock();


private:
    std::mutex readMtx;
    std::mutex writeMtx;
    int count;
};

// 信号量
class Semaphore
{
public:
    Semaphore(int cnt = 0) : count(cnt) {}

    // V操作 唤醒（count非0时唤醒）
    void signal();

    // P操作，阻塞(count为0时阻塞)
    void wait();

private:
    std::mutex mtx;
    std::condition_variable cond;
    int count;
};

}



#endif
