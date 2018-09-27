#ifndef MESSAGER_H
#define MESSAGER_H

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

public slots:
    void putMessage(QString msg){
        message = msg;
        emit sg_Message(message);
    }

    void setProgramExecutionComplite(){
        emit sg_executeComplite();
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
