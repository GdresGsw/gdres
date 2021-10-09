/* =======================================================================
 * @brief  : 基础常用工具函数
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */
#include "func_pub.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <string>
#include <string.h>
#include "fiber.h"
#include "wthread.h"

namespace gdres {

// 获取当前线程ID
uint64_t GetThreadId()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    uint64_t id;
    ss >> id;
    return id;
}

// 获取当前线程名称（使用gdres::Thread创建的线程才能返回线程名称）
std::string GetThreadName()
{
    return gdres::Thread::GetThreadName();
}

// 获取当前协程ID
uint32_t GetFiberId()
{
    return gdres::Fiber::GetFiberId();
}

// 获取当前时间（s）
uint64_t GetNowTimS()
{
    return std::chrono::time_point_cast<std::chrono::seconds>(
           std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}

// 获取当前时间
uint64_t GetNowTimMS()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(
           std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}

// 获取当前时间（us)
uint64_t GetNowTimUS()
{
    return std::chrono::time_point_cast<std::chrono::microseconds>(
           std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}



}

