#include "test.h"
#include <iostream>
#include "timer.h"


using namespace gdres;
using namespace std;


atomic<int> cntf(0);
void Func1()
{
    SLEEPS(1);
    LOG_SS() << "cnt is ：" << ++cntf;
}

void Ftest1()
{
    Scheduler* pool = new Scheduler(5, "test");
    pool->Start();
    for (int i = 0; i < 6; i++) {
        pool->AddTask(Func1);
    }
    getchar();
    pool->Stop();
    delete pool;    
}


void funt()
{
    static int coun = 5;
    LOG_SS() << "test in fiber coun = " << coun;

    SLEEPS(1);
    if(--coun >= 0) {
        Scheduler::GetThis()->AddTask(std::bind(funt), 2);
    }
}


void FiberPoolTest()
{
    Scheduler sch(4, "test");
    sch.Start();
    sch.AddTask(std::bind(funt));
    SLEEPS(5);
    sch.Stop();
    LOG_SS() << "over";
    getchar();
}




void FiberTest()
{
    FiberPoolTest();
}