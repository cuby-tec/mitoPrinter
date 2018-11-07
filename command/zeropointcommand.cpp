#include "zeropointcommand.h"
#include <QDebug>
#include "myglobal.h"
#include "gparcer/gcodeworker.h"

#include "links/eModelstate.h"
#include <string.h>
#include <QtConcurrent/QtConcurrent>


#define cout qDebug()<<__FILE__<<__LINE__

Status_t*
ZeroPointCommand::_checkStatus()
{
    Status_t* status = new Status_t;
    RequestFactory* factory = new RequestFactory;
    ComDataReq_t* req = new ComDataReq_t;
    factory->build(req,eoState);
    req->requestNumber = ++MyGlobal::requestIndex;

    //2. Send request.
    UsbExchange* exch = new UsbExchange;
    int result_exch;
    thermo_gmutex.lock();
    result_exch = exch->sendRequest(req);
    if(result_exch == EXIT_SUCCESS){
        memcpy(status,exch->getStatus(),sizeof(Status_t));
    }

    thermo_gmutex.unlock();

    cout<<"checkStatus";
    return status;
}

Status_t*
ZeroPointCommand::moveExtruderZeroPoint()
{
    char cmdbuffer[80];
    char* pbuffer;
    size_t len;
    int parce_error;
    sGcode sgcode;
    Status_t* status = new Status_t;

    QString gcommand("G1 Z-100");   // TODO length from profile
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
    return status;
}

ZeroPointCommand::ZeroPointCommand(QObject *parent) : QObject(parent)
{
#if Zero_VERTION==3
    zrun = new ZeroPoint_runnable();
    zrun->setAutoDelete(false);

#endif
#if Zero_VERTION==2
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));

#endif
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
#if Zero_VERTION==1
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
#endif
    //3. Wait response.
#if Zero_VERTION==2
//    QFutureWatcher<Status_t*> statusLoader;

    //connect(&m_modelLoader, SIGNAL(finished()), this, SLOT(modelLoaded()));
    connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));
    statusLoader.setFuture(QtConcurrent::run(ZeroPointCommand::moveExtruderZeroPoint));

    timer->start(1000);

#endif

#if Zero_VERTION==3
//    ZeroPoint_runnable* zrun = new ZeroPoint_runnable();
//    zrun->setAutoDelete(false);
    QThreadPool::globalInstance()->start(zrun);
    cout<<zrun->a;
#endif

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
    /**
 * @brief The eModelstate enum
 * enum eModelstate{
 *     ehIdle = 1, ehIwork
 *     ,ehEnderXmax, ehEnderXmin, ehEnderYmax, ehEnderYmin, ehEnderZmax, ehEnderZmin
 *     ,ehException, ehWait_instrument1, ehWait_instrument2
 * };
*/
#if Zero_VERTION==2
    Status_t* st = statusLoader.result();
    cout<<"statusLoaded"<< ModelState::state( st->modelState.modelState)<<st->coordinatus[Z_AXIS];
    if(st->modelState.modelState == ehEnderZmin){
        timer->stop();
        emit sg_commandDone();
        executeGCommand("G92 Z0");
        cout<<"Stoped";
    }
#endif
}
#if Zero_VERTION==2
void ZeroPointCommand::checkStatus()
{
    //TODO
    statusLoader.setFuture(QtConcurrent::run(ZeroPointCommand::_checkStatus));
}
#endif

