#include "comdata.h"
#include <string.h>
#include <QString>
#include <QDebug>

#include <QtGlobal>
#include <QWaitCondition>
#include <QVarLengthArray>

#include "myglobal.h"
#include "geometry/Arc.h"


#include <cmath>

#define cout qDebug()<<__FILE__<<__LINE__

//const QString msg3 = "Conversion error.";
//const QString msg4 = "Profile should be selected.";
//const char* msg3 = "Conversion error.";
//const char* msg4 = "Profile should be selected.";

ComData::ComData(QObject *parent) : QObject(parent)
  ,condition(egcLine)
  ,steps(0)
{

    memset(&request,'\0',sizeof(ComDataReq_t));
//    request.requestNumber = 22;

    state = ecdOne;// отправка одного Сегмента.

    profile = Profile::instance();

    cord = Coordinatus::instance();
    cord->setupProfileData();

//    motor = new StepMotor();

    controller = new Controller();

    messager = Messager::instance();

    waitTemperature = nullptr;

    waitsendAction = nullptr;
    _action = nullptr;

    setupThread();

}

void
ComData::setupThread()
{
    acknowledge_flag = false;

    connect(&threadarc,SIGNAL(sg_status_updated(const Status_t*)),this,SLOT(updateStatus(const Status_t*)));
    connect(&threadarc,SIGNAL(sg_failed_status()),this,SLOT(failedStatus()) );

}

void
ComData::setWorkValue(QString value, size_t axis_num)
{
    int i;
    bool ok;
    QString str_val = QString( value );

    i = str_val.indexOf(',');
    if(i>0)
    {
        str_val = str_val.replace(i,1,'.');
    }

    float coord = str_val.toFloat(&ok);

    Q_ASSERT(ok);

    if(cord->isAbsolute())
    {
        cord->setWorkValue(axis_num,coord);
    }else{
        float wv = cord->getWorkvalue(axis_num);
        wv += coord;
        cord->setWorkValue(axis_num,wv);
    }


}

void
ComData::setParam_coord(sGparam *param)
{
    size_t axis_num = N_AXIS;

    switch (param->group) {
    case 'X':
       axis_num = X_AXIS;
       setWorkValue(param->value,axis_num);
        break;
    case 'Y':
        axis_num = Y_AXIS;
        setWorkValue(param->value,axis_num);
        break;
    case 'Z':
        axis_num = Z_AXIS;
        setWorkValue(param->value,axis_num);
        break;
    case 'E':
        axis_num = E_AXIS;
        setWorkValue(param->value,axis_num);
        break;
    default:
        break;
    }



}
/*
bool
ComData::isPlaneHasSteps()
{

    float sum = 0.0;

    for(int i=0; i<N_AXIS;i++)
    {
        sum +=  fabs(cord->getCurrentValue(i) - cord->getNextValue(i));
    }

    return (sum != 0);
}
*/

void
ComData::setProfileValue()
{
    bool ok;

    QString fspeed; // rpm unit
    float speed;

    QString facceleration;
    float acceleration;


    for(int i=0;i<N_AXIS;i++)
    {
        block_state* block = &cord->nextBlocks[i];

        switch(i)
        {
        case X_AXIS:
            fspeed = MyGlobal::formatFloat(profile->getX_MAX_RATE());
            facceleration = MyGlobal::formatFloat(profile->getX_ACCELERATION());
            break;

        case Y_AXIS:
            fspeed = MyGlobal::formatFloat(profile->getY_MAX_RATE());
            facceleration = MyGlobal::formatFloat(profile->getY_ACCELERATION());
            break;

        case Z_AXIS:
            fspeed = MyGlobal::formatFloat(profile->getZ_MAX_RATE());
            facceleration = MyGlobal::formatFloat(profile->getZ_ACCELERATION());
            break;

        case E_AXIS:
            fspeed = MyGlobal::formatFloat(profile->getE_MAX_RATE());
            facceleration = MyGlobal::formatFloat(profile->getE_ACCELERATION());
            break;
        }

        speed = fspeed.toFloat(&ok);
        Q_ASSERT(ok);
        block->nominal_speed = speed;

        acceleration = facceleration.toFloat(&ok);
        Q_ASSERT(ok);
        block->acceleration = acceleration;

        qDebug()<<"ComData [171]"<< block->speedLevel;
    }

}


