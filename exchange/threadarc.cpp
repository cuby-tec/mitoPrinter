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
    mutex.lock();
}

void ThreadArc::process()
{
//    QMutexLocker locker(&mutex);
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        mutex.lock();
        condition.wakeOne();
        mutex.unlock();
//        cout<<"isRunning:"<<isRunning();
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
    ComDataReq_t* request = &buffer;
    forever{
//        mutex.lock();
        while(!queue.isEmpty())
        {
//            if(!queue.isEmpty()){
                buffer = queue.dequeue();
//            }

//volatile uint32_t line = request->payload.instrument1_parameter.head.linenumber;
//            request->requestNumber = ++MyGlobal::requestIndex;
            try_counter = 0;
            //
            do{
//cout<<"Lock."<<queue.count();
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
#if DEBUG_LEVEL==1
                    if(request->requestNumber != status.frameNumber)
                        cout<<"Number status:"<<status.frameNumber <<"\treq:"<<request->requestNumber;
#endif
                }

                thermo_gmutex.unlock();

                // check flag, and wait and resend if needed
                if(!(status.modelState.reserved1&COMMAND_ACKNOWLEDGED))
                {
                    try_counter++;
#if LEVEL==2
                    cout<<"number:"<<status.currentSegmentNumber <<"\tqueue:"<<status.freeSegments<<"\tstate:"<<status.modelState.modelState<<"\tord_ly:"<<status.modelState.reserved1;
                    cout<<"Number status:"<<status.frameNumber <<"\treq:"<<request->requestNumber;
#endif
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
            msleep(20);//debug delay 40

        }// while()

        if(try_counter<max_tryCounter){
            if (!restart && (result_exch == EXIT_SUCCESS )){
                emit sg_status_updated(&status);
//                cout<<"restart:" <<restart<<"\tresult_exch:"<<result_exch;
            }
            else{
                cout<<"restart:" <<restart<<"\tresult_exch:"<<result_exch;
            }
        }else{
            emit sg_failed_status();
        }
//        mutex.unlock();
//        mutex.lock();
//        thermo_gmutex.lock();
//        if (!restart){
            condition.wait(&mutex);// thermo_gmutex
//            cout;
//        }
        restart = false;
//        mutex.unlock(); //Debug mode
//        thermo_gmutex.unlock();

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
