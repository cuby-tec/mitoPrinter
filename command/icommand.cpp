
#include "gparcer/gcodeworker.h"
#include "icommand.h"

#include <QDebug>

#define cout qDebug()<<__FILE__<<__LINE__

ICommand::ICommand()
{

}

ICommand::~ICommand()
{

}

Status_t*
ICommand::_checkStatus()
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
ICommand::executeGCommand(QString command)
{
    char cmdbuffer[80];
    char* pbuffer;
    size_t len;
    int parce_error;
    sGcode sgcode;
    Status_t* status = new Status_t;

//    QString gcommand("G1 Z-100");   // TODO length from profile
    std::string cmdstd = command.toStdString();
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
        delete exch;

    }
    cout<<"executeGCommand";
    return status;
}
