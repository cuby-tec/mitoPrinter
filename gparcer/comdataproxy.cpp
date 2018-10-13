#include "comdataproxy.h"
#include "links/ComDataReq_t.h"

#include "mitoaction.h"

#include <QDebug>
#include <cmath>


#define DEBUGLEVEL  1

#define cout qDebug()<<__FILE__<<__LINE__


ComdataProxy::ComdataProxy(QObject *parent) : QObject(parent)
  , line_counter(0)
  , controller(new Controller)
{
    coordinatus = Coordinatus::instance();
}

//#define diff(M) coordinatus->getCurrentValue(M##_AXIS) - coordinatus->getNextValue(M##_AXIS)
#define diff(M) coordinatus->getNextValue(M##_AXIS) - coordinatus->getCurrentValue(M##_AXIS)

//Line motion
mito::Action_t*
ComdataProxy::sendG0Line(sG0_t *data)
{

    //TODO
    mito::Action_t *action = new mito::Action_t;
    line_counter++;
#if DEBUGLEVEL==2
    qDebug()<<__FILE__<<__LINE__ <<"G0:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\tz:"<<data->z;
#endif
    //setParam_coord
    coordinatus->setWorkValue(X_AXIS, data->x);
    coordinatus->setWorkValue(Y_AXIS, data->y);
    coordinatus->setWorkValue(Z_AXIS, data->z);
    coordinatus->setWorkValue(E_AXIS, data->e);

    if(!isPlaneHasSteps())
    {
        action->a = eNext;
        return action;
    }
    coordinatus->moveNextToCurrent();
    coordinatus->moveWorkToNext();

#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"DIFF:"<<"X:"<<diff(X) <<"\tY:"<<diff(Y)<<"\tZ:"<<diff(Z);
#endif
    bool build = controller->buildBlock(coordinatus);

    if(build == true){
        RequestFactory *factory = new RequestFactory();
        //    buildComdata(data->n);
        ComDataReq_t* req = factory->build(data->n);
        req->requestNumber = line_counter;
        action->queue.enqueue(*req);
        delete req;

        ComDataReq_t &r = action->queue.head();//DEBUG
        cout<<r.requestNumber;// DEBUG

        action->a = eSend;
    }else{
        action->a = eNext;
    }
    return action;
}

//Line motion
mito::Action_t*
ComdataProxy::sendG1Line(sG1_t *data)
{
    //TOD
    mito::Action_t *action = new mito::Action_t;

    line_counter++;
#if DEBUGLEVEL==2
    qDebug()<<__FILE__<<__LINE__<<"G1:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\tz:"<<data->z;
#endif
    //setParam_coord
    coordinatus->setWorkValue(X_AXIS, data->x);
    coordinatus->setWorkValue(Y_AXIS, data->y);
    coordinatus->setWorkValue(Z_AXIS, data->z);
    coordinatus->setWorkValue(E_AXIS, data->e);
    // line number
    //buildG0command
    if(!isPlaneHasSteps())
    {
        action->a = eNext;
        return action;
    }
    coordinatus->moveNextToCurrent();
    coordinatus->moveWorkToNext();


#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"DIFF:"<<"X:"<<diff(X) <<"\tY:"<<diff(Y)<<"\tZ:"<<diff(Z)<<"\tE:"<<diff(E)<<"\tline:"<<data->n;
#endif
    bool build = controller->buildBlock(coordinatus);
    if(build==true){
        //    ComDataReq_t* req = buildComdata(data->n);
        RequestFactory *factory = new RequestFactory();
        ComDataReq_t* req = factory->build(data->n);
//        req->requestNumber = line_counter;
        action->queue.enqueue(*req);
        delete req;


        ComDataReq_t &r = action->queue.head();//DEBUG
        cout<<r.requestNumber;// DEBUG

        action->a = eSend;
    }else{
        action->a = eNext;
    }
    return action;
}

//Circle motion
void ComdataProxy::sendG2Line(sG2_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G2:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\ti:"<<data->i<<"\tj:"<<data->j;
#endif
}

//Circle motion
void ComdataProxy::sendG3Line(sG3_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G3:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\ti:"<<data->i<<"\tj:"<<data->j;
#endif
}

//Wait param
void ComdataProxy::sendG4Tag(sG4_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G4:"<<"p:"<<data->p <<"\ts:"<<data->s;
#endif
}

