#include "comdataproxy.h"
#include "links/ComDataReq_t.h"
#include "geometry/Arc.h"

#include "mitoaction.h"
#include "profiles/profile.h"
#include "step_motor/a4988.h"

#include <QDebug>
#include <cmath>


#define DEBUGLEVEL  1

#define cout qDebug()<<__FILE__<<__LINE__
//#define diff(M) coordinatus->getCurrentValue(M##_AXIS) - coordinatus->getNextValue(M##_AXIS)
#define diff(M) coordinatus->getNextValue(M##_AXIS) - coordinatus->getCurrentValue(M##_AXIS)

//--------------function

ComdataProxy::ComdataProxy(QObject *parent) : QObject(parent)
  , line_counter(0)
  , controller(new Controller)
{
    coordinatus = Coordinatus::instance();
}


void
ComdataProxy::_setMicrosteps(ComDataReq_t* req)
{
    struct sSegment* segment = &req->payload.instrument1_parameter;

	for(uint i=0; i<N_AXIS;i++){
		uint32_t ms = coordinatus->getMicrostep(i);
        segment->axis[i].microsteps = microstepTable[ms];
	}

}




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
    coordinatus->setSpeedrate(data->f);

    if(!isPlaneHasSteps())
    {
        action->a = eNext;
        return action;
    }
    coordinatus->moveNextToCurrent();
    coordinatus->moveWorkToNext();

#if DEBUGLEVEL==2
    qDebug()<<__FILE__<<__LINE__<<"DIFF:"<<"X:"<<diff(X) <<"\tY:"<<diff(Y)<<"\tZ:"<<diff(Z);
#endif
    bool build = controller->buildBlock(coordinatus);

    if(build == true){
        RequestFactory *factory = new RequestFactory();
        //    buildComdata(data->n);
        ComDataReq_t* req = factory->build(data->n);
//        req->requestNumber = line_counter;
        _setMicrosteps(req);
        action->queue.enqueue(*req);
        delete req;

        ComDataReq_t &r = action->queue.head();//DEBUG
#if DEBUGLEVEL==2
        cout<<r.requestNumber;// DEBUG
#endif
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
    if(data->f != 0.0)
        coordinatus->setSpeedrate(data->f);
    // line number
    //buildG0command
    if(!isPlaneHasSteps())
    {
        action->a = eNext;
        return action;
    }
    coordinatus->moveNextToCurrent();
    coordinatus->moveWorkToNext();


#if LEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"DIFF:"<<"X:"<<diff(X) <<"\tY:"<<diff(Y)<<"\tZ:"<<diff(Z)<<"\tE:"<<diff(E)<<"\tline:"<<data->n;
#endif
    bool build = controller->buildBlock(coordinatus);
    if(build==true){
        //    ComDataReq_t* req = buildComdata(data->n);
        RequestFactory *factory = new RequestFactory();
        ComDataReq_t* req = factory->build(data->n);
//        req->requestNumber = line_counter;
        _setMicrosteps(req);
        action->queue.enqueue(*req);
        delete req;


        ComDataReq_t &r = action->queue.head();//DEBUG
#if LEVEL==1
        cout<<r.requestNumber;// DEBUG
#endif
        action->a = eSend;
    }else{
        action->a = eNext;
    }
    return action;
}

//Circle motion
mito::Action_t*
ComdataProxy::sendG2Line(sG2_t *data)
{
    //TODO feedrate
    mito::Action_t *action = new mito::Action_t;
    RequestFactory *factory = new RequestFactory();

    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G2:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\ti:"<<data->i<<"\tj:"<<data->j;
#endif

// ARC
    Arc* arc = new Arc();
    Point* start = new Point(coordinatus->getNextValue(X_AXIS),coordinatus->getNextValue(Y_AXIS));
    arc->setStart(*start);
    Point end(data->x,data->y);
    arc->setEnd(end);
    Point center(data->i,data->j);
    arc->setCenter(center);

    //TODO precicion for X and Y, difference.
    double_t precicion = controller->getPrecicion(X_AXIS);
    arc->setPrecicion(precicion);

    arc->calculate();

    coordinatus->moveNextToCurrent();

    Point pStart = arc->getStart();
    Point pCurrent = *start;
    int32_t d20[3][2] = {{0,0},{0,0},{0,0}};
    double_t e_point = coordinatus->getCurrentValue(E_AXIS);
    double_t delta_e = fabs((data->e - e_point)/arc->getPointsNumber());

    for(uint32_t i=1;i<arc->getPointsNumber();i++){
        Point p = arc->getPoint(i); // Line 611
        Point dp = p - *start;

        d20[0][0] = static_cast<int32_t>( trunc(dp.x/precicion));
        d20[0][1] = static_cast<int32_t>( trunc(dp.y/precicion));

//        bool h20 = d20[0][0] ^ d20[1][0];
//        bool i20 = d20[0][1] ^ d20[1][1];

        d20[2][0] = d20[0][0] - d20[1][0];
        d20[2][1] = d20[0][1] - d20[1][1];

        d20[1][0] = d20[0][0];
        d20[1][1] = d20[0][1];

//        if((!h20)&&(!i20))
        if((d20[2][0]==0)&&(d20[2][1]==0))
            continue;

        coordinatus->setWorkValue(X_AXIS,p.x);
        coordinatus->setWorkValue(Y_AXIS,p.y);
        coordinatus->setWorkValue(E_AXIS,e_point + delta_e*i);
        coordinatus->moveWorkToNext();
//        controller->buildCircleStep(coordinatus);
        bool build = controller->buildBlock(coordinatus);

        if(build==true){
            //    buildComdata(data->n);
            ComDataReq_t* req = factory->build(data->n);
            req->requestNumber = line_counter;
            action->queue.enqueue(*req);
            delete req;

//            ComDataReq_t &r = action->queue.head();//DEBUG
//            cout<<r.requestNumber;// DEBUG

        }
    }
    // arc
    action->a = eSend;
    return action;
}

