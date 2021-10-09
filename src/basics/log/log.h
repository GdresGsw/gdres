/* =======================================================================
 * @brief  : 封装日志
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */
#ifndef _GDRES_W_LOG_H_
#define _GDRES_W_LOG_H_

#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <fstream>
#include <lock.h>
#include <file.h>
#include "func_pub.h"

#ifdef ERROR
#undef ERROR
#endif

// 每条日志最多写入内容的字符数
#define LOG_CHAR_NUM_MAX 200

//  *  %m 消息
//  *  %p 日志级别
//  *  %r 累计毫秒数
//  *  %c 日志名称
//  *  %t 线程id
//  *  %n 换行
//  *  %d 时间
//  *  %f 文件名
//  *  %l 行号
//  *  %T 制表符
//  *  %F 协程id
//  *  %N 线程名称
#define GDRES_LOG_STYLE "%d{%Y-%m-%d %H:%M:%S}%T%N[%t]%T%F%T[%p]%T%f[%l]:%m%n"

#ifdef _WIN32
// #define LOG_FILE_PATH "..\\gdres_log.txt"
#define LOG_FILE_PATH "gdres_log.txt"
#define filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#else
// #define LOG_FILE_PATH "/home/guo/temp/gdres_log.txt"
#define LOG_FILE_PATH "gdres_log.txt"
#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#endif

// 使用流的方式写入日志
#define GDRES_LOG_SS(level) \
    gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), gdres::GetFiberId(), 0, time(0),\
                        gdres::GetThreadName(), gdres::LoggerSingle::GetInstance()->GetLogger(), level)).GetSS()

// #define GDRES_LOG_SS(level) \
    // if(gdres::LoggerSingle::GetInstance()->GetLogger()->GetLevel() <= level) \
    //     gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), gdres::GetFiberId(), 0, time(0),\
    //                         gdres::GetThreadName(), gdres::LoggerSingle::GetInstance()->GetLogger(), level)).GetSS()

#define LOG_SS() \
    gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), gdres::GetFiberId(), 0, time(0),\
                        gdres::GetThreadName(), gdres::LoggerSingle::GetInstance()->GetLogger(), gdres::LogLevel::EVENT)).GetSS()

// #define LOG_SS() \
//     gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), 0, 0, time(0),\
//                         "123", gdres::LoggerSingle::GetInstance()->GetLogger(), gdres::LogLevel::EVENT)).GetSS()

// 使用格式化方式写日志
#define GDRES_LOG(level, fmt, ...) \
    if(gdres::LoggerSingle::GetInstance()->GetLogger()->GetLevel() <= level) \
        gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), gdres::GetFiberId(), 0, time(0),\
                            gdres::GetThreadName(), gdres::LoggerSingle::GetInstance()->GetLogger(), level)).GetEvent()->Format(fmt, __VA_ARGS__)

#define LOG(fmt, ...) \
    gdres::LogEventWrap(new gdres::LogEvent(filename(__FILE__), __LINE__, gdres::GetThreadId(), gdres::GetFiberId(), 0, time(0),\
                        gdres::GetThreadName(), gdres::LoggerSingle::GetInstance()->GetLogger(), gdres::LogLevel::EVENT)).GetEvent()->Format(fmt, __VA_ARGS__)



#define LOG_DEBUG gdres::LogLevel::DEBUG 
#define LOG_INFO gdres::LogLevel::INFO 
#define LOG_WARN gdres::LogLevel::WARN 
#define LOG_ERROR gdres::LogLevel::ERROR 
#define LOG_EVENT gdres::LogLevel::EVENT


namespace gdres {

class Logger;
class LogAppender;

// 日志级别类 定义日志输出级别
class LogLevel{
    public:
        enum Level{
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            EVENT = 5
        };
        static const char* ToString(LogLevel::Level);
        static LogLevel::Level FromString(const std::string& str);
};


// 日志事件类
class LogEvent{
public:
    LogEvent(const char* file, int32_t lineNum, uint64_t thId, uint32_t fiId, uint32_t elap, uint64_t timeNow,
             const std::string& thName, Logger* logge, LogLevel::Level lev);

    // 返回文件名
    std::string GetFile() const{return fileName;}
    // 返回行号
    int32_t GetLine() const {return line;}
    // 返回线程ID
    uint64_t GetThreadId() const {return threadId;}
    // 返回协程ID
    uint64_t GetFiberId() const {return fiberId;}
    // 返回启动至当前时间的耗时
    uint32_t GetElapse() const {return elapse;}  
    // 返回时间     
    uint64_t GetTime() const {return time;}                     
    // 返回线程名称
    std::string GetThreadName() const { return threadName;} 
    // 返回日志内容    
    std::string GetContent() const { return ss.str();} 
    // 返回日志内容字符串流         
    std::stringstream& GetSS() { return ss;} 
    // 返回日志器
    Logger* GetLogger() const { return logger;}    
    // 返回日志级别
    LogLevel::Level GetLevel() const { return level;}           
    
