#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <string.h>
#include <unistd.h>
#include <thread_pool.h>
#include <thread_pool.cpp>
#include <safe_queue.h>
#include <future>
#include <functional>

using namespace std;

#pragma region
// struct Person {
//     int age;
//     char name[10];
// };

// void* Callbackp(void* arg)
// {
//     Person* p = new Person;
//     for (int i = 0; i < 5; i++) {
//         cout << i << endl;
//     }
    
//     p->age = 10;
//     strcpy(p->name, "xiaohua");
//     cout << "子线程id：" << pthread_self() << endl;
//     pthread_exit(p);  // 将p指针从子线程返回
// }

// int Test1()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, Callbackp, nullptr);
//     cout << "主线程id：" << pthread_self() << endl;

//     void* p = nullptr;
//     pthread_join(tid, &p); // 用一个指针接收子线程返回的指针

//     Person* pr =  (Person*)p;

//     cout << pr->age << endl;
//     cout << pr->name << endl;


//     return 0;
// }

// // 线程数组练习，读写锁
// int count = 0;
// pthread_rwlock_t rwlock;
// void* WriteTh(void* arg)
// {
//     for (int i = 0; i < 30; i++) {
//         pthread_rwlock_wrlock(&rwlock);
//         count++;
//         cout << "write thread " << pthread_self() << ":" << count << endl;
//         pthread_rwlock_unlock(&rwlock);
//     }
//     return nullptr;
// }

// void* ReadTh(void* arg)
// {

//     pthread_rwlock_rdlock(&rwlock);
//     cout << "read thread " << pthread_self() << ":" << count << endl;
//     pthread_rwlock_unlock(&rwlock);
//     usleep(rand() % 5);

//     return nullptr;
// }

// void Test2()
// {
//     pthread_t wid[3], rid[5];
    
//     pthread_rwlock_init(&rwlock, nullptr);
//     int i = 0;
//     for (i = 0; i < 3; i++) {
//         pthread_create(&wid[i], nullptr, WriteTh, nullptr);
//     }
//     for (i = 0; i < 5; i++) {
//         pthread_create(&rid[i], nullptr, ReadTh, nullptr);
//     }

//     for (i = 0; i < 3; i++) {
//         pthread_join(wid[i], nullptr);
//     }
//     for (i = 0; i < 5; i++) {
//         pthread_join(rid[i], nullptr);
//     }

//     pthread_rwlock_destroy(&rwlock);

// }

// // ==========================生产者消费者模式练习================================
// pthread_cond_t cond;
// pthread_mutex_t mutx;

// struct Node {
//     int num;
//     Node* next;
// };

// Node* head = nullptr;

// // 生产者不断生产链表节点
// // 如果生产者也需要条件唤醒，则需要再创建一个条件变量
// void* Producer(void* arg)
// {
//     while (1) {
//         // 此处需要互斥锁
//         pthread_mutex_lock(&mutx);   // 加锁
//         Node* node = new Node;
//         node->num = rand() % 1000;
//         node->next = head;
//         head = node;
//         cout << "生产者：" << pthread_self() << "   num: " << head->num << endl;
//         pthread_mutex_unlock(&mutx);  // 解锁
//         pthread_cond_broadcast(&cond); // 唤醒被阻塞的消费者线程，唤醒后的消费者线程会抢互斥锁，抢成功的会继续运行
//         sleep(rand() % 3);
//     }

//     return NULL;
// }

// // 消费者不断消费节点
// void* Consumer(void* arg)
// {
//     while (1) {
//         pthread_mutex_lock(&mutx);  // 加锁
//         while (head == nullptr) {  // 如果head为空，则消费者线程被阻塞，等待生产者线程来唤醒
//             pthread_cond_wait(&cond, &mutx);   // 此处在进行线程休眠时会自动将互斥锁解锁，所以不会进行死锁
//         }
//         cout << "消费者：" << pthread_self() << "   num: " << head->num << endl;
//         Node* node = head;
//         head = head->next;
//         delete node;
//         pthread_mutex_unlock(&mutx);
//         sleep(rand() % 3);
//     }

//     return nullptr;
// }

// void Test3()
// {
//     pthread_mutex_init(&mutx, nullptr);
//     pthread_cond_init(&cond, nullptr);

//     // 创建3个生产者线程和3个消费者线程
//     pthread_t ptid[3], ctid[3];
//     for (int i = 0; i < 3; i++) {
//         pthread_create(&ptid[i], nullptr, Producer, nullptr);
//         pthread_create(&ctid[i], nullptr, Consumer, nullptr);
//     }

//     for (int j = 0; j < 3; j++) {
//         pthread_join(ptid[j], nullptr);
//         pthread_join(ctid[j], nullptr);
//     }

