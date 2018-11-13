#include "waitsendaction.h"
#include "myglobal.h"
#include "gparcer/gcodeworker.h"

#include "links/eModelstate.h"
#include <string.h>
#include <QtConcurrent/QtConcurrent>

WaitSendAction::WaitSendAction(QObject *parent, mito::Action_t* action) : QObject(parent)
{
    this->action = action;
    timer = new QTimer(this);
    state = ews_waitQueue;
    connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
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
    switch (state) {
    case ews_waitQueue:
        if((st->modelState.queueState == SEGMENT_QUEE_SIZE)
                &&(st->modelState.modelState != ehIwork)){
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
