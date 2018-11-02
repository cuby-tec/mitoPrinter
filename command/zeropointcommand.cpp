#include "zeropointcommand.h"
#include <QDebug>
#include "myglobal.h"
#include "gparcer/gcodeworker.h"

#include <string.h>
#include <QtConcurrent/QtConcurrent>


#define cout qDebug()<<__FILE__<<__LINE__


static Status_t* moveExtruderZeroPoint()
{
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


    cout<<"moveExtruderZeroPoint";
    return status;
}

ZeroPointCommand::ZeroPointCommand(QObject *parent) : QObject(parent)
{

}

ZeroPointCommand::~ZeroPointCommand()
{
//    threadarc.exit();
}

void ZeroPointCommand::execute()
{
    char cmdbuffer[80];
    char* pbuffer;
    size_t len;
    int parce_error;
    sGcode sgcode;
    //    ThreadArc* threadarc = new ThreadArc;
    //    threadarc->exit();
    //    delete threadarc;
/*

    QString gcommand("G1 Z-100");   // TODO legth from profile
    std::string cmdstd = gcommand.toStdString();
    pbuffer = strncpy( cmdbuffer,cmdstd.data(),sizeof(cmdbuffer) );
    len = strlen(pbuffer);
    *(pbuffer+len) = 0x0A;
    memset(&sgcode,0,sizeof(struct sGcode));
    parce_error = MyGlobal::parceString(pbuffer,&sgcode);
//    if( parce_error > 0 )
    Q_ASSERT(parce_error > 0);

    //1. Creat request

    GcodeWorker* gworker = new GcodeWorker;
    mito::Action_t* action = nullptr;
    action = gworker->buildAction(&sgcode);
    ComDataReq_t req = action->queue.dequeue();
    req.requestNumber = ++MyGlobal::requestIndex;
//    queueSize = threadarc.putInArray(&req);

    //2. Send request.

    connect(&threadarc,SIGNAL(sg_status_updated(const Status_t*)),this,SLOT(updateStatus(const Status_t*)));
    connect(&threadarc,SIGNAL(sg_failed_status()),this,SLOT(failedStatus()) );

    int queueSize;
    queueSize = threadarc.putInArray(&req);
    threadarc.setMdelay(500);
    threadarc.process();

    cout<<"msg:"<<req.payload.instrument1_parameter.axis[Z_AXIS].initial_rate;
*/
    //3. Wait response.

//    QFutureWatcher<Status_t*> statusLoader;

    //connect(&m_modelLoader, SIGNAL(finished()), this, SLOT(modelLoaded()));
    connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));
    statusLoader.setFuture(QtConcurrent::run(::moveExtruderZeroPoint));



}




void ZeroPointCommand::updateStatus(const Status_t *status)
{
    // TODO
    cout<<status->frameNumber;
}

void ZeroPointCommand::failedStatus()
{
    //TODO
    cout<<"failed";
}

void ZeroPointCommand::statusLoaded()
{
    //TODO
    Status_t* st = statusLoader.result();
    cout<<"statusLoaded"<<st->frameNumber;
}


