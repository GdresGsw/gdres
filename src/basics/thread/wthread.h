/* =======================================================================
 * @brief  : 对thread类进行封装，支持线程命名等
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */
#ifndef _W_THREAD_H_
#define _W_THREAD_H_

#include <thread>
#include <string.h>
#include <functional>
#include "lock.h"

namespace gdres {


class Thread
{
public:
    enum State {
        INIT,      // 初始化状态
        EXEC,      // 执行中
        TERM,      // 线程已结束
    };

    Thread(std::function<void()> cb, const std::string& thName = "");
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    const std::string& getname() {return name;}
    uint64_t getid() {return id;};
    State getstate() {return thState;}
    void reset(std::function<void()>, const std::string& name = "");
    void join();

    static Thread* GetThis();                            // 获取当前线程的指针
    static const std::string& GetThreadName();           // 获取线程名称
    static void SetThreadName(const std::string& name);  // 设置当前线程名称

    

private:
    Thread();
    void Run();

private:
    std::string name;
    uint64_t id;
    std::thread* th;
    std::function<void()> func;  
    gdres::Semaphore sem;
    State thState;
};





}


#endif
