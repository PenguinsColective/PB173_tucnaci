#include "connectiontest.h"
#include <exception>

int tests_run;

namespace tests
{

ConnectionTest::~ConnectionTest()
{
    for(std::list<Connection*>::iterator it = allocated.begin(); it != allocated.end();)
    {
        Connection * c = *it;
        it = allocated.erase(it);
        delete c;
    }
}

char * ConnectionTest::testCloseConnectionNotOpened()
{
    Connection * c = new ConnectivityMock();
    allocated.push_back(c);
    mu_assert("The test of not opened connection close failed",c->closeConnection()!=0);
    return NULL;

}

char * ConnectionTest::testCloseConnectionOpened()
{
    Connection * c = new ConnectivityMock();
    allocated.push_back(c);
    EntityMock e;
    c->initConnection(&e);
    mu_assert("Test close opened connection failed", c->closeConnection() == 0);
}



char * ConnectionTest::runTests()
{
    mu_run_test(testCloseConnectionOpened);
    mu_run_test(testCloseConnectionNotOpened);
}

}
