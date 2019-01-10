#ifndef EXECUTEPROGRAMM_H
#define EXECUTEPROGRAMM_H

#include "icommand.h"
#include "gparcer/gcodeworker.h"
#include "gparcer/comdata.h"

#include <QTextStream>
//#include <QObject>
#include <QMutex>

class Producer;

class ExecuteProgramm : public QObject, public ICommand
{

    Q_OBJECT

public:
    explicit ExecuteProgramm(QObject *parent = nullptr);
    virtual ~ ExecuteProgramm() override {}
    virtual void execute() override;
    void execute(QFile &stream);


    ComData* getComdata() const{
        return comdata;
    }

    static QMutex exec_mutex;
    static QWaitCondition queueNotFull;
    static uint queueSize;
    static uint numaction;// amount actions in queue.
    static QQueue<mito::Action_t> actionQueue;


    static mito::Action_t* getAction(){
        exec_mutex.lock();
        ExecuteProgramm::action = actionQueue.dequeue();
        numaction--;
        queueNotFull.wakeAll();
        exec_mutex.unlock();
        return &ExecuteProgramm::action;
    }

public slots:
    void finished();


signals:
    void sg_executionFinished();

private:
    GcodeWorker *gcodeworker;

    ComData * comdata;


    Producer* producer;

   static mito::Action_t action;

};


class Producer: public QThread
{
    typedef ExecuteProgramm ThreadViser;


public:
//    QQueue<mito::Action_t> actionQueue;

    Producer(QObject* parent=nullptr):QThread(parent) {
        abort = false;
        restart = false;
    }

    ~Producer() override
    {
        abort = true;
//        QThread::~QThread();
    }

    void run() override {
        forever{

            ThreadViser::exec_mutex.lock();

            while(ThreadViser::numaction<ThreadViser::queueSize)
            {
                mito::Action_t* action = gcodeWorker->readCommandLine();
                actionQueue->enqueue(*action);
                ThreadViser::numaction++;
                if(action->a == eEOF){
                    abort = true;
                    break;
                }
            }

            qDebug()<<__FILE__<<__LINE__<<"numaction:"<<ThreadViser::numaction;

            if(abort == false)
                ThreadViser::queueNotFull.wait(&ThreadViser::exec_mutex);
            else{
                ThreadViser::exec_mutex.unlock();
                break;
            }

            ThreadViser::exec_mutex.unlock();

        }
    }

    void setActionQueue(QQueue<mito::Action_t>* value){actionQueue = value;}
    void setGcodeWorker(GcodeWorker* value){gcodeWorker = value;}

private:
    QQueue<mito::Action_t>* actionQueue;
    GcodeWorker* gcodeWorker;

    bool abort;
    bool restart;
};


#endif // EXECUTEPROGRAMM_H
