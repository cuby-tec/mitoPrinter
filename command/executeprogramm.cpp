#include "executeprogramm.h"
#include <QDebug>
#define cout	qDebug()<<__FILE__<<__LINE__

ExecuteProgramm::ExecuteProgramm(QObject *parent) : QObject(parent)
{
    producer = new Producer(this);
    producer->setActionQueue(&ExecuteProgramm::actionQueue);
    gcodeworker = nullptr;
    comdata = nullptr;
    numaction = 0;
    queueSize = 10;
}

ExecuteProgramm::~ExecuteProgramm()
{
    producer->quit();
    producer->wait(1000);

    delete  producer;
}

void ExecuteProgramm::execute()
{
    //TODO
    qDebug()<<__FILE__<<__LINE__<<"execute.";


}

void ExecuteProgramm::execute(QFile &stream)
{
    //--------- QFile check
    bool rd;
    QString line;
    char buf[1024];
    qint64 lineLength = stream.readLine(buf, sizeof(buf));
    volatile QString ln;

    if (lineLength != -1) {
        // the line is available in buf
        QString lnp = QString(buf);
    }

    rd = stream.seek(0);
    if(rd)
    {
        line = stream.readLine();
    }


     numaction = 0;

    if(gcodeworker == nullptr)
        gcodeworker = new GcodeWorker();

//    connect(gcodeworker, SIGNAL(sg_executeComplite()), this, SLOT(finished()) );
    gcodeworker->setFileExecute(stream);

    if(comdata == nullptr)
        comdata = new ComData();

    connect(comdata, SIGNAL(sg_executeComplite()), this, SLOT(finished()));
    producer->setGcodeWorker(gcodeworker);
    producer->setController(comdata->getController());
    producer->start();
    comdata->run(gcodeworker);

}

void ExecuteProgramm::finished()
{
    try {
        cout<<"finished";
        emit sg_executionFinished();
//        delete comdata;
//        delete gcodeworker;
    } catch (std::exception &e) {
        qFatal("Error ");

    }    catch (...) {
        qFatal("Error ");
    }


}

void Producer::setController(Controller *value)
{
    controller = value;
}

//---------- vars
QMutex ExecuteProgramm::exec_mutex;

QWaitCondition ExecuteProgramm::queueNotFull;

QWaitCondition ExecuteProgramm::queueNotEmpty;

uint ExecuteProgramm::queueSize = 10;

uint ExecuteProgramm::numaction;

QQueue<mito::Action_t> ExecuteProgramm::actionQueue;

mito::Action_t ExecuteProgramm::action;

