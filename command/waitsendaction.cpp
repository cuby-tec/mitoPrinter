#include "waitsendaction.h"
#include "myglobal.h"
#include "gparcer/gcodeworker.h"

#include "links/eModelstate.h"
#include <string.h>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#define cout qDebug()<<__FILE__<<__LINE__


WaitSendAction::WaitSendAction(QObject *parent, mito::Action_t* action) : QObject(parent)
{
    this->action = action;
    timer = new QTimer(this);
    state = ews_waitQueue;
    connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
    segment_number = 0;
}

void
WaitSendAction::execute()
{
    //TODO
    connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));
    timer->start(500);
    statusLoader.setFuture(QtConcurrent::run(WaitSendAction::_checkStatus));
}

Status_t*
WaitSendAction::_sendAction(mito::Action_t *action)
{
    Status_t* status = new Status_t;

    ComDataReq_t req = action->queue.dequeue();
    req.requestNumber = ++MyGlobal::requestIndex;
    //2. Send request.
    UsbExchange* exch = new UsbExchange;
    int result_exch;
    thermo_gmutex.lock();
    result_exch = exch->sendRequest(&req);
    if(result_exch == EXIT_SUCCESS){
        memcpy(status,exch->getStatus(),sizeof(Status_t));
    }
    thermo_gmutex.unlock();
    return status;
}

void WaitSendAction::statusLoaded()
{
    //TODO
    Status_t* st = statusLoader.result();
    uint32_t number = st->frameNumber;
    uint32_t req_number = MyGlobal::requestIndex;
    switch (state) {
    case ews_waitQueue:
        if((st->modelState.queueState == SEGMENT_QUEE_SIZE)
                &&(st->modelState.modelState != ehIwork)){
            cout<<"queueState:"<<st->modelState.queueState<<"\tmodelState:"
               <<st->modelState.modelState<<"\tnumber:"<<number<<"\treq_number:"<<req_number;
            state = ews_sendCommand;
            statusLoader.setFuture(QtConcurrent::run(WaitSendAction::_sendAction,action));
        }
        break;
    case ews_sendCommand:
        emit sg_commandDone();
        break;
    }
}

void WaitSendAction::failedStatus()
{
    //TODO
}

void WaitSendAction::checkStatus()
{
    statusLoader.setFuture(QtConcurrent::run(WaitSendAction::_checkStatus));
}

uint32_t WaitSendAction::getSegment_number()
{
    return segment_number;
}

void WaitSendAction::setSegment_number(uint32_t &value)
{
    segment_number = value;
}