void
ComData::setDirection_bits()
{
    for(int i=0;i<N_AXIS;i++)
    {
        block_state* block = &cord->nextBlocks[i];
        float_t ds = cord->getNextValue(i) - cord->getCurrentValue(i);

        if(ds>0){
            block->direction_bits |= DIRECTION_BIT<<i;
            block->axis_mask |= STEP_BIT<<i;
        }else if(ds<0){
            block->direction_bits &= ~(DIRECTION_BIT<<i);
            block->axis_mask |= STEP_BIT<<i;
        }else{
            block->axis_mask &= ~(STEP_BIT<<i);
        }

    }
}
/*
//TODO planner_recalculate
void
ComData::planner_recalculate()
{

}
*/

/**
 * @brief ComData::buildG0command
 */
/*
void
ComData::buildG0command()
{
    bool ok;

    sGparam* gparam;

    sSegment* segment = &request.payload.instrument1_parameter;

    for(int i=0;i<sgCode->param_number;i++)
    {
        gparam = &sgCode->param[i];

        switch (gparam->group)
        {
        case 'X':
        case 'Y':
        case 'Z':
            setParam_coord(gparam);
            break;

        case 'N': // Номер строки
            uint32_t number = QString(gparam->value).toInt(&ok);
            Q_ASSERT(ok);
            segment->head.linenumber = number;
            break;
        }

    }

    cord->moveNextToCurrent();
    cord->moveWorkToNext();
    if(!isPlaneHasSteps())
    {
        return;
    }

    Recalculate_flag* flag;

    for(int i=0;i<M_AXIS;i++){
        byte* fl = &cord->nextBlocks[i].recalculate_flag;
        *fl |= true; // Одиночная команда.
    }

    controller->buildBlock(cord);

    buildComdata();
}
*/


void
ComData::buildComdata()
{
    ComDataReq_t* req = &request;
    sSegment* segment;
    sControl* control;
    block_state_t* bstates = cord->nextBlocks;

    memset(req,0,sizeof(ComDataReq_t));

//	req->requestNumber = 0;//++MyGlobal::requestIndex;// TODO get request number
    req->instruments = 1;

    req->command.order = eoSegment;
    req->command.instrument = 1;
    req->command.reserved = 0;

    segment = &req->payload.instrument1_parameter;

    //------------ payload =============================
    segment->head.linenumber = sgCode->line ;
    segment->head.axis_number = M_AXIS;//0;

    //if(segment->head.reserved == EXIT_CONTINUE)
    //    ms_finBlock = continueBlock;
    //else
    //    ms_finBlock = exitBlock;
    segment->head.reserved &= ~EXIT_CONTINUE;

    for(int i=0;i<M_AXIS;i++)
    {
        if(bstates[i].steps>0)
            segment->head.axis_mask |= (1<<i);
        else
            segment->head.axis_mask &= ~(1<<i);
    }
    //======== sControl =========

    for(int i =0;i<M_AXIS;i++){
        control = &segment->axis[i];
        block_state_t* bstate = &bstates[i];

        control->accelerate_until = bstate->accelerate_until;
        control->decelerate_after = bstate->decelerate_after;

        if(bstate->path>0)
            control->direction = edForward;
        else
            control->direction = edBackward;

        control->final_rate = bstate->final_rate;
        control->initial_rate = bstate->initial_rate;
        control->nominal_rate = bstate->nominal_rate;
        control->final_speedLevel = bstate->final_speedLevel;

        control->speedLevel = bstate->accelerate_until;// TODO Attention

        control->microsteps = bstate->microstep;

        for(int j=0;j<3;j++){
            control->schem[j] = bstate->schem[j];
        }

        control->steps = bstate->steps;

        control->axis = bstate->axis_mask;
    }
}

