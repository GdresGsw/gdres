#ifndef W_SAFE_QUEUE_H
#define W_SAFE_QUEUE_H

#include <thread>
#include <queue>
#include <mutex>

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










#endif

