#include "thermothread.h"

ThermoThread::ThermoThread(QObject *parent)
    :QThread(parent)
{
    abort = false;
    restart = false;
//    symbol = "O";
//    symbol.append("%1");
//    slp = 1;
//    start();
    exch = new UsbExchange();
    factory = new RequestFactory;

}

ThermoThread::~ThermoThread()
{
    mutex.lock();
    delete exch;
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    int s = wait(5000);
    if(s == false)
        qWarning()<<__FILE__<<__LINE__<<"isFinished:"<<isFinished()<<"\twait:"<<s;
}


//TODO send/recieve status
void
ThermoThread::process()
{
    QMutexLocker locker(&mutex);
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}


// TODO : emit sg_temperature_updated
 void
 ThermoThread::run(){

     int result_exch;
     forever {
//         mutex.lock();
         thermo_gmutex.lock();
//         MGlobal::M_mutex->lock();

         // TODO frameIndex
         index++;

         factory->build(&request,eoState);
//         exch->buildComData(&request,eoState);

         result_exch = exch->sendRequest(&request);

         if(!(result_exch == EXIT_SUCCESS))
         {
             status = nullptr;
             if (!restart)
                 emit sg_failed_status();
         }else{
             status = exch->getStatus();
         }

         thermo_gmutex.unlock();

         if (abort)
             return;

         if (!restart && (result_exch == EXIT_SUCCESS ))
             emit sg_temperature_updated(status);

         mutex.lock();
//         thermo_gmutex.lock();
         if (!restart)
             condition.wait(&mutex);
//             condition.wait(&thermo_gmutex);
         restart = false;
         mutex.unlock();
//         thermo_gmutex.unlock();

     }// forever
 }
