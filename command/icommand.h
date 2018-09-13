#ifndef ICOMMAND_H
#define ICOMMAND_H


class ICommand
{
public:
    ICommand();// {}
    virtual ~ICommand();
    virtual void execute(){}
};

#endif // ICOMMAND_H