//Circle motion
mito::Action_t*
ComdataProxy::sendG3Line(sG3_t *data)
{
    mito::Action_t *action = new mito::Action_t;
    RequestFactory *factory = new RequestFactory();
    line_counter++;

#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G3:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\ti:"<<data->i<<"\tj:"<<data->j;
#endif

// ARC
    Arc* arc = new Arc();
    arc->setClockWise(COUNTERCLOCKWISE);
    Point* start = new Point(coordinatus->getNextValue(X_AXIS),coordinatus->getNextValue(Y_AXIS));
    arc->setStart(*start);
    Point end(data->x,data->y);
    arc->setEnd(end);
    Point center(data->i,data->j);
    arc->setCenter(center);

    //TODO precicion for X and Y, difference.
    double_t precicion = controller->getPrecicion(X_AXIS);
    arc->setPrecicion(precicion);

    arc->calculate();

    coordinatus->moveNextToCurrent();

    Point pStart = arc->getStart();
    Point pCurrent = *start;
    int32_t d20[3][2] = {{0,0},{0,0},{0,0}};
    double_t e_point = coordinatus->getCurrentValue(E_AXIS);
    double_t delta_e = fabs((data->e - e_point)/arc->getPointsNumber());

    for(uint32_t i=1;i<arc->getPointsNumber();i++){
        Point p = arc->getPoint(i); // Line 611
        Point dp = p - *start;

        d20[0][0] = static_cast<int32_t>( trunc(dp.x/precicion));
        d20[0][1] = static_cast<int32_t>( trunc(dp.y/precicion));

//        bool h20 = d20[0][0] ^ d20[1][0];
//        bool i20 = d20[0][1] ^ d20[1][1];

        d20[2][0] = d20[0][0] - d20[1][0];
        d20[2][1] = d20[0][1] - d20[1][1];

        d20[1][0] = d20[0][0];
        d20[1][1] = d20[0][1];

//        if((!h20)&&(!i20))
        if((d20[2][0]==0)&&(d20[2][1]==0))
            continue;

        coordinatus->setWorkValue(X_AXIS,p.x);
        coordinatus->setWorkValue(Y_AXIS,p.y);
        coordinatus->setWorkValue(E_AXIS,e_point + delta_e*i);
        coordinatus->moveWorkToNext();
//        controller->buildCircleStep(coordinatus);
        bool build = controller->buildBlock(coordinatus);

        if(build==true){
            //    buildComdata(data->n);
            ComDataReq_t* req = factory->build(data->n);
            req->requestNumber = line_counter;
            action->queue.enqueue(*req);
            delete req;

//            ComDataReq_t &r = action->queue.head();//DEBUG
//            cout<<r.requestNumber;// DEBUG

        }
    }
//arc
    action->a = eSend;
    return action;
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
#if LEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G28:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;
#endif
    double_t sum = 0;
    for (size_t i=0;i<3;i++){
        sum += coordinatus->getNextValue(i);
    }

    coordinatus->moveNextToCurrent();

    if(sum > 0)
    {
        coordinatus->moveCurrentToWork();
        action->a = eSend;
        //TODO build request.
//        if(data->x == true)
        coordinatus->setWorkValue(X_AXIS, data->x);
//        if(data->y == true)
        coordinatus->setWorkValue(Y_AXIS, data->y);
//        if(data->z == true)
        coordinatus->setWorkValue(Z_AXIS, data->z);

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
ComdataProxy::sendG92Tag(sMover *data)
{
    mito::Action_t* action = new mito::Action_t;
    line_counter++;
//    qDebug()<<__FILE__<<__LINE__<<"G92:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z <<"\te:"<<data->e ;
    coordinatus->moveNextToCurrent();
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

    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->buildTag92(request, data);
//    factory->build(request, eoG92, data);
    action->queue.enqueue(*request);
    action->a = eWaitSend;
//    action->a = eNext;
    return action;
}

//M104 tag
#define KD  0.12
#define KP  0.8//0.75
#define KI  1.0 - (KP + KD)
mito::Action_t*
ComdataProxy::sendM104Tag(sM104_t *data)
{
    mito::Action_t* action = new mito::Action_t;
    line_counter++;
    //M104: Set Extruder Temperature
//    coordinatus->setTemperature(data->s);
    sHotendControl_t *hend = coordinatus->getHotend();
    hend->temperature = static_cast<int32_t>(data->s*10);
    if(data->s == 0.0)
        hend->_switch.heater = 0;
    else
        hend->_switch.heater = 1;

//    double_t hdata = KD;
//    hend->kd = static_cast<int32_t>(hdata*1000);
//    hdata = KP;
//    hend->kp = static_cast<int32_t>(hdata*1000);
//    hdata = KI;
//    hend->ki = static_cast<int32_t>(hdata*1000);
    loadHotendFromProfile(hend);

#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M104:"<< data->s<<"coordinatus:"<<hend->temperature/10;
#endif
//    line_counter++;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    request->payload.instrument_hotend.head.linenumber = data->n;
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
#if DEBUGLEVEL==2
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
    hend->temperature = static_cast<int32_t>(data->s*10);
    if(data->s == 0.0)
        hend->_switch.heater = 0;
    else
        hend->_switch.heater = 1;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M109:"<< data->s<<"crd:"<<hend->temperature;
#endif
    mito::Action_t* action = new mito::Action_t;
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoHotendControl, hend);
    request->payload.instrument_hotend.head.linenumber = data->n;
    action->queue.enqueue(*request);
    action->a = eSendWait;
    action->param.d = data->s;

    return action;
}

mito::Action_t*
ComdataProxy::sendM140_Tag(sM140_t *data)
{
    line_counter++;
    mito::Action_t* action = new mito::Action_t;
    ComDataReq_t* request = new ComDataReq_t;
    request->command.order = bedTemperaure;
//    request->payload.instrument2_paramter = static_cast<uint32_t>(data->s*10);
    request->size = sizeof(struct ComDataReq_t);
    sBedControl_t* bedcontrol = reinterpret_cast<sBedControl_t*>(&request->payload.bedcontrol);
    bedcontrol->temperature = static_cast<int32_t>(data->s*10);
//    bedcontrol->kd = 123;
//    bedcontrol->ki = 234;
//    bedcontrol->kp = 345;
    loadBedFromProfile(bedcontrol);
    action->queue.enqueue(*request);
    action->a = eSend;

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
mito::Action_t*
ComdataProxy::sendM84_Tag(sM84_t *data)
{
    //M84: Stop idle hold
	mito::Action_t* action = new mito::Action_t;
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M84:";
#endif
    // tag M84 ; Turn steppers off
    ComDataReq_t* request = new ComDataReq_t;
    RequestFactory* factory = new RequestFactory();
    factory->build(request, eoM84, data);
    action->queue.enqueue(*request);
    action->a = eWaitSend;//
    return action;
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

void ComdataProxy::loadHotendFromProfile(sHotendControl_t *shc)
{
    bool ok;
    double_t datap;
    Profile* profile = Profile::instance();
    QString strp = profile->get_PROPTIONAL();
    datap = strp.toDouble(&ok);
    shc->kp = static_cast<int32_t>(datap*1000);

    strp = profile->get_INTEGRAL();
    datap = strp.toDouble(&ok);
    shc->ki = static_cast<int32_t>(datap*1000);

    strp = profile->get_DERIVATIVE();
    datap = strp.toDouble(&ok);
    shc->kd = static_cast<int32_t>(datap*1000);

}

void
ComdataProxy::loadBedFromProfile(sBedControl_t* shc)
{
    bool ok;
    double_t datap;
    Profile* profile = Profile::instance();
    QString strp = profile->get_BED_PROPTIONAL();
    datap = strp.toDouble(&ok);
    shc->kp = static_cast<int32_t>(datap*1000);

    strp = profile->get_BED_INTEGRAL();
    datap = strp.toDouble(&ok);
    shc->ki = static_cast<int32_t>(datap*1000);

    strp = profile->get_BED_DERIVATIVE();
    datap = strp.toDouble(&ok);
    shc->kd = static_cast<int32_t>(datap*1000);
}


