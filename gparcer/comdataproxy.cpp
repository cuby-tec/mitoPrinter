#include "comdataproxy.h"

#include <QDebug>

#define DEBUGLEVEL  1

ComdataProxy::ComdataProxy(QObject *parent) : QObject(parent)
  , line_counter(0)
{
    coordinatus = Coordinatus::instance();
}

//Line motion
void ComdataProxy::sendG0Line(sG0_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==2
    qDebug()<<__FILE__<<__LINE__ <<"G0:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\tz:"<<data->z;
#endif

}

//Line motion
void ComdataProxy::sendG1Line(sG1_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==2
    qDebug()<<__FILE__<<__LINE__<<"G1:"<<"x:"<<data->x <<"\ty:"<<data->y<<"\tz:"<<data->z;
#endif
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
void ComdataProxy::sendG3Tag(sG4_t *data)
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
void ComdataProxy::sendG20_21Tag(sG20_21_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G20:G21:"<<data->a;
#endif
}

//Line motion
void ComdataProxy::sendG28Tag(sG28_t *data)
{
    //TODO
    line_counter++;
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G28:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z ;
#endif
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
void ComdataProxy::sendG90_Tag(sG90_t *data)
{
    // false - Relative; true - Absolute.
     // TODO
    line_counter++;
    Coordinatus *crd = Coordinatus::instance();
    crd->setAbsolute(data->value);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"G90:"<<  (data->value == true?"Absolute":"Relative" )<<":"<<crd->isAbsolute();
#endif
}

//Set param
void ComdataProxy::sendG91_Tag(sG90_t *data)
{
    // false - Relative; true - Absolute.
     // TODO
    line_counter++;
    Coordinatus *crd = Coordinatus::instance();
    crd->setAbsolute(data->value);
#if DEBUGLEVEL == 1
     qDebug()<<__FILE__<<__LINE__<<"G91:"<<  (crd->isAbsolute() == true?"Absolute":"Relative" );
#endif
}

//Set param send
void ComdataProxy::sendM106_Tag(sM106_t *data)
{
    Coordinatus *crd = Coordinatus::instance();
    crd->setFan_value(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"GM106:"<<"s:"<< data->s<<"crd:"<<crd->getFan_value() ;
#endif
    //Fan speed (0 to 255; RepRapFirmware also accepts 0.0 to 1.0))
    // TODO Send Command
    line_counter++;

}

//Set param
void ComdataProxy::sendG92Tag(sG92_t *data)
{
    //TODO
    line_counter++;
//    qDebug()<<__FILE__<<__LINE__<<"G92:" <<"x:"<<data->x <<"\ty:"<<data->y <<"\tz:"<<data->z <<"\te:"<<data->e ;

}

uint ComdataProxy::getLine_counter() const
{
    return line_counter;
}


//Set param send
//Deprecated in Teacup firmware and in RepRapFirmware. Use M106 S0 instead.
void ComdataProxy::sendM107_Tag(sM106_t *data)
{
//    Coordinatus *crd = Coordinatus::instance();
        coordinatus->setFan_value(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M107:"<< data->s<<"crd:"<<coordinatus->getFan_value();
#endif
    //TODO send Command
    line_counter++;
}
//Wait param
void ComdataProxy::sendM109_Tag(sM109_t *data)
{
    //TODO
    line_counter++;
    //M109: Set Extruder Temperature and Wait
    Coordinatus *crd = Coordinatus::instance();
    crd->setTemperature(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M109:"<< data->s<<"crd:"<<crd->getTemperature();
#endif
}

void ComdataProxy::sendM104Tag(sM104_t *data)
{
    //TODO
    line_counter++;
    //M104: Set Extruder Temperature
    coordinatus->setTemperature(data->s);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M104:"<< data->s<<"coordinatus:"<<coordinatus->getTemperature();
#endif

}
//Set param
void ComdataProxy::sendM82_Tag(sM82_t *data)
{
    //M82: Set extruder to absolute mode
    line_counter++;
    Coordinatus *crd = Coordinatus::instance();
    crd->setExtruder_mode(true);
#if DEBUGLEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"M82:"<< crd->getExtruder_mode();
#endif
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

