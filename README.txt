Application with CSS:
$ qt_application -stylesheet style.qss

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/mito

/* Create request sequence */
1. [mito::Action_t* action = nullptr;]
2. [lexer( new Lexer(dst))] ; where dst is [sGcode* dst = &gcode;]    [sGcode gcode;]
3. [result = lexer->parcer(line);] where line is [QString line;] with \n as end of string and [result>0]
4. [action = GcodeWorker::buildAction(dst);]
5. [action->a == eSend]
6. 	[ComDataReq_t req = action->queue.dequeue();]
7.	[req.requestNumber = ++MyGlobal::requestIndex;]
8.	[queueSize = threadarc.putInArray(&req);]

9.    [connect(&threadarc,SIGNAL(sg_status_updated(const Status_t*)),this,SLOT(updateStatus(const Status_t*)));]
10.   [connect(&threadarc,SIGNAL(sg_failed_status()),this,SLOT(failedStatus()) );]

11. [threadarc.process();]
12. [updateStatus(const Status_t *status)]    

/// Template
//.h
#include <QFutureWatcher>
#include <QTimer>

QFutureWatcher<Status_t*> statusLoader;

//.cpp
#include <QtConcurrent/QtConcurrent>
connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));

connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));

timer->start(1000);



============
class LTop
{
public:
    virtual void notInMid() const
    {
        qDebug("Method LTop::notInMode()");
    }
    virtual void allDo() const
    {
        qDebug("Method LTop::allDo()");
    }
};
class LMid : public LTop
{
    typedef LTop Base;
public:
    virtual void allDo() const
    {
        Base::allDo();
        qDebug("Method LMid::allDo()");
    }
};
class LBottom : public LMid
{
    typedef LMid Base;
public:
    virtual void notInMid() const
    {
        //Cannot make this call because it does not exist
        //Base::notInMid();
        //Only works if there is public inheritance.
        LTop::notInMid();
        qDebug("Method LBottom::notInMode()");
    }
    virtual void allDo() const
    {
        //this->LMid::allDo();
        Base::allDo();
        qDebug("Method LBottom::allDo()");
    }
};
=============
