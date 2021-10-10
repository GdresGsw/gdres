/* =======================================================================
 * @brief  : 封装日志
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */
#include <log.h>
#include <string>
#include <cstdarg>
#include <stdio.h>
#include <time.h>
#include <map>
#include <functional>
#include <lock.h>
#include <file.h>

namespace gdres {

#define THREAD_NAME_LEN 12
#define FILE_NAME_LEN 20


//================================  class LogLevel  ================================//
/*==================================================================
 * FuncName : LogLevel::ToString
 * description :  将级别转换成字符串
 * author : guoshuaiwei  
 *==================================================================*/
const char* LogLevel::ToString(LogLevel::Level level)
{
        switch(level){
    #define XX(name)\
        case LogLevel::name:\
            return #name;\
            break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(EVENT);
    #undef XX

        default:
            return "UNKNOW";
        }
}

LogLevel::Level FromString(const std::string& str)
{
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(EVENT, event);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(EVENT, EVENT);
    return LogLevel::UNKNOW;
#undef XX
}


//================================  class LogEvent  ================================//

/*==================================================================
 * FuncName : LogEvent::LogEvent
 * description : 
 * author : guoshuaiwei  
 *==================================================================*/
LogEvent::LogEvent(const char* file, int32_t lineNum, uint64_t thId, uint32_t fiId, uint32_t elap,
                   uint64_t timeNow, const std::string& thName, Logger* logge, LogLevel::Level lev) :
                   fileName(file), line(lineNum),
                   threadId(thId), fiberId(fiId),
                   elapse(elap), time(timeNow), threadName(thName),
                   logger(logge), level(lev)
{
    if (thName.length() < THREAD_NAME_LEN) {
        threadName = std::string(THREAD_NAME_LEN - thName.length(), ' ') + thName;
    }

    if (strlen(file) < FILE_NAME_LEN) {
        fileName = std::string(FILE_NAME_LEN - strlen(file), ' ') + file;
    }
}

/*==================================================================
 * FuncName : LogEvent::Format
 * description : 
 * author : guoshuaiwei  
 *==================================================================*/
void LogEvent::Format(const char* fmt, ...) {
    va_list al;          // al指向形参列表内存
    va_start(al, fmt);   // 初始化形参列表指针
    Format(fmt, al);
    va_end(al);
}

/*==================================================================
 * FuncName : LogEvent::LogEvent
 * description : 
 * author : guoshuaiwei  
 *==================================================================*/
void LogEvent::Format(const char* fmt, va_list al) {
    char buf[LOG_CHAR_NUM_MAX] = {};
    // int len = vasprintf(&buf, fmt, al);
    int len = vsprintf(buf, fmt, al);
    if(len != -1) {
        ss << std::string(buf, len);
    }
}


//================================  class Logger  ================================//

/*==================================================================
 * FuncName : Logger::Logger
 * description : 构造
 * author : guoshuaiwei  
 *==================================================================*/
Logger::Logger(const std::string& nm) : name(nm), level(LogLevel::DEBUG)
{
    formatter = new LogFormatter(GDRES_LOG_STYLE);
}

Logger::~Logger()
{
    delete formatter;
    ClearAppender();
}

/*==================================================================
 * FuncName : Logger::Log
 * description : 输出日志
 * author : guoshuaiwei  
 *==================================================================*/
void Logger::Log(LogLevel::Level lev, LogEvent* event)
{
    if(lev >= level){
        for(auto& appender : appenders){
            appender->Log(this, lev, event);
        }
    }
}

void Logger::Debug(LogEvent* event)
{
    Log(LogLevel::DEBUG, event);
}

void Logger::Info(LogEvent* event)
{
    Log(LogLevel::INFO, event);
}

void Logger::Error(LogEvent* event)
{
    Log(LogLevel::ERROR, event);
}

void Logger::Warn(LogEvent* event)
{
    Log(LogLevel::WARN, event);
}

void Logger::Event(LogEvent* event)
{
    Log(LogLevel::EVENT, event);
}

/*==================================================================
 * FuncName : Logger::AddAppender
 * description : 关于appender的操作
 * author : guoshuaiwei  
 *==================================================================*/
void Logger::AddAppender(LogAppender* appender)
{
    WrapLock lk(spLock);
    if(!appender->GetFormatter()) {
        WrapLock lck(appender->spLock);
        appender->formatter = formatter;
    }
    appenders.push_back(appender);
}

void Logger::DelAppender(LogAppender* appender)
{
    WrapLock lk(spLock);
    for(auto it = appenders.begin(); it != appenders.end(); ++it) {
        if(*it == appender) {
            delete appender;
            appenders.erase(it);
            break;
        }
    }
}

void Logger::ClearAppender()
{
    WrapLock lk(spLock);
    for (auto it : appenders) {
        delete it;
    }
    appenders.clear();
}

/*==================================================================
 * FuncName : Logger::GetFormatter
 * description : 返回格式器
 * author : guoshuaiwei  
 *==================================================================*/
LogFormatter* Logger::GetFormatter()
{
    WrapLock lk(spLock);
    return formatter;
}

/*==================================================================
 * FuncName : Logger::SetFormatter
 * description : 设置日志格式器
 * author : guoshuaiwei  
 *==================================================================*/
void Logger::SetFormatter(LogFormatter* val)
{
    WrapLock lk(spLock);
    LogFormatter* tmp = formatter;
    formatter = val;
    delete tmp;

    // 更新每一个输出器的格式
    for(auto& it : appenders) {
        WrapLock lck(it->spLock);
        if(!it->hasFormatter) {
            it->formatter = formatter;
        }
    }
}



//================================  class LogFormatter  ================================//
#pragma region
// 日志内容
class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetContent();
    }
};

