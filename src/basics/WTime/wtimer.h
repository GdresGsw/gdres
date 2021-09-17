#ifndef W_TIMER_H
#define W_TIMER_H

#include <thread>
// #include <chrono>


class WTimer
{
public:
    WTimer() {update();}
    ~WTimer() {}

    // 更新计时器
    void update()
    {
        begine = std::chrono::high_resolution_clock::now();
    }

    // 获取秒
    double GetSecond()
    {
        return this->GetMicroSec() * 0.000001;
    }

    // 获取毫秒
    double GetMilliSec()
    {
        return this->GetMicroSec() * 0.001;
    }

    // 获取微秒
    long long GetMicroSec()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - begine).count();
    }

protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> begine;

};


void wsleeps(int second) { std::this_thread::sleep_for(std::chrono::seconds(second)); }
void wsleep(int millisecond) { std::this_thread::sleep_for(std::chrono::milliseconds(millisecond)); }




#endif
