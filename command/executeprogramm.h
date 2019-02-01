#ifndef EXECUTEPROGRAMM_H
#define EXECUTEPROGRAMM_H

#include "icommand.h"
#include "gparcer/gcodeworker.h"
#include "gparcer/comdata.h"
//#include "scontrolblocks.h"
#include "optimization.h"

#include <QTextStream>
//#include <QObject>
#include <QMutex>

class Producer;

class ExecuteProgramm : public QObject, public ICommand
{

    Q_OBJECT

public:
    explicit ExecuteProgramm(QObject *parent = nullptr);
    virtual ~ ExecuteProgramm() override;// {}
    virtual void execute() override;
    void execute(QFile &stream);


    ComData* getComdata() const{
        return comdata;
    }

    static QMutex exec_mutex;
    static QWaitCondition queueNotFull;
    static QWaitCondition queueNotEmpty;
    static uint queueSize;
    static uint numaction;// amount actions in queue.
    static QQueue<mito::Action_t> actionQueue;


    static mito::Action_t* getAction(){
        exec_mutex.lock();
//        if(numaction == 0)
        if(actionQueue.isEmpty())
        {
            qDebug()<<__FILE__<<__LINE__<<"Waiting Queue.";
            queueNotEmpty.wait(&exec_mutex);
//            if(queueNotEmpty.wait(&exec_mutex,1000) == false){
//                qFatal("\tWaiting command timeout.");
//            }
        }
        exec_mutex.unlock();

        ExecuteProgramm::action = actionQueue.dequeue();
        qDebug()<<__FILE__<<__LINE__<<"\tnumaction:"<<numaction;
        exec_mutex.lock();
         numaction--;
        exec_mutex.unlock();
        queueNotFull.wakeAll();
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
/*
typedef block_state_t _controlBlocks[N_AXIS];
struct sControlBlocks{
//    block_state_t x_block;
//    block_state_t y_block;
//    block_state_t z_block;
//    block_state_t e_block;
    _controlBlocks bb;
};
*/
class Producer: public QThread
{
    typedef ExecuteProgramm ThreadViser;


public:
//    QQueue<mito::Action_t> actionQueue;

    Producer(QObject* parent=nullptr):QThread(parent) {
        abort = false;
        restart = false;
        current_index = 0;
        tag_action.index = 0;
        cord = Coordinatus::instance();
//        optimization = new Optimization;
    }

    ~Producer() override
    {
        abort = true;
//        QThread::~QThread();
        delete optimization;
    }

    void run() override {
        forever{


            while(ThreadViser::numaction < ThreadViser::queueSize)
            {
                producer_m1:
                mito::Action_t* action = gcodeWorker->readCommandLine();

                if(tag_action.index == 0){
                    tag_action = *action;
                    current_index = tag_action.index;
//                    actionQueue->enqueue(tag_action);
//                    Coordinatus* cord = Coordinatus::instance();
//                    sControlBlocks* blocks = new sControlBlocks;
                    memcpy(blocks.bb,cord->currentBlocks,sizeof (sControlBlocks));
                    controlblocks.enqueue(blocks);
                    goto producer_m1;
                }


                if((current_index == action->index)&&(current_index != 1))
                {
                    do {
                        tag_action.queue.enqueue(action->queue.dequeue());
                        memcpy(blocks.bb,cord->nextBlocks,sizeof (sControlBlocks));
                        controlblocks.enqueue(blocks);
                        do{
                            action = gcodeWorker->readCommandLine();
                        }while (action->a == eNext); //(action->queue.isEmpty());

                     }while(current_index == action->index);

                    //TODO optimization tag_action & controlblocks
                    optimization = new Optimization;
                    optimization->calc(tag_action, controlblocks);

                    delete  optimization;

                    controlblocks.clear();
                    actionQueue->enqueue(tag_action);
                    current_index = action->index;
                    tag_action = *action;
                }else{
                    controlblocks.clear();
                    actionQueue->enqueue(tag_action);
                    current_index = action->index;
                    tag_action = *action;
                    memcpy(blocks.bb,cord->nextBlocks,sizeof (sControlBlocks));
                    controlblocks.enqueue(blocks);

                    if(action->index==1)
                    {
                        actionQueue->enqueue(*action);
                    }
                }

                ThreadViser::exec_mutex.lock();
//                ThreadViser::numaction++;
                ThreadViser::numaction = static_cast<uint>(actionQueue->size());
                ThreadViser::queueNotEmpty.wakeAll();
                ThreadViser::exec_mutex.unlock();
                if(action->a == eEOF){
                    abort = true;
                    break;
                }
            }

            qDebug()<<__FILE__<<__LINE__<<"numaction:"<<ThreadViser::numaction;

            ThreadViser::exec_mutex.lock();
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

    uint current_index ;
    mito::Action_t tag_action;

    QQueue<sControlBlocks> controlblocks;
//    QList<controlBlocks*> list;
    sControlBlocks blocks;
    Coordinatus* cord;// = Coordinatus::instance();
    Optimization* optimization;


};


#endif // EXECUTEPROGRAMM_H
