#include "test.h"
#include <iostream>

using namespace gdres;
using namespace std;

// 日志输出测试
void logTest1()
{
    LOG_SS() << "this is a test";
    // GDRES_LOG_SS(gdres::LogLevel::ERROR) << "this is a test";
    LOG("A IS %d\n", 123);
    // GDRES_LOG(LOG_ERROR, "A IS %d\n", 123);
    // GDRES_LOG_SS(LogLevel::ERROR) << "this is a test";

}












void LogTest()
{
    logTest1();
}