//Line motion
void ComdataProxy::sendG6Tag(sG6_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G6:"<<"a:"<<data->a <<"\tb:"<<data->b<<"\tc:"<<data->c<<"\tr:"<<data->r;
#endif

}

//Set param
void ComdataProxy::sendG10Tag(sG10_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G10:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z <<"\tp:"<<data->p;
#endif

}

//Set param
mito::Action_t*
ComdataProxy::sendG20_21Tag(sG20_21_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    coordinatus->setUnits(data->a);
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G20:G21:"<<data->a;
#endif
    action->a = eNext;
    return action;
}

//Line motion
mito::Action_t*
ComdataProxy::sendG28Tag(sG28_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    //G28: Move to Origin (Home)
    // Если по данным Coordinatus инструмент находится в нуле, то действий не требуется.
    // Если Coordinatus не в нуле, то сформировать запрос для перемещения инструмента в начало.
    // Предполагается, что калибровка выполнена.
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G28:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;
#endif
    double_t sum = 0;
    for (size_t i=0;i<3;i++){
        sum += coordinatus->getCurrentValue(i);
    }

    if(sum > 0)
    {
        coordinatus->moveCurrentToWork();
        action->a = eSend;
        //TODO build request.
        if(data->x == true)
            coordinatus->setWorkValue(X_AXIS, 0.0);
        if(data->y == true)
            coordinatus->setWorkValue(Y_AXIS, 0.0);
        if(data->z == true)
            coordinatus->setWorkValue(Z_AXIS, 0.0);

        //prepare move
        coordinatus->moveWorkToNext();
        bool build = controller->buildBlock(coordinatus);
        if(build == true){

            RequestFactory* factory = new RequestFactory();
            ComDataReq_t* request = factory->build(data->n);
            action->queue.enqueue(*request);
        }else{
            action->a = eNext;
        }
    }else{
        action->a = eNext;
    }
    return action;
}

//Calibration
void ComdataProxy::sendG29_1Tag(sG29_1_t *data)
{
    //TODO
    line_counter++;
    qDebug()<<__FILE__<<__LINE__<<"G29.1:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;

}

//Set param
void ComdataProxy::sendG29_2Tag(sG29_2_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G29.2:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;
#endif
}

//Set param
void ComdataProxy::sendG30Tag(sG30_t *data)
{
 // TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G30:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;
#endif
}

//Seet param
// G90 and G91
mito::Action_t*
ComdataProxy::sendG90_Tag(sG90_t *data)
{
    // false - Relative; true - Absolute.
    mito::Action_t* action = new mito::Action_t;

    line_counter++;
    Coordinatus *crd = Coordinatus::instance();
    crd->setAbsolute(data->value);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G90:"<<  (data->value == true?"Absolute":"Relative" )<<":"<<crd->isAbsolute();
#endif
    action->a = eNext;
    return action;
}


//Set param
mito::Action_t*
ComdataProxy::sendG92Tag(sG92_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    line_counter++;
//    qDebug()<<__FILE__<<__LINE__<<"G92:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z <<"\te:"<<data->e ;
    coordinatus->moveCurrentToWork();
    if( !std::isnan(data->x) )
        coordinatus->setWorkValue(X_AXIS,data->x);
    if( !std::isnan(data->y))
        coordinatus->setWorkValue(Y_AXIS,data->y);
    if(!std::isnan(data->z))
        coordinatus->setWorkValue(Z_AXIS,data->z);
    if(!std::isnan(data->e))
        coordinatus->setWorkValue(E_AXIS, data->e);
    coordinatus->moveWorkToNext();
    coordinatus->moveNextToCurrent();
    controller->uploadPosition(coordinatus);

    action->a = eNext;
    return action;
}

mito::Action_t*
ComdataProxy::sendM104Tag(sM104_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    line_counter++;
    //M104: Set Extruder Temperature
//    coordinatus->setTemperature(data->s);
    sHotendControl_t *hend = coordinatus->getHotend();
    hend->temperature = static_cast<int32_t>(data->s*10);

#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M104:"<< data->s<<"coordinatus:"<<hend->temperature/10;
#endif
    line_counter++;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    action->queue.enqueue(*request);
    action->a = eSend;//TODO

    return action;
}