//     pthread_mutex_destroy(&mutx);
//     pthread_cond_destroy(&cond);
// }

// // =====================信号量实现生产者和消费者模型==============================
// sem_t semp;  // 生产者信号量
// sem_t semc;  // 消费者信号量
// void* ProducerS(void* arg)
// {
//     while (1) {
//         // 此处需要互斥锁
//         sem_wait(&semp); // 判断是否有资源，如果没有则阻塞
//         pthread_mutex_lock(&mutx);   // 加锁必须在semwait之后
//         Node* node = new Node;
//         node->num = rand() % 1000;
//         node->next = head;
//         head = node;
//         cout << "生产者：" << pthread_self() << "   num: " << head->num << endl;
//         pthread_mutex_unlock(&mutx);  // 解锁
//         sem_post(&semc); // 告诉消费者有新的资源可被消费
//         sleep(rand() % 3);
//     }

//     return NULL;
// }

// // 消费者不断消费节点
// void* ConsumerS(void* arg)
// {
//     while (1) {
//         sem_wait(&semc);
//         pthread_mutex_lock(&mutx);  // 加锁
//         cout << "       消费者：" << pthread_self() << "   num: " << head->num << endl;
//         Node* node = head;
//         head = head->next;
//         delete node;
//         pthread_mutex_unlock(&mutx);
//         sem_post(&semp);
//         sleep(rand() % 3);
//     }

//     return nullptr;
// }




// void Test4()
// {
//     // 生产者资源数初始化为5 ，如果为1，则只能有一个线程运行，不存在资源共享的问题，不需要加锁
//     // 如果大于1，则需要加锁
//     sem_init(&semp, 0, 5);  
//     sem_init(&semc, 0, 0);  // 消费者资源数初始化为0
//     pthread_mutex_init(&mutx, NULL);

//     // 创建3个生产者线程和3个消费者线程
//     pthread_t ptid[3], ctid[3];
//     for (int i = 0; i < 3; i++) {
//         pthread_create(&ptid[i], nullptr, ProducerS, nullptr);
//         pthread_create(&ctid[i], nullptr, ConsumerS, nullptr);
//     }

//     for (int j = 0; j < 3; j++) {
//         pthread_join(ptid[j], nullptr);
//         pthread_join(ctid[j], nullptr);
//     }

//     pthread_mutex_destroy(&mutx);
//     sem_destroy(&semp);
//     sem_destroy(&semc);
// }


// /* --------------线程池测试-----------------*/
// int testNum = 0;
// void Count(void* arg)
// {
//     int num = *(int*)arg;
//     printf("thread %ld is Working, number = %d\n", pthread_self(), num);
//     sleep(1);
// }
// void Test5()
// {
//     ThreadPoolp* pool = ThreadPoolCreat(3, 10, 1000);
//     for (int i = 0; i < 100; i++) {
//         int* num = new int;
//         *num = i;
//         ThreadPoolAdd(pool, Count, num);
//     }

//     sleep(20);

//     ThreadPoolDestroy(pool);
//     return;
// }
#pragma endregion
/* --------------线程池对象测试-----------------*/
void test7()
{
    SafeQueue<int> sq;
    for (int i = 0; i < 10; i++) {
        sq.push(i);
    }

    int b;
    for (int i = 0; i < 10; i++) {
        sq.take(b);
        cout << b << endl;
    }

    sleep(20);
}

struct mytest
{
    int a;
    int b;
};


void func(int arg, mytest &my)
{    
    int sum = arg + my.a + my.b;
    // printf("thread %ld is Working, number = %d\n", this_thread::get_id(), sum);
    sleep(1);
}
void Test6()
{
    ThreadPool* pool = new ThreadPool(8);
    pool->Init();
    mytest mt = {2, 3};
    for (int i = 0; i < 100; i++) {
        pool->AddTask(func, i, mt);
    }
    sleep(15);
    cout << "time is ok" << endl;
    delete pool;
    cout << "-------------exit--------------" << endl;
    getchar();
    return;
}

class MyTest
{
public:
    MyTest() : num(0), pool(new ThreadPool(3)) {}
    ~MyTest() {}

    void start()
    {
        pool->Init();
        for (int i = 0; i < 10; i++) {
            pool->AddTask(std::mem_fn(&MyTest::Test), this, 2);
        }        
        sleep(2);
        cout << "num is : " << num << endl;
        delete pool;
    }

private:
    void Test(int a)
    {
        mtx.lock();
        num += a;
        mtx.unlock();
    }

    int num;
    mutex mtx;
    ThreadPool* pool;
};


void Test16()
{
    MyTest t;
    t.start();
    getchar();
}







int main()
{
    Test16();
}