//TODOH Circle G2 G3 (Clockwise Arc)
void
ComData::buildG2Command()
{
    bool ok;

    sGparam* gparam;

    arc = new Arc();

    QString xstr,ystr,istr,jstr,rstr;

    sSegment* segment = &request.payload.instrument1_parameter;

    for(int i=0;i<sgCode->param_number;i++)
    {
        gparam = &sgCode->param[i];

        switch (gparam->group)
        {
        case 'X':
            xstr = gparam->value;
            break;
        case 'Y':
            ystr = gparam->value;
            break;

        case 'I':
            istr = gparam->value;
            break;

        case 'J':
            jstr = gparam->value;
            break;

        case 'R':
            rstr = gparam->value;
            break;

        case 'E':
            // TODOH Extruder
            break;

        case 'N': // Номер строки
            uint32_t number = QString(gparam->value).toInt(&ok);
            Q_ASSERT(ok);
            segment->head.linenumber = number;
            break;
        }
    }

    // G2 X90.6 Y13.8 I5 J10 E22.4
    //77,2317745639	56,0908965345 30 grad

    // G2 X90.6 Y13.8 I15 J10 E22.4
    //52,8477211996	-55,5541760683 300 grad
    //56,0908965344	-53,5715205261 302,88°

//    QString startX("77,2317745639");

//    QString startY("56,0908965345");

//    arc->setStart(Point(cord->getCurrentValue(X_AXIS),cord->getCurrentValue(Y_AXIS))); // TODO current value
//    arc->setStart(startX,startY);

    Point* start = new Point(cord->getNextValue(X_AXIS),cord->getNextValue(Y_AXIS));

    arc->setStart(*start);

    Q_ASSERT(xstr.count()>0 && ystr.count()>0);

    arc->setEnd(xstr,ystr);

    Q_ASSERT(istr.count()>0 && jstr.count()>0);// TODO set radious

    arc->setCenter(istr,jstr);

    QString tstr = profile->getARC_TOLERANCE();

    Q_ASSERT(tstr.count()>0);

    double_t precicion = controller->getPrecicion(X_AXIS,0);

    //TODO correct precicion from Controller.
    arc->setPrecicion(precicion);

    arc->calculate();

    cord->moveNextToCurrent();
    cord->initWork();

}

/*
void
ComData::buildGgroup()
{
    int groupnumber;
    bool ok;

    groupnumber = QString(sgCode->value).toInt(&ok);

    switch (groupnumber) {

    case 0:
    case 1:
        state = ecdOne;
        buildG0command();
        break;

    case 2:
    case 3:
        state = ecdCircle;
        buildG2Command();
        break;

    case 4:
        break;



    default:
        break;
    }

}
*/

void
ComData::buildMgroup()
{
    int groupnumber;
    bool ok;
    groupnumber = QString(sgCode->value).toInt(&ok);

    switch (groupnumber) {
    case 104:

        break;
    case 108:
        break;

    default:
        break;
    }

}