    // 格式化写入日志内容
    void Format(const char* fmt, ...);      
    // 格式化写入日志内容    
    void Format(const char* fmt, va_list al);

private:
    std::string fileName;                   // 文件名称
    int32_t line;                           // 行号
    uint64_t threadId;                      // 线程ID
    uint32_t fiberId;                       // 协程ID
    uint32_t elapse;                        // 程序运行到现在的时间(毫秒)
    uint64_t time;                          // 当前时间
    std::string threadName;                 // 线程名称
    std::stringstream ss;                   // 日志内容流
    Logger* logger;                         // 日志器
    LogLevel::Level level;                  // 日志级别
};

// 日志格式器
class LogFormatter
{
public:
    /*  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern);
    ~LogFormatter();

    // 返回格式化日志文本
    std::string Format(Logger* logger, LogLevel::Level level, LogEvent* event);
    std::ostream& Format(std::ostream& ofs, Logger* logger, LogLevel::Level level, LogEvent* event);

    // 初始化  解析日志模板
    void Init();

    // 是否有错误
    bool IsError() const { return isError;}

    // 返回日志模板
    const std::string GetPattern() const { return pattern;}

public:
    // 日志内容格式化项
    class FormatItem{
    public:
        virtual ~FormatItem(){}

        // 格式化日志到流
        virtual void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) = 0;
    };  

private:
    std::string pattern;                   // 格式模板
    std::vector<FormatItem*> items;        // 日志格式解析后格式
    bool isError;                          // 是否有错误
};
   
//日志器
class Logger
{
public:
    Logger(const std::string& name = "root");
    virtual ~Logger();
    
    void Log(LogLevel::Level, LogEvent*);     //日志输出
    void Debug(LogEvent*);                    // 输出debug级别的日志
    void Info(LogEvent*);
    void Warn(LogEvent*);
    void Error(LogEvent*);
    void Event(LogEvent*);
    
    void AddAppender(LogAppender*);           // 添加日志输出地目标
    void DelAppender(LogAppender*);           // 删除
    void ClearAppender();                     // 清除

    const std::string GetName() const {return name;}
    LogLevel::Level GetLevel() const {return level;}
    void SetLevel(LogLevel::Level val) {level = val;}

    void SetFormatter(LogFormatter* val);      // 设置日志格式器
    LogFormatter* GetFormatter();              //获取日志格式器

private:
    std::string name;        // 日志器名称 
    LogLevel::Level level;   // 要输出的日志级别大于该值才能能输出日志
    std::vector<LogAppender*> appenders; // 输出位置集合
    LogFormatter* formatter;  // 日志格式
    SpinLock spLock;          // 自旋锁
}; 

//日志输出器，日志输出位置
class LogAppender
{
friend class Logger;
public:
    LogAppender() : formatter(nullptr), level(LogLevel::DEBUG) {}
    virtual ~LogAppender(){}

    virtual void Log(Logger*, LogLevel::Level, LogEvent*) = 0;  // 写入日志

    void SetFormatter(LogFormatter* val);     // 更改日志格式
    LogFormatter* GetFormatter();       // 获取日志格式
    LogLevel::Level GetLevel();         // 获取日志级别
    void SetLevel(LogLevel::Level val);       // 设置日志级别
protected:
    LogFormatter* formatter;          // 日志输出格式
    LogLevel::Level level;            // 日志输出级别
    SpinLock spLock;                  // 自旋锁
    bool hasFormatter = false;        // 是否有自己的日志格式器，暂不扩展
};

//输出到控制台的输出器
class StdoutLogAppender : public LogAppender
{
public:
    void Log(Logger* logger, LogLevel::Level lev, LogEvent* eve) override;
};

//输出到文件的输出器
class FileoutLogAppender : public LogAppender
{
public:
    FileoutLogAppender(const std::string& filename);
    void Log(Logger* logger, LogLevel::Level lev, LogEvent* eve) override;
    bool ReOpen();    // 重新打开日志文件  成功返回true
private:    
    std::string fileName;     // 文件路径
    std::ofstream fileStream;   // 文件流
    uint64_t lastTime;   // 上次重新打开时间
};


// 单例日志器（饿汉式） 当前需求仅有一个日志文件作为输出
// 多线程条件下日志量大时可能出现日志紊乱的问题，可以根据需要为不同线程创建不同的日志器进行日志输出
class LoggerSingle
{
private:
    LoggerSingle();
public:
    static LoggerSingle* GetInstance();
    Logger* GetLogger() {return logger;}
private:
    static LoggerSingle* logSig;
    Logger* logger;
    LogAppender* stdoutAppender;
    LogAppender* fileAppender;
};

// 日志事件封装
class LogEventWrap
{
public:
    LogEventWrap(LogEvent*);
    ~LogEventWrap();

    // 获取日志事件
    LogEvent* GetEvent() {return event;}

    // 获取日志内容流
    std::stringstream& GetSS();
private:
    LogEvent* event;
};







}
#endif
