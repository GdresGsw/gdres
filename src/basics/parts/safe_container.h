/* =======================================================================
 * @brief  : 封装容器，支持多线程安全操作
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */

#ifndef W_SAFE_CONTAINER_H
#define W_SAFE_CONTAINER_H

#include <thread>
#include <queue>
#include <mutex>

namespace gdres {

// 安全队列
template <typename T>
class SafeQueue
{
public:
    SafeQueue() {};
    ~SafeQueue() {};

    bool empty()  // 队列是否为空
    {
        std::unique_lock<std::mutex> lck(mutex);
        return que.empty();
    }

    int size()    // 队列大小
    {
        std::unique_lock<std::mutex> lck(mutex);
        return que.size();
    }

    void push(T &t)  // 往队列添加元素
    {
        std::unique_lock<std::mutex> lck(mutex);
        que.emplace(t);
    }

    bool take(T &t)  // 从队列取出元素并删除
    {
        std::unique_lock<std::mutex> lck(mutex);
        if (!que.empty()) {   // 此处注意死锁，如果用自己封装的方法!empty()则死锁
            t = std::move(que.front());
            que.pop();
            return true;
        }
        return false;
    }

private:
    std::queue<T> que;
    std::mutex mutex;

};








}

#endif