// 日志级别
class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    LevelFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << LogLevel::ToString(level);
    }
};

// 启动至当前时间的耗时
class ElapseFormatItem : public LogFormatter::FormatItem
{
public:
    ElapseFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetElapse();
    }
};

// 日志器名称
class NameFormatItem : public LogFormatter::FormatItem
{
public:
    NameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetLogger()->GetName();
    }
};

// 线程ID
class ThreadIdFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetThreadId();
    }
};

// 协程ID
class FiberIdFormatItem : public LogFormatter::FormatItem
{
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetFiberId();
    }
};

// 线程名称
class ThreadNameFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetThreadName();
    }
};

// 日期
class DateTimeFormatItem : public LogFormatter::FormatItem
{
public:
    DateTimeFormatItem(const std::string& fmt = "%Y-%m-%d %H:%M:%S") : format(fmt)
    {
        if(format.empty()) {
            format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        struct tm tm;
        time_t time = event->GetTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), format.c_str(), &tm);
        os << buf;
    }
private:
    std::string format;
};

// cpp文件名
class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetFile();
    }
};

// 行号
class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << event->GetLine();
    }
};

// 换行
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << std::endl;
    }
};

// 制表符
class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << "\t";
    }
};

// 其他字符串内容
class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str) : string(str) {}
    void Format(std::ostream& os, Logger* logger, LogLevel::Level level, LogEvent* event) override {
        os << string;
    }
private:
    std::string string;
};
#pragma endregion
/*==================================================================
 * FuncName : LogFormatter::LogFormatter
 * description : 日志格式器
 * author : guoshuaiwei  
 *==================================================================*/
LogFormatter::LogFormatter(const std::string& pat) : pattern(pat)
{
    Init();
}

LogFormatter::~LogFormatter()
{
    for (auto &it : items) {
        delete it;
    }

    items.clear();
}

/*==================================================================
 * FuncName : LogFormatter::Init
 * description : 初始化  解析日志模板 
 *               默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 * author : guoshuaiwei  
 *==================================================================*/
