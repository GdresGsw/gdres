#ifndef W_SAFE_VECTOR_H
#define W_SAFE_VECTOR_H

// #include <thread>
// #include <vector>
// #include <mutex>

// template <typename T>
// class SafeVector
// {
// public:
//     SafeVector() {};
//     ~SafeVector() {};

//     bool empty()  //数组是否为空
//     {
//         std::unique_lock<std::mutex> lck(mutex);
//         return vec.empty();
//     }

//     int size()
//     {
//         std::unique_lock<std::mutex> lck(mutex);
//         return vec.size();
//     }

//     void push_back(T &t)  // 往队列添加元素
//     {
//         std::unique_lock<std::mutex> lck(mutex);
//         vec.emplace_back(t);
//     }

//     bool take(T &t)  // 从队列取出元素并删除
//     {
//         std::unique_lock<std::mutex> lck(mutex);
//         if (!que.empty()) {   // 此处注意死锁，如果用自己封装的方法!empty()则死锁
//             t = std::move(que.front());
//             que.pop();
//             return true;
//         }
//         return false;
//     }

// private:
//     std::vector<T> vec;
//     std::mutex mutex;

// };










#endif

