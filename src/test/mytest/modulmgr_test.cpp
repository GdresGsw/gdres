#include "test.h"
#include <iostream>

using namespace gdres;
using namespace std;



void ModulMgrTest1()
{
    ModulMgr* mr = ModulMgr::GetThis();
    ModulTest1* test1 = ModulTest1::GetThis();
    ModulTest2* test2 = ModulTest2::GetThis();
    ModulTest3* test3 = ModulTest3::GetThis();

    getchar();
}



void ModulMgrTest()
{
    ModulMgrTest1();
}