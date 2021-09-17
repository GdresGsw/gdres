#include <stdio.h>
#include <iostream>
#include <study_practice.h>
#include <vector>

using namespace std;


class A
{
public:
    A() {}
    void add() {a++;}

    static int a;
};
int A::a = 0;

int main()
{
    
    vector<int> ls;
    vector<int> b;
    for (int i = 0; i < 10; i++) {
        ls.emplace_back(i);
    }

    for (auto it : ls) {
        b.emplace_back(it);
    }

    for (auto &i : b) {
        cout << i << endl;
    }
    
    
    return 0;
}