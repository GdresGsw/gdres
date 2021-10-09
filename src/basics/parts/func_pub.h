/* =======================================================================
 * @brief  : 基础常用工具函数
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */
#ifndef _W_FUNC_PUB_H_
#define _W_FUNC_PUB_H_

#include <stdint.h>
#include <string>


namespace gdres {

// 获取当前线程ID
uint64_t GetThreadId();

// 获取当前线程名称（使用gdres::Thread创建的线程才能返回线程名称）
std::string GetThreadName();

// 获取当前协程ID
uint32_t GetFiberId();

// 获取当前的调用栈

// 获取当前栈信息的字符串

// 获取当前时间（s）
uint64_t GetNowTimS();

// 获取当前时间(ms)
uint64_t GetNowTimMS();

// 获取当前时间（us)
uint64_t GetNowTimUS();









}
#endif
