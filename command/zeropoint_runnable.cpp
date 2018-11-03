#include "zeropoint_runnable.h"

#include "myglobal.h"
#include "gparcer/gcodeworker.h"
#include "exchange/usbexchange.h"

#include <QDebug>
#include <QThread>


#define cout qDebug()<<__FILE__<<__LINE__

ZeroPoint_runnable::ZeroPoint_runnable():a(10)
{
    status = new Status_t;
    status->frameNumber = 35;
}

void ZeroPoint_runnable::run()
{
//    a++;
//     qDebug() << "Hello world from thread" << QThread::currentThread()<<status->frameNumber++;
    char cmdbuffer[80];
    char* pbuffer;
    size_t len;
    int parce_error;
    sGcode sgcode;
    Status_t* status = new Status_t;

    QString gcommand("G1 Z-100");   // TODO legth from profile
    std::string cmdstd = gcommand.toStdString();
    pbuffer = strncpy( cmdbuffer,cmdstd.data(),sizeof(cmdbuffer) );
    len = strlen(pbuffer);
    *(pbuffer+len) = 0x0A;
    memset(&sgcode,0,sizeof(struct sGcode));
    parce_error = MyGlobal::parceString(pbuffer,&sgcode);

    GcodeWorker* gworker = new GcodeWorker;
    mito::Action_t* action = nullptr;
    action = gworker->buildAction(&sgcode);
    if(!action->queue.isEmpty()){
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
    }


    cout<<"moveExtruderZeroPoint";
}
