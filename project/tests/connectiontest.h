#ifndef CONNECTIONTEST_H
#define CONNECTIONTEST_H
#include "testmanager.h"
#include "../connection.h"

namespace tests{



class ConnectionTest
        : public TestCase
{

    class NotImplementedException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "This method is not implemented yet";
        }
    };


    class ConnectivityMock :
            public Connection
    {
    public:
        virtual int initConnection(Entity *e)
        {
            throw new NotImplementedException;
        }

        virtual int closeConnection()
        {
            throw new NotImplementedException;
        }

        virtual int sentRequestToEntity(Message *toSent)
        {
            throw new NotImplementedException;
        }

        virtual int getResponseFromEntity(Message *recieved)
        {
            throw new NotImplementedException;
        }
    };

    class EntityMock: public Entity
    {
        virtual std::string getDestinationPlace()
        {
            throw new NotImplementedException;
        }

        virtual TypeOfEntity getEntityType()
        {
            throw new NotImplementedException;
        }
    };

public:

    virtual ~ConnectionTest();
    virtual char *runTests();

private:



    char * testCloseConnectionOpened();

    char * testCloseConnectionNotOpened();

    std::list<Connection*> allocated;
};
}
#endif // CONNECTIONTEST_H