/*
ComDataReq_t*
ComData::build(sGcode *sgcode)
{
    this->sgCode = sgcode;

    cord->initWork();

    switch (sgCode->group) {
    case 'G':
        buildGgroup();
        break;

    case 'M':
        buildMgroup();
        break;

    default:
        // somthing wrong.
        break;
    }
    return(&request);
}
*/
#define cout	qDebug()<<__FILE__<<__LINE__
// from GConsole
void ComData::buildComData(sGcode *sgcode, bool checkBox_immediately)
{
    int queueSize = 0;
    mito::Action_t* action = nullptr;
    condition = egcLine;
    gworker = new GcodeWorker;
    QMessageBox* msgBox;

    action = gworker->buildAction(sgcode);
    if(action == nullptr)
        return;//TODO
    switch (action->a) {
    case eWaitSend:
    case eSendWait:
    case eSend:
        if(action->queue.isEmpty()){
            cout<<"Empty.";
        }else{
            while (!action->queue.isEmpty()){
                ComDataReq_t req = action->queue.dequeue();
                req.requestNumber = ++MyGlobal::requestIndex;
                queueSize = threadarc.putInArray(&req);
            }
            threadarc.setMdelay(500);
            //                threadarc.setMax_tryCounter(200);
            threadarc.process();
            cout<<"process==========<<"<<MyGlobal::requestIndex<<"\tqueeSize:"<<queueSize;
        }
        break;

    case eNext:
//        emit sg_updateStatus(nullptr);
        msgBox = new QMessageBox;
        msgBox->setText("No motion requered.");
        msgBox->exec();
        break;

    case eEOF:
        break;
    }
    delete gworker;

#if VERSION == 11
    //    ComDataReq_t* req = getRequest();
    ComDataReq_t* req = build(sgcode);

    switch(state){

    case ecdOne:
        if(!isPlaneHasSteps())
        {
            qDebug()<<__FILE__<<":"<<__LINE__<<": group:"<<sgcode->group;
            break;
        }
        // immediately execute
        if(checkBox_immediately)
            req->command.reserved |= EXECUTE_IMMEDIATELY;
        else
            req->command.reserved &= ~EXECUTE_IMMEDIATELY;

        req->payload.instrument1_parameter.head.reserved &= ~EXIT_CONTINUE;
//		qDebug()<<"ComData[584] from GConsole";
        setRequestNumber(++MyGlobal::requestIndex);//MyGlobal::requestIndex MyGlobal::commandIndex
        threadarc.putInArray(&request);
        threadarc.process();
        break;

    case ecdCircle:
        //================
        //    QVarLengthArray<ComDataReq_t,1024> array(num);
            ThreadArc *pthreadarc = &threadarc;
        //    QVarLengthArray<ComDataReq_t> array = threadarc->getArray();
            double_t delay = 1000*controller->getTimeOfCounter(2500420);
            threadarc.setMdelay(delay);//50 100 900

            uint send_counter = 0;

            double_t precicion = arc->getPrecicion();

            double_t precicionY = arc->getPrecicion();

            int32_t d20[2][2] = {{0,0},{0,0}};

            Point pStart = arc->getStart();

            cout<<"Start X:"<<pStart.x<<"\tY:"<<pStart.y <<endl;

            Point pCurrent = pStart;

            uint32_t i_points = 0;

            for(uint32_t i=1;i<arc->getPointsNumber();i++){

                Point p = arc->getPoint(i); // Line 611
                Point dp = p-pStart;

                d20[1][0] = trunc(dp.x/precicion);
                d20[1][1] = trunc(dp.y/precicionY);

                bool h20 = d20[0][0] ^ d20[1][0];
                bool i20 = d20[0][1] ^ d20[1][1];

                d20[0][0] = d20[1][0];
                d20[0][1] = d20[1][1];

                if((!h20)&&(!i20))
                    continue;
                if(h20){
                    pCurrent.x = pStart.x + d20[1][0]*precicion;
                }
                if(i20){
                    pCurrent.y = pStart.y + d20[1][1]*precicionY;
                }

                cord->setWorkValue(X_AXIS,pCurrent.x);//pCurrent.x
                cord->setWorkValue(Y_AXIS,pCurrent.y);//pCurrent.y
                cord->moveWorkToNext();
                i_points++;
                controller->buildBlock(cord);
/*
                cout
                    <<"ComData[664] i:"<<i_points
        //			<<"\tstepX:"<<h20<<"\tstepY:"<<i20
                    <<"\tstepx:"<<cord->nextBlocks[X_AXIS].steps
                    <<"\tstepsy:"<<cord->nextBlocks[Y_AXIS].steps
                    <<"\tX:"<<cord->getNextValue(X_AXIS)
                    <<"\tY:"<<cord->getNextValue(Y_AXIS);
//		        	<<endl;
*/
                buildComdata();
                request.requestNumber = ++MyGlobal::requestIndex;
//		        request.command.reserved &= ~EXECUTE_IMMEDIATELY;
                if(++send_counter==1)
                {
                    if(checkBox_immediately)
                        request.command.reserved |= EXECUTE_IMMEDIATELY;
                    else
                        request.command.reserved &= ~EXECUTE_IMMEDIATELY;
                }

                request.payload.instrument1_parameter.head.reserved |= EXIT_CONTINUE;

                int s = pthreadarc->putInArray(&request);
                qDebug()<<"ComData[690] s:"<<s;
                cord->moveNextToCurrent();

            }
/*
            cout<<"ComData[692]  X:"<<cord->getNextValue(X_AXIS)<<"\tY:"<<cord->getNextValue(Y_AXIS)
                    <<"\tX:"<<cord->getNextValue(X_AXIS)
                    <<"\tY:"<<cord->getNextValue(Y_AXIS)
                    << endl;
*/
            // sending
            threadarc.process();
        //================
        break;
    }
#endif


}

void ComData::run(GcodeWorker* gworker)
{
    condition = egcFile;
    this->gworker = gworker;
//    connect(&testT, SIGNAL(timeout()),this ,SLOT(testTimer()) );
//    testT.start(200);

    // Check Controller@USB
/*    RequestFactory* factory = new RequestFactory();
    ComDataReq_t *request = new ComDataReq_t;
    factory->build(request,eoState);
    threadarc.putInArray(request);
    threadarc.process();*/
    runState = ersRunning;
//    runState = ersWaitParamTemperature; // DEBUG
/*    delete request;
    delete factory;*/

    waitParam();
    //wait answer ...

//    gworker->readCommandLine();
//    connect(gworker, SIGNAL(sg_executeComplite()),this, SLOT(slot_fileComplite()));

}

