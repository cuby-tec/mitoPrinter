#include "executeprogramm.h"
#include <QDebug>
#define cout	qDebug()<<__FILE__<<__LINE__

ExecuteProgramm::ExecuteProgramm(QObject *parent) : QObject(parent)
{

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
    comdata->run(gcodeworker);

}

void ExecuteProgramm::finished()
{
    cout<<"finished";
    emit sg_executionFinished();
}
