#include "testmanager.h"
#include <iostream>
#include "connectiontest.h"
using namespace std;



namespace tests {

TestManager::~TestManager()
{

    for(list<TestCase*>::iterator it = tests.begin(); it != tests.end();)
    {
        TestCase * t = *it;
        it = tests.erase(it);
        delete t;

    }
}

void TestManager::prepare()
{
    //Add here new offsprings of TestCase
    //F.E. tests.push_back(new ConnectionTest());
    //these objects are dealocated in destructor

    tests.push_back(new ConnectionTest());
}
int tests_run;
void TestManager::startTesting()
{
    const char * result = NULL;
    for(list<TestCase*>::iterator it = tests.begin(); it != tests.end(); ++it)
    {
        result = (*it)->runTests();
        if(result == NULL) continue;
        cerr << "The test no. " << tests_run << " failed: " << result <<endl;
        break;
    }
    cout << "Number of passed tests: " << tests_run << endl;
    cout << "Result of testing: " << ((result==NULL)? "fine" : "failed") << endl;
}

}
