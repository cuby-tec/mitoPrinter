Application with CSS:
$ qt_application -stylesheet style.qss

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/mito

/* Create request sequence */
1. [mito::Action_t* action = nullptr;]
2. [lexer( new Lexer(dst))] ; where dst is [sGcode* dst = &gcode;]    [sGcode gcode;]
3. [result = lexer->parcer(line);] where line is [QString line;] with \n as end of string and [result>0]
4. [action = GcodeWorker::buildAction(dst);]
5. [action->a == eSend]
6. 	[ComDataReq_t req = action->queue.dequeue();]
7.	[req.requestNumber = ++MyGlobal::requestIndex;]
8.	[queueSize = threadarc.putInArray(&req);]

9.    [connect(&threadarc,SIGNAL(sg_status_updated(const Status_t*)),this,SLOT(updateStatus(const Status_t*)));]
10.   [connect(&threadarc,SIGNAL(sg_failed_status()),this,SLOT(failedStatus()) );]

11. [threadarc.process();]
12. [updateStatus(const Status_t *status)]    

/// Template
//.h
#include <QFutureWatcher>
#include <QTimer>

QFutureWatcher<Status_t*> statusLoader;

//.cpp
#include <QtConcurrent/QtConcurrent>
connect(timer, SIGNAL(timeout()), this, SLOT(checkStatus()));

connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));

timer->start(1000);



::checkStatus(){
statusLoader.setFuture(QtConcurrent::run(ZeroPointCommand::_checkStatus));
}

::statusLoaded(){
Status_t* st = statusLoader.result();
}
============
G0 Z1
G0 Z0
G92
M84
G0 F9000 X36.705 Y36.705 Z0.250
G0 X0 Y0 F9000
G28
G0 F9000 X36.705
G0 F9000 X0
G0 F19000 Y76.705
G0 F15000 Y0
G0 F9000 X36.705 Y36.705
=============
