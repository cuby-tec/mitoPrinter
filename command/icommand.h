#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "exchange/threadarc.h"
#include "links/Status_t.h"

#include <QString>

class ICommand
{
public:
    ICommand();// {}
    virtual ~ICommand();
    virtual void execute(){}

    static Status_t* executeGCommand(QString command);
};

#endif // ICOMMAND_H
