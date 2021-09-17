#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <algorithm> // sort
#include <stack>

#include <unordered_map>

using namespace std;


// unordered_map练习
void HashStudyTest1()
{
    unordered_map<string, int> umap;

    umap.insert(pair<string, int>("wang", 1)); // 插入键值对
    umap["3"] = 3; // 写入键值    

    int ret = umap.erase("13"); // 根据键值删除键值对，没有找到返回0

    auto iter = umap.find("3"); // 根据键查找,算法复杂度常数
    if (iter != umap.end()) {
        iter->second = 5;
    }

    cout << iter->first << endl;
    cout << umap["3"] << endl;
}

// unordered_set练习
void SetTest1()
{
    class A 
    {
    public:
        A() {cout << " is A " << endl;}
        virtual ~A() {cout << "cloase A" << endl;}
        virtual void C() { cout << " func C from A" << endl;}
    };

    class B : public A
    {
    public:
        B() {cout << " is B " << endl;}
        ~B() {cout << "cloase B" << endl;}
        void C() { cout << " func C from B" << endl;}
    };

    A* b;
    b = new B();

    b->C();


    delete b;

}




void StudyTest1()
{
    SetTest1();
}