void LogFormatter::Init()
{
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < pattern.size(); ++i) {
        if(pattern[i] != '%') {
            nstr.append(1, pattern[i]);
            continue;
        }

        if((i + 1) < pattern.size()) {
            if(pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < pattern.size()) {
            if(!fmt_status && (!isalpha(pattern[n]) && pattern[n] != '{'
                    && pattern[n] != '}')) {
                str = pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(pattern[n] == '{') {
                    str = pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(pattern[n] == '}') {
                    fmt = pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == pattern.size()) {
                if(str.empty()) {
                    str = pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << pattern << " - " << pattern.substr(i) << std::endl;
            isError = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem*(const std::string& str)>> s_format_items = {
        // {"m", [](const std::string& fmt) { return new MessageFormatItem(fmt);}}}
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return new C(fmt);}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            items.push_back(new StringFormatItem(std::get<0>(i)));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                items.push_back(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>"));
                isError = true;
            } else {
                items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
    //std::cout << m_items.size() << std::endl;
}

/*==================================================================
 * FuncName : LogFormatter::Format
 * description : 返回格式化日志文本
 * author : guoshuaiwei  
 *==================================================================*/
std::string LogFormatter::Format(Logger* logger, LogLevel::Level level, LogEvent* event)
{
    std::stringstream ss;
    for(auto& i : items) {
        i->Format(ss, logger, level, event);
    }
    return ss.str();
}

std::ostream& LogFormatter::Format(std::ostream& ofs, Logger* logger, LogLevel::Level level, LogEvent* event) {
    for(auto& i : items) {
        i->Format(ofs, logger, level, event);
    }
    return ofs;
}


//================================  class LogAppender  ================================//
/*==================================================================
 * FuncName : LogAppender::SetFormatter
 * description : 更改日志格式
 * author : guoshuaiwei  
 *==================================================================*/
void LogAppender::SetFormatter(LogFormatter* val)
{
    WrapLock lk(spLock);
    formatter = val;
}

LogFormatter* LogAppender::GetFormatter()
{
    WrapLock lk(spLock);
    return formatter;
}

void LogAppender::SetLevel(LogLevel::Level val)
{
    WrapLock lk(spLock);
    level = val;
}

LogLevel::Level LogAppender::GetLevel()
{
    WrapLock lk(spLock);
    return level;
}

//================================  class StdoutLogAppender  ================================//

/*==================================================================
 * FuncName : StdoutLogAppender::Log
 * description : 输出到控制台的输出器输出日志
 * author : guoshuaiwei  
 *==================================================================*/
void StdoutLogAppender::Log(Logger* logger, LogLevel::Level lev, LogEvent* eve)
{
    if (lev >= level) {
        spLock.lock();
        formatter->Format(std::cout, logger, lev, eve);
        spLock.unlock();
    }
}

//================================  class FileoutLogAppender  ================================//

/*==================================================================
 * FuncName : FileoutLogAppender::FileoutLogAppender
 * description : 输出到文件的输出器输出日志
 * author : guoshuaiwei  
 *==================================================================*/
FileoutLogAppender::FileoutLogAppender(const std::string& file) : LogAppender(), fileName(file), lastTime(0)
{
    ReOpen(); 
}

/*==================================================================
 * FuncName : FileoutLogAppender::Log
 * description : 输出到文件的输出器输出日志
 * author : guoshuaiwei  
 *==================================================================*/
void FileoutLogAppender::Log(Logger* logger, LogLevel::Level lev, LogEvent* eve)
{
    if(lev >= level) {
        uint64_t now = eve->GetTime();
        if(now >= (lastTime + 3)) { // 
            ReOpen();
            lastTime = now;
        }
        WrapLock lk(spLock);
        if(!formatter->Format(fileStream, logger, lev, eve)) {
            std::cout << "error" << std::endl;
        }
    }
}

/*==================================================================
 * FuncName : FileoutLogAppender::ReOpen
 * description : 重新打开日志文件
 * author : guoshuaiwei  
 *==================================================================*/
bool FileoutLogAppender::ReOpen()
{
    WrapLock lk(spLock);
    if(fileStream) {
        fileStream.close();
    }
    return WFile::OpenForWrite(fileStream, fileName, std::ios::app);
}


//================================  class LoggerSingle  ================================//
/*==================================================================
 * FuncName : LoggerSingle::LoggerSingle
 * description : 单例日志器构造
 * author : guoshuaiwei  
 *==================================================================*/
LoggerSingle::LoggerSingle()
{    
    logger = new Logger();
    stdoutAppender = new StdoutLogAppender();
    fileAppender = new FileoutLogAppender(LOG_FILE_PATH);
    logger->AddAppender(stdoutAppender);
    logger->AddAppender(fileAppender);
}

LoggerSingle* LoggerSingle::GetInstance()
{
    return logSig;
}

LoggerSingle* LoggerSingle::logSig = new LoggerSingle();


//================================  class LogEventWrap  ================================//
/*==================================================================
 * FuncName : LogEventWrap::LogEventWrap
 * description : 日志事件封装，便于宏定义日志函数
 * author : guoshuaiwei  
 *==================================================================*/
LogEventWrap::LogEventWrap(LogEvent* eve) : event(eve)
{    
}

LogEventWrap::~LogEventWrap()
{
    event->GetLogger()->Log(event->GetLevel(), event);
    // std::cout << "~LogEventWrap()" << std::endl;
    delete event;
    // std::cout << "~LogEventWrap() over" << std::endl;
}


std::stringstream& LogEventWrap::GetSS()
{
    return event->GetSS();
}









}
