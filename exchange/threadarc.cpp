#include "threadarc.h"
#include <QDebug>

#define cout	qDebug()<<__FILE__<<__LINE__

ThreadArc::ThreadArc()
{
    abort = false;
    restart = false;
    exch = new UsbExchange();
    array.clear();
    max_tryCounter = MAX_TRY_COUNTER;
    mdelay = DEFAULT_DELAY;
}

void ThreadArc::process()
{
//    QMutexLocker locker(&mutex);
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

int ThreadArc::putInArray(ComDataReq_t *src)
{
    queue.enqueue(*src);
    return queue.size();
}

void ThreadArc::run()
{
    int result_exch = 0;
    size_t try_counter = 0;
    forever{

        while(!queue.isEmpty())
        {
            if(!queue.isEmpty()){
                buffer = queue.dequeue();
            }

            ComDataReq_t* request = &buffer;
volatile uint32_t line = request->payload.instrument1_parameter.head.linenumber;
//            request->requestNumber = ++MyGlobal::requestIndex;
            try_counter = 0;
            //
            do{

                thermo_gmutex.lock();

                result_exch = exch->sendRequest(request);

                if(result_exch != EXIT_SUCCESS)
                {
                    status.frameNumber = 0;
                    if (!restart)
                        emit sg_failed_status();
                    qDebug()<<__FILE__<<__LINE__<<"ThreadExchange[38]"<<" failed.";
                    break;
                }else{
                    //             status = exch->getStatus();
                    memcpy(&status,exch->getStatus(),sizeof(Status_t));
                    if(request->requestNumber != status.frameNumber)
                        cout<<"Number status:"<<status.frameNumber <<"\treq:"<<request->requestNumber;
                }

                thermo_gmutex.unlock();

                // check flag, and wait and resend if needed
                if(!(status.modelState.reserved1&COMMAND_ACKNOWLEDGED))
                {
                    try_counter++;
                    cout<<"number:"<<status.currentSegmentNumber <<"\tqueue:"<<status.freeSegments<<"\tstate:"<<status.modelState.modelState<<"\tord_ly:"<<status.modelState.reserved1;
                    cout<<"Number status:"<<status.frameNumber <<"\treq:"<<request->requestNumber;
                    msleep(mdelay);
                }
                if(try_counter>=max_tryCounter){
#if LEVEL==1
                    cout<<"number:"<<status.frameNumber <<"\tqueue:"<<status.freeSegments;
#endif
                    break;
                }

            }while(!(status.modelState.reserved1&COMMAND_ACKNOWLEDGED));

            if(try_counter>=max_tryCounter){
                break;
            }
            msleep(40);//debug delay

        }// while()

        if(try_counter<max_tryCounter){
            if (!restart && (result_exch == EXIT_SUCCESS ))
                emit sg_status_updated(&status);
        }else{
            emit sg_failed_status();
        }

        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock(); //Debug mode

    }// forever
}

size_t ThreadArc::getMdelay() const
{
    return mdelay;
}

void ThreadArc::setMdelay(const size_t &value)
{
    mdelay = value;
}

uint ThreadArc::get_tryCounter() const
{
    return max_tryCounter;
}

void ThreadArc::set_tryCounter(const uint &value)
{
    max_tryCounter = value;
}
