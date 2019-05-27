#ifndef THREADARC_H
#define THREADARC_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QQueue>


#include "links/Status_t.h"
#include "usbexchange.h"
#include "myglobal.h"
#include "links/ComDataReq_t.h"

#include <QTime>

#define MAX_TRY_COUNTER     20
#define DEFAULT_DELAY       100


extern QMutex thermo_gmutex;

class ThreadArc : public QThread
{
    Q_OBJECT

public:
    ThreadArc();

    void process();

    int putInArray(ComDataReq_t *src);

    void clear() { array.clear();}

    uint get_tryCounter() const;
    void set_tryCounter(const uint &value);

    size_t getMdelay() const;
    void setMdelay(const size_t &value);

signals:
    void sg_status_updated(const Status_t* status);
    void sg_failed_status();

protected:
    void run();


private:
    bool abort;
    bool restart;

    uint max_tryCounter;

    size_t mdelay;// milliseconds delay

    Status_t status;
    QVarLengthArray<ComDataReq_t,1024> array;

    QQueue<ComDataReq_t> queue;

    ComDataReq_t buffer;

    QMutex mutex;
    QWaitCondition condition;
    UsbExchange* exch;

    QTime timer;

};

#endif // THREADARC_H
