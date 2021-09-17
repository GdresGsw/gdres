#ifndef SP_THREAD_H
#define SP_THREAD_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>// 原子操作头文件
#include <ctime> // 计时器
#include <condition_variable>


using namespace std;


void ThreadTest1();
void ThreadTest2();


class MyThread
{
public:
    // 入口线程函数
    void Main()
    {
        cout << name << "  " << age << endl;
    }

    string name;
    int age;
};
// 自动锁
class XMutex
{
public:
    XMutex(mutex& mux) : mux(mux)
    {
        mux.lock();
    }
    ~XMutex()
    {
        mux.unlock();
    }
private:
    mutex& mux;
};



// 封装线程
class XThread
{
public:
    virtual void Start()
    {
        isExit = false;
        th = std::thread(&XThread::ThMain, this); // 启动线程
    }
    virtual void Stop()
    {
        isExit = true;
        Wait(); // 保证子线程结束后再关闭主线程
    }
    virtual void Wait()
    {
        if (th.joinable()) {
            th.join();
        }
    }
    bool GetIsExit() {return isExit;}

private:
    virtual void ThMain() = 0; // 实现子线程业务逻辑的主函数，子类必须实现
    std::thread th;
    bool isExit;
};





// 使用测试
class TestThread : public XThread
{
public:
    void ThMain() override
    {
        while (!GetIsExit()) {
            this_thread::sleep_for(chrono::seconds(1));
            cout << 1 << endl;
        }

        cout << "\nchild thread exit..." << endl; 
    }

    string name;
};

struct Node {
    int num;
    Node* next;
};

class MyTest
{
public:
    MyTest() : head(nullptr), exitThred(false) {}
    ~MyTest() {};

    void Producer();
    void Consumer();

    void Start();
    void End();

private:
    Node* head;
    mutex mtx;
    condition_variable cv;
    bool exitThred;
    thread proTh;
    thread consTh;
};
















#endif