void ComData::stop()
{
    runState = ersStop;
}
#define FSMDEBUG  1
#define FSMCOUNTER  10
#define TRY_COUNTER 500
#define MDELAY      70

void ComData::_run()
{
    //    cout<<"run:"<<gworker->isFileOpened();
    mito::Action_t* action;
    int queueSize;
    // TODO execute by states;
//    cout<<"runState:"<<runState<<"\t requestIndex:"<<MyGlobal::requestIndex;
    switch (runState) {

    case ersStop:
        cout<<"Soft Stop.";
        emit sg_executeComplite();
//        return;
        break;

    case ersEOF:
        cout<<"end of file";
        emit sg_executeComplite();
        return;
        break;

    case ersRunning:
#if LEVEL==1
        cout<<"Running,=======\n";
#endif
        // Build request
_run1:
        action = gworker->readCommandLine();
        // Send request
        if(action != nullptr)
        {
            switch (action->a) {
            case eNext:
                goto _run1;
                break;
            case eSend:
                if(action->queue.isEmpty())
                    cout<<"Empty.";
                while (!action->queue.isEmpty()){
                    ComDataReq_t req = action->queue.dequeue();
                    req.requestNumber = ++MyGlobal::requestIndex;
#if DEBUG_LEVEL==1
                    cout<<req.payload.instrument1_parameter.axis[X_AXIS].steps<<"\taccel:"<<req.payload.instrument1_parameter.axis[X_AXIS].accelerate_until<<"\tnuber:"<<req.requestNumber;
#endif
//                    cout<<req.payload.instrument1_parameter.head.linenumber;
                    queueSize = threadarc.putInArray(&req);
                }
                threadarc.setMdelay(MDELAY);
                threadarc.set_tryCounter(TRY_COUNTER);
                threadarc.process();
#if LEVEL == 1
                cout<<"process==========<<"<<MyGlobal::requestIndex<<"\tqueeSize:"<<queueSize;
#endif
                break;

            case eWaitSend:
            	//TODO eWaitSend
                waitsendAction = new WaitSendAction(this,action);
//                waitsendAction->setSegment_number()
                connect(waitsendAction,SIGNAL(sg_commandDone()),this, SLOT(waitsendDone()));
//                _action = action;
                if(!action->queue.isEmpty())
                {
                    ComDataReq_t& req = action->queue.head();
                    waitsendAction->setSegment_number(req.payload.instrument1_parameter.head.linenumber);
                }
                _action = new mito::Action_t;
                _action->queue.enqueue(action->queue.head());
                waitsendAction->execute();
            	break;

            case eSendWait:
#ifndef WAITTEMPERATURE_H
                while (!action->queue.isEmpty()){
                    ComDataReq_t req = action->queue.dequeue();
                    req.requestNumber = ++MyGlobal::requestIndex;
                    threadarc.putInArray(&req);
                }
                threadarc.process();
                cout<<"process==========<<"<<MyGlobal::requestIndex;

                param.d = action->param.d;// wait param;
                runState = ersWaitParamTemperature;
                connect(&waitTimer, SIGNAL(timeout()),this ,SLOT(waitParam()) );
                waitTimer.start(500);
                pd = new QProgressDialog("Heating in progress.", "Cancel", 0, 100);
#if FSMDEBUG==1
                pd->setRange(0, FSMCOUNTER );
#else
                pd->setRange(0, static_cast<int>(param.d) );
#endif
                pd->setMinimumDuration(0);
                connect(pd,SIGNAL(canceled()), this, SLOT(heatingCancel()) );
#else
                runState = ersWaitParamTemperature;
                waitTemp(action);
#endif

                break;

            case eEOF:
                runState = ersEOF;
                cout<<"eEOF";
                break;
            }

        }else{
            cout<<"unused tag.";
            goto _run1;
        }
        // wait answer
        break;

    case ersWaitParamTemperature:
#ifndef WAITTEMPERATURE_H
        // Compare status and target param.
//        float status_temperature = statusParam.f;
//        double_t target = param.d;
        if(steps++ >= FSMCOUNTER){
//            pd->cancel();
#if FSMDEBUG==1
            waitTimer.stop();
            runState = ersRunning;
            cout<<"timer stoped.";
            delete pd;
            goto _run1;
#endif
        }
        else
#if FSMDEBUG==1
            pd->setValue(steps);
#else
            pd->setValue( static_cast<int>(statusParam.f));
#endif
        if(  static_cast<double_t>(statusParam.f) >= param.d ){
            waitTimer.stop();
            runState = ersRunning;
            cout<<"timer stoped.";
            goto _run1;
        }
        cout<<"\ttemperature:"<<statusParam.f<<"\t"<<param.d;
#else
        cout<<"Failed. Never come in ersWaitParamTemperature.";
#endif
        break;

    case ersError:
        cout<<"Device is unreachable.";
//        Messager* messaager = Messager::instance();
        messager->putMessage(QString("Device is unreachable."));
        break;

    }
}