//Set param send
mito::Action_t*
ComdataProxy::sendM106_Tag(sM106_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    Coordinatus *crd = Coordinatus::instance();
//    crd->setFan_value(data->s);
    sHotendControl_t *hend = crd->getHotend();
    hend->_switch.cooler = static_cast<uint16_t>(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"GM106:"<<"s:"<< data->s<<"crd:"<<hend->_switch.cooler ;
#endif
    //Fan speed (0 to 255; RepRapFirmware also accepts 0.0 to 1.0))
    // TODO Send Command
    line_counter++;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    action->queue.enqueue(*request);
    action->a = eSend;//TODO

    return action;
}

//Set param send
//Deprecated in Teacup firmware and in RepRapFirmware. Use M106 S0 instead.
// M107: Fan Off
//void
mito::Action_t*
ComdataProxy::sendM107_Tag(sM106_t *data)
{
        sHotendControl_t *hend = coordinatus->getHotend();
        hend->_switch.cooler = static_cast<uint16_t>(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M107:"<< data->s<<"crd:"<<hend->_switch.cooler;
#endif
    //TODO send Command
    line_counter++;

    mito::Action_t* action = new mito::Action_t;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    action->queue.enqueue(*request);
    action->a = eSend;//TODO

    return action;
}

//Wait param
//void
mito::Action_t*
ComdataProxy::sendM109_Tag(sM109_t *data)
{
    //TODO
    line_counter++;
    //M109: Set Extruder Temperature and Wait
//    Coordinatus *crd = Coordinatus::instance();
//    crd->setTemperature(data->s);
    sHotendControl_t *hend = coordinatus->getHotend();
    hend->temperature = static_cast<int32_t>(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M109:"<< data->s<<"crd:"<<hend->temperature;
#endif
    mito::Action_t* action = new mito::Action_t;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    action->queue.enqueue(*request);
    action->a = eSendWait;
    action->param.d = data->s;

    return action;
}

//Wait param
mito::Action_t*
ComdataProxy::sendM190_Tag(sM190_t* data)// comproxy->sendM190_Tag(vTag);
{
    line_counter++;
    //bedTemperaure
    mito::Action_t* action = new mito::Action_t;
    ComDataReq_t* request = new ComDataReq_t;
//    RequestFactory* factory = new RequestFactory();
//    factory->build(request, eoHotendControl, hend);
//    factory->build(request,eoState);
    request->command.order = bedTemperaure;
    request->payload.instrument2_paramter = static_cast<uint32_t>(data->s);
    request->size = sizeof(struct ComDataReq_t);

    action->queue.enqueue(*request);
//    action->a = eSendWait;//TODO
    action->a = eNext;
    action->param.d = data->s;

    return action;
}

//Set param
mito::Action_t*
ComdataProxy::sendM82_Tag(sM82_t *data)
{
    //M82: Set extruder to absolute mode
    mito::Action_t* action = new mito::Action_t;
    line_counter++;
    coordinatus->setExtruder_mode(data->a);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M82:"<< coordinatus->getExtruder_mode();
#endif

    action->a = eNext;
    return action;
}

//Set param
void ComdataProxy::sendM84_Tag(sM84_t *data)
{
    //M84: Stop idle hold
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M84:";
#endif
}

//M83: Set extruder to relative mode


uint ComdataProxy::getLine_counter() const
{
    return line_counter;
}

bool ComdataProxy::isPlaneHasSteps()
{
    double_t sum = 0.0;
    for(size_t i=0; i<N_AXIS;i++)
    {
        sum +=  fabs(coordinatus->getWorkvalue(i) - coordinatus->getNextValue(i));
    }
    return (sum != 0.0);
}


ComDataReq_t*
ComdataProxy::buildComdata(uint linenumber)
{
//    ComDataReq_t* req = &request;
    ComDataReq_t* req = new ComDataReq_t;
    sSegment* segment;
    sControl* control;
    block_state_t* bstates = coordinatus->nextBlocks;

    memset(req,0,sizeof(ComDataReq_t));

//	req->requestNumber = 0;//++MyGlobal::requestIndex;// TODO get request number
    req->instruments = 1;

    req->command.order = eoSegment;
    req->command.instrument = 1;
    req->command.reserved = 0;

    segment = &req->payload.instrument1_parameter;

    //------------ payload =============================
    segment->head.linenumber = linenumber;//sgCode->line ;
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
    cout<<segment->axis[X_AXIS].steps<<"\t"<<segment->axis[Y_AXIS].steps<<"\tline"<<segment->head.linenumber;
    return req;
}




