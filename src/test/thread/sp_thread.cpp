#include <iostream>
#include <thread>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sp_thread.h>
#include <vector>
#include <mutex>
#include <atomic>// 原子操作头文件
#include <ctime> // 计时器
#include <condition_variable>
#include <functional>

using namespace std;

bool isExit = false;
// 子线程
void ThreadMain1(int arv, const char* prt, int& refPara)
{   
    // 获取线程ID
    auto thID = this_thread::get_id();
    cout << "the child thread ID is :" << thID << endl;
    cout << arv << endl;
    cout << prt << endl;
    cout << refPara++ << endl;
    // 延时
    int i = 0;
    while (!isExit) {
        cout << "i is :" << i++ << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "child thread exit..." << endl;    
}

// 使用模板
void TemplatePrc()
{
    // 传指针时要注意指向的空间被提前释放
    int a = 10;
    char test[] = "my name is XiaoMing";
    thread t1(ThreadMain1, 50, test, ref(a)); // 此处子线程启动 传参是进行拷贝
    // cout << "the main thread ID is :" << this_thread::get_id() << endl; // 存在控制台输出竞争

    this_thread::sleep_for(chrono::seconds(5)); // 主线程执行任务,同时子线程执行任务

    isExit = true; // 主线程退出时，通知子线程退出

    t1.join(); // 主线程等子线程退出后再退出
    // t1.detach(); // 主线程和子线程互不影响，使用中问题较多，多调用join

    cout << a << endl;

    cout << "the main thread exit..." << endl;

    getchar();
}

// 类成员函数作为子线程入口函数
void ThreadClassTest()
{
    MyThread myth;
    myth.name = "xiao ming";
    myth.age = 20;

    thread t1(&MyThread::Main, &myth); // 类的成员函数作为子线程的入口
    t1.join();
}

// 封装测试 自动锁测试
static mutex mux;
void ThreadClassTest2()
{    
    // XMutex lock(mux); // 对该函数自动加锁和解锁
    lock_guard<mutex> lg(mux); // 系统自带自动锁
    TestThread tth;
    tth.Start(); // 启动子线程任务
    this_thread::sleep_for(chrono::seconds(5)); // 执行主线程任务
    tth.Stop(); // 结束子线程
    cout << "the main thread exit..." << endl;
}

// 创建线程数组
void ThreadClassTest3()
{
    thread* t[10];
    int a = 10;
    for (int i = 0; i < 10; i++) {
        t[i] = new thread(ThreadMain1, 50, "test", ref(a));
    }

    this_thread::sleep_for(chrono::seconds(5)); // 主线程执行任务,同时子线程执行任务

    isExit = true;

    for (int i = 0; i < 10; i++) {
        t[i]->join();
    }

    getchar();

}

// 原子操作测试
atomic<int> sum(0); // sum自动加解锁
void Th1()
{
    for (int i = 0; i < 10000000; i++) {
        sum++;
    }
    
}

void ThreadClassTest4()
{
    clock_t start_time = clock(); // c++自带毫秒计时器

    int thNum = 10;
    vector<thread> ths;
    for (int i = 0; i < thNum; i++) {
        ths.push_back(thread(Th1));
    }
    for (auto &iter : ths) {
        iter.join();
    }

    cout << sum << endl;

    clock_t end_time = clock();
	std::cout << "use time: " << end_time - start_time << "ms" << std::endl;
}



// 迭代器测试
void IterTest()
{
    vector<int> list;
    for (int i = 0; i < 5; i++) {
        list.push_back(i);
    }

    for (int k = 0; k < list.size() - 1; k++) {
        if (list[k] == 2) {
            auto iter = list.begin() + k;
            list.erase(iter);
        }
    }

    for (auto l : list) {
        cout << l << endl;
    }
}

/*----------- thread 练习 ----------- */ 

struct Person {
    int age;
    char name[10];
};

void Callbackp(void* arg)
{
    
    Person* p = (Person*)arg;
    for (int i = 0; i < 5; i++) {
        cout << i << endl;
    }
    
    p->age = 10;
    strcpy(p->name, "xiaohua");
    cout << "the child thread id:" << std::this_thread::get_id() << endl;

}

int Test1()
{
    Person* pr =  new Person();
    std::thread t(Callbackp, pr);
    cout << "the main thread id:" << std::this_thread::get_id() << endl;
    cout << "the child thread id is:" << t.get_id() << endl;
    cout << "joinable:" << t.joinable() << endl;

    t.join();
    this_thread::sleep_for(chrono::seconds(5)); // 主线程执行任务,同时子线程执行任务
    cout << pr->age << endl;
    cout << pr->name << endl;


    return 0;
}

/*----------------生产者消费者练习-----------------------*/


// Node* head = nullptr;
// mutex mtx;
// condition_variable cv;
// bool exitThred = false;

void MyTest::Producer()
{
    cout << "Producer thread start." << endl;
    while (!exitThred) {
        std::unique_lock<std::mutex> lck(mtx);
        Node* node = new Node();
        node->num = rand() % 1000;        
        node->next = head;
        head = node;
        cout << "thread id: " << this_thread::get_id();
        cout << "   producer num :" << head->num << endl;
        lck.unlock();
        cv.notify_all();                
        sleep(2);
    }

    cout << "producer thread exit." << endl;
}

void MyTest::Consumer()
{
    cout << "Consumer thread start." << endl;
    while (!exitThred) {
        std::unique_lock<std::mutex> lck(mtx);
        while (head == nullptr) {
            cout << "consumer wait" << endl;
            cv.wait(lck);
            if (exitThred) {
                cout << "Consumer thread exit." << endl;
                return;
            }

        }
        Node* node = head;
        head = head->next;
        cout << "thread id: " << this_thread::get_id();
        cout << "   consumer num :" << node->num << endl;
        delete node;
        sleep(rand() % 3);
    }
    cout << "Consumer thread exit." << endl;
}


void MyTest::Start()
{
    proTh = thread(&MyTest::Producer, this);
    consTh = thread(&MyTest::Consumer, this);    
}

void MyTest::End()
{
    exitThred = true;
    cv.notify_all();
    proTh.join();
    consTh.join();
    cout << "main thread exit" << endl;
}

void Test3()
{
    MyTest test;
    test.Start();
    sleep(20);
    test.End();
    
    // exitThred = true;

}

void Wtest()
{
    sleep(1);
    cout << "thread id : " << this_thread::get_id() << endl;
    return;
}
void Test4()
{
    thread* th = new thread(Wtest);
    sleep(5);
    
    cout << "thread id is :" << th->get_id() << endl;
    th->join();
    

}





class A
{
public:
    A(int b1) : a(b1) {}
    void Ptest()
    {
        b = B(this);
        b.Print();
        cout << "a is " << a << endl;
    }
private:
    int a;
    class B
    {
    public:
        B() : ac(nullptr) {}
        B(A* at) : ac(at) {}
        void Print() {cout << "a is " << ac->a++ << endl;}
    private:
        A* ac;
    };
    B b;
};


void Test7()
{
    A a(100);
    a.Ptest();
}










class test36
{
public:
    test36() {cout << "creat" << endl;}
    ~test36() {cout << "...clear" << endl;}
private:
    int a;
};

class test216
{
public:
    test216() : m(0) {}
    ~test216() {}
private:
    test36 t;
    int m;
};

void Test5()
{

    test216 nmk();

    
}

class WThreadBase
{
public:
    WThreadBase() : isExit(false), isEnd(false) {}
    virtual ~WThreadBase() {};
    virtual void Start()
    {
        th = std::thread(&WThreadBase::ThMain, this);
    }
    virtual void Stop()
    {
        isExit = true;
        if (th.joinable()) {
            th.join();
        }
        std::cout << "thread exit" << std::endl;
    }

    virtual bool GetStatus() {return isEnd;}
    

protected:
    virtual void ThMain() = 0; // 实现子线程业务逻辑的主函数，子类必须实现

    std::thread th;
    bool isExit;   // 控制线程退出标志，在ThMain函数运行条件中进行判断
    bool isEnd;    // 线程执行状态标志，在ThMain函数退出前置为true 
};

class WorkThread : public WThreadBase
{
public:
    WorkThread() : WThreadBase(), pool(0) {}
    ~WorkThread() {}
private:
    virtual void ThMain() override
    {

    }
    int pool;
};

class  te
{
public:
    te(int ac, int bc) : a(ac), b(bc) {}

    int a;
    int b;
};


void wor()
{
    vector<te> ts;
    ts.emplace_back(2, 3);

    cout << ts.size() << endl;
    cout << ts[0].a << endl;
    
}


void ThreadTest1()
{
    
}
