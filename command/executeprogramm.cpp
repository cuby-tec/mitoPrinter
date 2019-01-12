#include "executeprogramm.h"
#include <QDebug>
#define cout	qDebug()<<__FILE__<<__LINE__

ExecuteProgramm::ExecuteProgramm(QObject *parent) : QObject(parent)
{
    producer = new Producer(this);
    producer->setActionQueue(&ExecuteProgramm::actionQueue);
}

void ExecuteProgramm::execute()
{
    //TODO
    qDebug()<<__FILE__<<__LINE__<<"execute.";


}

void ExecuteProgramm::execute(QFile &stream)
{
    gcodeworker = new GcodeWorker();

    connect(gcodeworker, SIGNAL(sg_executeComplite()), this, SLOT(finished()) );
    gcodeworker->setFileExecute(stream);


    comdata = new ComData();
//    connect(comdata, SIGNAL(sg_executeComplite()), this, SLOT(finished()));
    producer->setGcodeWorker(gcodeworker);
    producer->start();
    comdata->run(gcodeworker);

}

void ExecuteProgramm::finished()
{
    cout<<"finished";
    emit sg_executionFinished();
}

QMutex ExecuteProgramm::exec_mutex;

QWaitCondition ExecuteProgramm::queueNotFull;

QWaitCondition ExecuteProgramm::queueNotEmpty;

uint ExecuteProgramm::queueSize = 10;

uint ExecuteProgramm::numaction;

QQueue<mito::Action_t> ExecuteProgramm::actionQueue;

mito::Action_t ExecuteProgramm::action;
