#ifndef MESSAGER_H
#define MESSAGER_H

#include "links/Status_t.h"

#include <QObject>

class Messager : public QObject
{
    Q_OBJECT
public:
//    explicit Messager(QObject *parent = nullptr);

    static Messager* instance()
    {
        static Messager p;
        return &p;
    }

signals:
    void sg_Message(QString msg);
    void sg_executeComplite();
    void sg_statusChanged(const Status_t* status);

public slots:
    void putMessage(QString msg){
        message = msg;
        emit sg_Message(message);
    }

    void setProgramExecutionComplite(){
        emit sg_executeComplite();
    }
    void putStatus(const Status_t* status){
        emit sg_statusChanged(status);
    }

private:

    QString message;

    Messager(QObject *parent = nullptr)
    : QObject(parent)
    {

    }
    ~Messager() {}
    Messager(Messager const&) = delete;
    void operator = (Messager const&) = delete;

};

#endif // MESSAGER_H
