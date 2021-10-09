#include "test.h"
#include "log.h"
#include "timer.h"

using namespace gdres;
using namespace std;

atomic<int> cnt(0);
void Func()
{
    SLEEPS(1);
    LOG_SS() << "cnt is ：" << ++cnt;
}

void tpTest1()
{
    ThreadPool* pool = new ThreadPool(5);
    for (int i = 0; i < 6; i++) {
        pool->AddTask(Func);
    }
    getchar();
    delete pool;    
}



void ThreadPoolTest()
{
    tpTest1();
}