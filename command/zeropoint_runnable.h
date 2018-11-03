#ifndef ZEROPOINT_RUNNABLE_H
#define ZEROPOINT_RUNNABLE_H

#include <QRunnable>
#include "links/Status_t.h"

#include <QMutex>

extern QMutex thermo_gmutex;

class ZeroPoint_runnable : public QRunnable
{
public:
    ZeroPoint_runnable();

    void run();

    int a;

    Status_t* status;// = new Status_t;
};

#endif // ZEROPOINT_RUNNABLE_H
