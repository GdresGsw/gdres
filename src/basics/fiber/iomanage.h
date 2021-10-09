/* =======================================================================
 * @brief  : 基于协程调度器和epoll封装io协程调度
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#ifndef _GDRES_IOMANAGE_H_
#define _GDRES_IOMANAGE_H_

#include "scheduler.h"
#include "lock.h"
#include <mutex>

namespace gdres {


class IOManager : public Scheduler
{
public:
    typedef gdres::RdWrLock MutexType;  // 用读写锁

    enum Event {
       NONE = 0x0,
       READ = 0x01, 
       WRITE = 0x04,
    };

    IOManager(size_t threads = 1, const std::string& name = "", bool useCaller = false);
    ~IOManager();

    int AddEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool DelEvent(int fd, Event event);
    bool CancelEvent(int fd, Event event);
    bool CancelAll(int fd);

    static IOManager* GetThis();
private:
    // 事件上下文
    struct EventContext {
        Scheduler* scheduler;      // 执行的协程调度器
        Fiber* fiber;              // 协程
        std::function<void()> cb;  // 函数
    };
    
    struct FdContext {
        EventContext& GetContext(Event event);   // 获取对应事件上下文
        void ResetContext(EventContext& ctx);    // 重置事件上下文
        void TriggerEvent(Event event);          // 触发事件

        EventContext read;          // 读事件
        EventContext write;         // 写事件
        int fd;                     // 事件关联的句柄
        Event events = NONE;        // 已注册事件
        std::mutex mtx;             // 互斥锁
    };

    int m_epfd;                     // epoll文件句柄
    // int m_tickleFds[2];             // pipe文件句柄
    std::atomic<size_t> m_pendingEventCount;   // 当前等待执行的事件数量
    MutexType m_mutex;                      
    std::vector<FdContext*> m_fdContexts;      // 事件上下文容器
};





}

#endif



