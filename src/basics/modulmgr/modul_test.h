#ifndef _GDRES_MODUL_TEST_H_
#define _GDRES_MODUL_TEST_H_

#include "modul_base.h"

namespace gdres {


class ModulTest1 : public ModulBase
{
public:
    ModulTest1(uint32_t modID, const std::string& nm = "");
    ~ModulTest1() {Stop();}

    void ProcessMsg(GDRES_MSG*) override;
    void TimerEvent() override;

    static ModulTest1* GetThis();

private:
    std::atomic<uint64_t> msgCnt;
};


class ModulTest2 : public ModulBase
{
public:
    ModulTest2(uint32_t modID, const std::string& nm = "");
    ~ModulTest2() {Stop();}

    void ProcessMsg(GDRES_MSG*) override;
    void TimerEvent() override;

    static ModulTest2* GetThis();

private:
    std::atomic<uint64_t> msgCnt;
};


class ModulTest3 : public ModulBase
{
public:
    ModulTest3(uint32_t modID, const std::string& nm = "");
    ~ModulTest3() {Stop();}

    void ProcessMsg(GDRES_MSG*) override;
    void TimerEvent() override;

    static ModulTest3* GetThis();

private:
    std::atomic<uint64_t> msgCnt;
};



// 各测试模块支持的消息类型
enum TestMsgType {
    TEST1_MSG_TYPE1 = 0x01,
    TEST1_MSG_TYPE2,
    TEST1_MSG_TYPE3,

    TEST2_MSG_TYPE1,
    TEST2_MSG_TYPE2,
    TEST2_MSG_TYPE3,

    TEST3_MSG_TYPE1,
    TEST3_MSG_TYPE2,
    TEST3_MSG_TYPE3,
};





















}


#endif
