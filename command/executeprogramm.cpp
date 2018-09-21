#include "executeprogramm.h"
#include <QDebug>

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
    comdata->run(gcodeworker);

}

void ExecuteProgramm::finished()
{
    emit sg_executionFinished();
}