//Start reading  G-file.
void ComData::waitParam()
{
    RequestFactory* factory = new RequestFactory();
    ComDataReq_t *request = new ComDataReq_t;
    factory->build(request,eoState);
    threadarc.putInArray(request);
     threadarc.process();
    delete request;
     delete factory;
}


void ComData::waitTemp(mito::Action_t *action)
{
    waitTemperature = new WaitTemperature(this, action->param.d);
    connect(waitTemperature,SIGNAL(sg_commandDone()),this, SLOT(temperatureDone()) );
    connect(waitTemperature, SIGNAL(sg_canceled()), this, SLOT(sl_caceled()));
    waitTemperature->execute();

}


void ComData::temperatureDone()
{
    cout<<"temperatureDone";
    if(runState != ersStop)
        runState = ersRunning;
	delete waitTemperature;
	waitTemperature = nullptr;
    _run();
}

void ComData::waitsendDone()
{
//    int queueSize;
#if REPOERT_LEVEL==2
    cout<<"waitsendDone";
#endif
    if(runState != ersStop)
        runState = ersRunning;
    delete waitsendAction;
    waitsendAction = nullptr;
    /*
    //------------
    while (!_action->queue.isEmpty()){
        ComDataReq_t req = _action->queue.dequeue();
        req.requestNumber = ++MyGlobal::requestIndex;
        queueSize = threadarc.putInArray(&req);
    }
    delete _action;
    _action = nullptr;
//    threadarc.setMdelay(70);
//    threadarc.set_tryCounter(200);
    threadarc.process();
    //------------
    */
    _run();
}
/*
void ComData::testTimer()
{
    cout<<"testTimer";
}
*/
void ComData::slot_fileComplite()
{
#if LEVEL==1
    cout<<"slot_fileComplite";
#endif
    emit sg_executeComplite();
    Messager* message = Messager::instance();
    message->setProgramExecutionComplite();
}
/*
void ComData::heatingCancel()
{
    //TODO
    cout<<"heatingCancel";
    waitTimer.stop();
    delete  pd;
}
*/
void ComData::updateStatus(const Status_t *status)
{
    acknowledge_flag = true;
    if(status->freeSegments<=SEGMENT_QUEE_SIZE){

        emit sg_updateStatus(status);
        //    Messager* message = Messager::instance();
        messager->putStatus(status);
        statusParam.f = status->temperature;

        //    runState = ersRunning;
#if LEVEL==1
        cout<<"updateStatus" <<"\t" <<status->modelState.modelState;
#endif
        switch(condition)
        {
        case egcFile:
            _run();
            break;
        case egcLine:
            cout<<"egcLine";
            break;
        }
    }else{
        //Error
        cout<<"Error:status.freeSegments:"<<status->freeSegments;
    }
}




#define DEBUG_no
void ComData::failedStatus()
{
 //TODO failed Status
    cout<<"failedStatus";
#ifdef DEBUG
    runState = ersRunning;
#else
    runState = ersError;
#endif
    _run();
}

void ComData::sl_caceled()
{
    //Heating canceled by user.
//    message->setProgramExecutionComplite();
    messager->putMessage("Canceled.");
    threadarc.quit();
    emit sg_executeComplite();
    delete  waitTemperature;
    waitTemperature = nullptr;
}


void
ComData::initWorkAray()
{
    cord->initWork();
}
