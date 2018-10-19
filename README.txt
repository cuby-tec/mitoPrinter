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
