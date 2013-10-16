#ifndef CONNECTION_H
#define CONNECTION_H

#include "config.h"
#include <iostream>

typedef enum
{
    SERVER, CLIENT, CA
}TypeOfEntity;

class Entity
{
    /**
     * @brief getDestinationPlace
     * @return the identificator for used connection type
     */
    virtual std::string getDestinationPlace() = 0;

    /**
     * @brief getEntityType
     * @return type of entity
     */
    virtual TypeOfEntity getEntityType() =0;


};

class Message
{

    /**
     * @brief getFormatedMessage
     * @return format of message for conneciton.
     */
    virtual std::string getFormatedMessage() = 0;

};

class Connection
{
public:

    /**
     * @brief initConnection Starts connection
     * @param e
     * @return
     */
    virtual int initConnection(Entity * e) = 0;

    /**
     * @brief closeConnection
     * @return
     */
    virtual int closeConnection() = 0;

    /**
     * @brief sentRequestToEntity
     * @param[in] toSent
     * @return 0 on success else fail code
     */
    virtual int sentRequestToEntity(Message * toSent) = 0;

    /**
     * @brief getResponseFromEntity
     * @param[out] recieved
     * @return 0 on success else fail code
     */
    virtual int getResponseFromEntity(Message * recieved) = 0;
};

#endif // CONNECTION_H
