#include "waittemperature.h"
#include "myglobal.h"
#include "gparcer/gcodeworker.h"

#include "links/eModelstate.h"
#include <string.h>
#include <QtConcurrent/QtConcurrent>

//-------------- defs
#define cout qDebug()<<__FILE__<<__LINE__

//#define FSMCOUNTER  10

//------ function

WaitTemperature::WaitTemperature(QObject *parent, double_t temperature) : QObject(parent)
{
    targetTemperature = temperature;
    state = e_waitQueue;
    timer = new QTimer(this);
    pd = nullptr;
    connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
}

void WaitTemperature::execute()
{
    connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));
    timer->start(1000);
    statusLoader.setFuture(QtConcurrent::run(WaitTemperature::_checkStatus));
}

void WaitTemperature::statusLoaded()
{
    Status_t* st = statusLoader.result();
//    Status_t* sttmp;
    QString str;
    cout<<"segments:"<<st->modelState.queueState<<"\tstat:"<<state;

    switch (state) {
    case e_waitQueue:
        if((st->modelState.queueState == SEGMENT_QUEE_SIZE)
                &&(st->modelState.modelState != ehIwork)){
            // We can send a Gcommand M109 Sxxx
            str = QString("M109 S%1").arg(targetTemperature);
//            sttmp = executeGCommand(str);
            statusLoader.setFuture(QtConcurrent::run(WaitTemperature::executeGCommand,str));
            state = e_waitTemperature;
            pd = new QProgressDialog("Heating in progress.", "Cancel", 0, 100);
//            pd->setRange(0, FSMCOUNTER );
            pd->setRange(0, static_cast<int>(targetTemperature) );
            pd->setMinimumDuration(0);
            connect(pd,SIGNAL(canceled()), this, SLOT(heatingCancel()) );
        }
//        else{
//            statusLoader.setFuture(QtConcurrent::run(WaitTemperature::_checkStatus));
//        }

        break;

    case e_sendCommand:

        break;

    case e_waitTemperature:
        if(static_cast<double_t>(st->temperature) >= targetTemperature)
        {    cout<<"Extruder ready.";
            pd->setValue(static_cast<int>(targetTemperature));
            timer->stop();
            emit sg_commandDone();
        }else{
            pd->setValue(static_cast<int>(st->temperature));
//            statusLoader.setFuture(QtConcurrent::run(WaitTemperature::_checkStatus));
        }
        break;
    }


}

void WaitTemperature::failedStatus()
{
    //TODO
}

void WaitTemperature::checkStatus()
{
    //TODO
    statusLoader.setFuture(QtConcurrent::run(WaitTemperature::_checkStatus));
}

void WaitTemperature::heatingCancel()
{
    //TODO
    cout<<"heating Canceled";
    pd->cancel();
    timer->stop();
    emit sg_canceled();
}
