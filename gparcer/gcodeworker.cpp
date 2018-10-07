/**
  File : gcodeworker.c
  Company: CUBY,Ltd
  date: 25.05.2018
 */


#include "gcodeworker.h"

#include <string.h>
//#include <cstring>

#include <QDebug>

//#include "gParcer.h"
//#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtGlobal>
//#include "gparcer/comdata.h"
#include <string.h>


/*
 *     eG0=0, eG1, eG2, eG3, eG4, eG6, eG10, eG28, eG29_1, eG29_2, eG30, eG33
    , eG90, eG91, eG92, eG92_1, eM82, eM84, eM104, eM106, eM107, eM109, eM140, eM190, eM550
    , eF, eS, eT
 */

GcodeWorker::GcodeWorker(QObject *parent) : QObject(parent)
  , arraytag (new ArrayTag)
  , errorCounter(0)
  , comproxy(new ComdataProxy)
  , linecounter(0)
  , lexer( new Lexer(dst))

{
    callTagRef[eG0] = &GcodeWorker::tagG0_Do;
    callTagRef[eG1] = &GcodeWorker::tagG1_Do;
    callTagRef[eG2] = &GcodeWorker::tagG2_Do;
    callTagRef[eG3] = &GcodeWorker::tagG3_Do;
    callTagRef[eG4] = &GcodeWorker::tagG4_Do;
    callTagRef[eG6] = &GcodeWorker::tagG6_Do;
    callTagRef[eG10] = &GcodeWorker::tagG10_Do;
    callTagRef[eG20] = &GcodeWorker::tagG20_Do;
    callTagRef[eG21] = &GcodeWorker::tagG21_Do;
    callTagRef[eG28] = &GcodeWorker::tagG28_Do;
    callTagRef[eG29_1] = &GcodeWorker::tagG29_1_Do;
    callTagRef[eG29_2] = &GcodeWorker::tagG29_2_Do;
    callTagRef[eG30] = &GcodeWorker::tagG30_Do;
    callTagRef[eG33] = nullptr;
    callTagRef[eG90] = &GcodeWorker::tagG90_Do;
    callTagRef[eG91] = &GcodeWorker::tagG91_Do;
    callTagRef[eG92] = &GcodeWorker::tagG92_Do;
    callTagRef[eG92_1] = nullptr;
    callTagRef[eM82] = &GcodeWorker::tagM82_Do;
    callTagRef[eM83] = &GcodeWorker::tagM83_Do;
    callTagRef[eM84] = &GcodeWorker::tagM84_Do;
    callTagRef[eM104] = &GcodeWorker::tagM104_Do;
    callTagRef[eM106] = &GcodeWorker::tagM106_Do;
    callTagRef[eM107] = &GcodeWorker::tagM107_Do;
    callTagRef[eM109] = &GcodeWorker::tagM109_Do;
    callTagRef[eM140] = nullptr;
    callTagRef[eM190] = nullptr;
    callTagRef[eM550] = nullptr;
    callTagRef[eF] = &GcodeWorker::tagF_Do;
    callTagRef[eS] = nullptr;
    callTagRef[eT] = nullptr;   //30
#if VERSION==0
    arraytag = ArrayTag::instance();
#else
//    arraytag = new ArrayTag;
#endif

//    comproxy = new ComdataProxy;
#if lEVEL==4
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(queueReady()) );
#endif

}



//void
mito::Action_t*
GcodeWorker::buildAction(sGcode *src)
{
    eGCodeTag etag = eError;
    mito::Action_t* action = nullptr;

//    void *action = nullptr;

    QString tag(src->group);

    if(tag == 'G' || tag =='M'){
        tag.append(src->value);
    }else if (tag == '(' || tag == ";" || QString(tag).isEmpty() ){
        //comment
        return action;
    }

    if(tag == "G0") etag = eG0;
    else if (tag == "G1") etag = eG1;
    else if (tag == "G2") etag = eG2;
    else if (tag == "G3") etag = eG3;
    else if (tag == "G4") etag = eG4;
    else if (tag == "G6") etag = eG6;
    else if (tag == "G10") etag = eG10;
    else if (tag == "G20") etag = eG20;
    else if (tag == "G21") etag = eG21;
    else if (tag == "G28") etag = eG28;
    else if (tag == "G29.1") etag = eG29_1;
    else if (tag == "G29.2") etag = eG29_2;
    else if (tag == "G30") etag = eG30;
    else if (tag == "G33") etag = eG33;
    else if (tag == "G90") etag = eG90;
    else if (tag == "G91") etag = eG91;
    else if (tag == "G92") etag = eG92;
    else if (tag == "G92.1") etag = eG92_1;
    else if (tag == "M104") etag = eM104;
    else if (tag == "M106") etag = eM106;
    else if (tag == "M107") etag = eM107;
    else if (tag =="M109") etag = eM109;
    else if (tag == "M140") etag = eM140;
    else if (tag == "M190") etag = eM190;
    else if (tag == "M550") etag = eM550;
    else if (tag == "M82") etag = eM82;
    else if (tag == "M84") etag = eM84;
    else if (tag == "F") etag = eF;
    else if (tag == "S") etag = eS;
    else if (tag == "T") etag = eT;

    switch (etag) {
    case eG0:
        action = (this->*callTagRef[etag])(src); //tagG0_Do
        break;
    case eG1:
        action = (this->*callTagRef[etag])(src); //tagG1_Do
        break;
    case eG2:
        (this->*callTagRef[etag])(src); //tagG2_Do
        break;
    case eG3:
        (this->*callTagRef[etag])(src); //tagG3_Do
         break;
    case eG4:
        (this->*callTagRef[etag])(src); //tagG4_Do
        break;
    case eG6:
        (this->*callTagRef[etag])(src); //tagG6_Do
        break;
    case eG10:
        (this->*callTagRef[etag])(src); //tagG10_Do
        break;
    case eG20:
        (this->*callTagRef[etag])(src); //tagG20_Do
        break;
    case eG21:
        (this->*callTagRef[etag])(src); //tagG21_Do
        break;
    case eG28:
        action = (this->*callTagRef[etag])(src); //tagG28_Do
        break;
    case eG29_1:
        (this->*callTagRef[etag])(src); //tagG29.1_Do
        break;
    case eG29_2:
        (this->*callTagRef[etag])(src); //tagG29.2_Do
        break;
    case eG30:
        (this->*callTagRef[etag])(src); //tagG30_Do
        break;
    case eG33:
        break;
    case eG90:
        (this->*callTagRef[etag])(src); //tagG90_Do
        break;
    case eG91:
        (this->*callTagRef[etag])(src); //tagG91_Do
        break;
    case eG92:
        (this->*callTagRef[etag])(src); //tagG92_Do
        break;
    case eG92_1:
        break;
    case eM104:
        action = (this->*callTagRef[etag])(src); //tagM104_Do
        break;
    case eM106:
        action = (this->*callTagRef[etag])(src); //tagM106_Do
        break;
    case eM107:
        action = (this->*callTagRef[etag])(src); //tagM107_Do
        break;
    case eM109:
        action = (this->*callTagRef[etag])(src); //tagM109_Do
        break;
    case eM140:
        break;
    case eM190:
        break;
    case eM550:
        break;
    case eM82:  (this->*callTagRef[etag])(src); //tagM82_Do
        break;
    case eM83:  (this->*callTagRef[etag])(src); //tagM83_Do
        break;
    case eM84:  (this->*callTagRef[etag])(src); //tagM84_Do
        break;
    case eF:    (this->*callTagRef[etag])(src); //tagGF_Do
        break;
    case eS:
        break;
    case eT:
        break;

    default:
        qWarning()<<__FILE__<<__LINE__<<"value:"<<etag;
//        qFatal("Wrong index, wrong Profile");
        errorCounter++;
    }


//    qDebug()<<__FILE__<<__LINE__<<":"<<tag;
    return (action);
}

/*
 struct sG0_t{
    double x;     double y;     double z;     double e;    double f;    double s; };
*/
//void
mito::Action_t*
GcodeWorker::tagG0_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG0_t * vTag =  reinterpret_cast<sG0_t *>( arraytag->getTagValue(eG0));
    sG0_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;

    bool ok = false;

    for(int i=0;i<sgCode->param_number;i++)
    {
        sGparam* gparam = &sgCode->param[i];

        QString value(gparam->value);
        value.replace(',','.');
        double dvalue = value.toDouble(&ok);

        switch (gparam->group)
        {
        case 'X':
            Q_ASSERT(ok);
            if(ok) { valueTag.x = dvalue; }
            break;

        case 'Y':
            Q_ASSERT(ok);
            if(ok) { valueTag.y = dvalue; }
            break;

        case 'Z':
            Q_ASSERT(ok);
            if(ok) { valueTag.z = dvalue; }
            break;

        case 'E':
            Q_ASSERT(ok);
            if(ok) { valueTag.e = dvalue; }
            break;

        case 'F':
            Q_ASSERT(ok);
            if(ok) { valueTag.f = dvalue; }
            break;

        case 'S':
            Q_ASSERT(ok);
            if(ok) { valueTag.s = dvalue; }
            break;

        case 'N': // Номер строки
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }

    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    syncXYZ(valueTag.x, valueTag.y, valueTag.z);

    action = comproxy->sendG0Line(vTag);
#endif
//    qDebug()<<__FILE__<<__LINE__<<"tagG0_Do"<<"\tx:"<<vTag->x<<"\ty:"<<vTag->y<<"\tz:"<<vTag->z<<"\te:"<<vTag->e; // Level2

    return action;
}

mito::Action_t*
GcodeWorker::tagG1_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG1_t * vTag =  reinterpret_cast<sG1_t *>( arraytag->getTagValue(eG1));
    sG1_t valueTag ;
    vTag->get(&valueTag);
//    double v;
    valueTag.n = 0;
    bool ok = false;

//    char buf[20];
//    memset(buf,0,sizeof (buf));

    for(int i=0;i<sgCode->param_number;i++)
    {
        sGparam* gparam = &sgCode->param[i];

        QString value(clearNumValue(gparam->value));

        double dvalue = value.toDouble(&ok);
//qDebug()<<__FILE__<<__LINE__<<dvalue;
        switch (gparam->group)
        {
        case 'X':
            Q_ASSERT(ok);
            if(ok) { valueTag.x = dvalue; }
            break;

        case 'Y':
            Q_ASSERT(ok);
            if(ok) { valueTag.y = dvalue; }
            break;

        case 'Z':
            Q_ASSERT(ok);
            if(ok) { valueTag.z = dvalue; }
            break;

        case 'E':
//            value.replace('.',',');
//            v = std::atof(value.toStdString().c_str());
//            strcpy(buf,value.toStdString().c_str());
//            v = std::atof(buf);
//            qDebug()<<__FILE__<<__LINE__<<":"<<buf<<":"<<v;

            Q_ASSERT(ok);
            if(ok) { valueTag.e = dvalue; }
            break;

        case 'F':
            Q_ASSERT(ok);
            if(ok) { valueTag.f = dvalue; }
            break;

        case 'S':
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'N': // Номер строки
            uint number = QString(gparam->value).toUInt(&ok);
//            qDebug()<<__FILE__<<__LINE__<<gparam->value;
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
//        if(!ok) {qDebug()<<__FILE__<<__LINE__<<value; }
    }

    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    syncXYZ(valueTag.x, valueTag.y, valueTag.z);
    action = comproxy->sendG1Line(vTag);
#endif


    //    qDebug()<<__FILE__<<__LINE__;
    return action;
}

mito::Action_t*
GcodeWorker::tagG2_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG2_t * vTag =  reinterpret_cast<sG2_t *>( arraytag->getTagValue(eG2));
    sG2_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;

    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){

        sGparam* gparam = &sgCode->param[i];

        QString value(clearNumValue(gparam->value));

        switch (gparam->group){
        case 'X':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.x = dvalue;
            break;

        case 'Y':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.y = dvalue;
            break;

        case 'E':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.e = dvalue;
            break;

        case 'F':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.f = dvalue;
            break;

        case 'I':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.i = dvalue;
            break;

        case 'J':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.j = dvalue;
            break;

        case 'N':// Номер строки
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;

        }

    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    syncXY(valueTag.x, valueTag.y);
    comproxy->sendG2Line(vTag);
#endif
    return action;
}

mito::Action_t*
GcodeWorker::tagG3_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG3_t * vTag =  reinterpret_cast<sG3_t *>( arraytag->getTagValue(eG3));
    sG3_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){

        sGparam* gparam = &sgCode->param[i];

        QString value(clearNumValue(gparam->value));

        switch (gparam->group){

        case 'X':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.x = dvalue;
            break;

        case 'Y':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.y = dvalue;
            break;

        case 'I':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.i = dvalue;
            break;

        case 'J':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.j = dvalue;
            break;

        case 'E':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.e = dvalue;
            break;

        case 'F':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.f = dvalue;
            break;

        case 'N':// Номер строки
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    syncXY(valueTag.x, valueTag.y);
    comproxy->sendG3Line(vTag);
#endif
    return action;
}

mito::Action_t*
GcodeWorker::tagG4_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG4_t * vTag =  reinterpret_cast<sG4_t *>( arraytag->getTagValue(eG4));
    sG4_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){

        sGparam* gparam = &sgCode->param[i];

        QString value(clearNumValue(gparam->value));

        switch (gparam->group){
        case 'P':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.p = dvalue;
            break;

        case 'S':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'N':// Номер строки
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    comproxy->sendG3Tag(vTag);
#endif
    return action;
}

mito::Action_t*
GcodeWorker::tagG6_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG6_t * vTag =  reinterpret_cast<sG6_t *>( arraytag->getTagValue(eG6));
    sG6_t valueTag ;
    vTag->get(&valueTag);
    vTag->r = false;
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

     for(int i=0;i<sgCode->param_number;i++){
         sGparam* gparam = &sgCode->param[i];
         QString value(clearNumValue(gparam->value));
         switch (gparam->group){
         case 'A':
             dvalue = value.toDouble(&ok);
             Q_ASSERT(ok);
             valueTag.a = dvalue;
             break;

         case 'B':
             dvalue = value.toDouble(&ok);
             Q_ASSERT(ok);
             valueTag.b = dvalue;
             break;

         case 'C':
             dvalue = value.toDouble(&ok);
             Q_ASSERT(ok);
             valueTag.c = dvalue;
             break;

         case 'R': // R Relative move flag
             valueTag.r = true;
             break;

         case 'N':// Номер строки
             uint number = QString(gparam->value).toUInt(&ok);
             Q_ASSERT(ok);
             if(ok) {valueTag.n = number; }
             break;
         }
     }
     if(valueTag.n == 0)
         valueTag.n = linecounter;
     vTag->set(&valueTag);
     comproxy->sendG6Tag(vTag);
#endif
     return action;
}

mito::Action_t*
GcodeWorker::tagG10_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG10_t * vTag =  reinterpret_cast<sG10_t *>( arraytag->getTagValue(eG10));
    sG10_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'X':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.x = dvalue;
            break;

        case 'Y':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.y = dvalue;
            break;

        case 'Z':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.z = dvalue;
            break;

        case 'P':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.p = dvalue;
            break;

        case 'R':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.r = dvalue;
            break;

        case 'S':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'U':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.u = dvalue;
            break;

        case 'V':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.v = dvalue;
            break;

        case 'W':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.w = dvalue;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    comproxy->sendG10Tag(vTag);
#endif
    return action;
}

//G20: Set Units to Inches
mito::Action_t*
GcodeWorker::tagG20_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG20_21_t * vTag =  reinterpret_cast<sG20_21_t *>( arraytag->getTagValue(eG20));
    sG20_21_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;

    valueTag.a = false; // G20: Set Units to Inches

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    comproxy->sendG20_21Tag(vTag);
#endif
    return action;
}

//G21: Set Units to Millimeters
mito::Action_t*
GcodeWorker::tagG21_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG20_21_t * vTag =  reinterpret_cast<sG20_21_t *>( arraytag->getTagValue(eG21));
    sG20_21_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;

    valueTag.a = true; // G21: Set Units to Millimeters

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    comproxy->sendG20_21Tag(vTag);
#endif
    return action;
}


/**
 * @brief GcodeWorker::tagG28_Do
 * @param sgCode
 * @return
 *  This command can be used without any additional parameters.
 * X Flag to go back to the X axis origin
 * Y Flag to go back to the Y axis origin
 * Z Flag to go back to the Z axis origin
 *
 */
mito::Action_t*
GcodeWorker::tagG28_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
//    sG28_t * vTag =  reinterpret_cast<sG28_t *>( arraytag->getTagValue(eG28));
    sG28_t valueTag ;
//    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
//    double dvalue;

//    if(sgCode->param_number>0){
        valueTag.reset();
//    }

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'X':
            valueTag.x = true;
            break;

        case 'Y':
            valueTag.y = true;
            break;

        case 'Z':
            valueTag.z = true;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }

    if(valueTag.n == 0)
        valueTag.n = linecounter;
    if(valueTag.x==false && valueTag.y==false && valueTag.z==false){
        valueTag.x = true; valueTag.y=true; valueTag.z=true;
    }
    action = comproxy->sendG28Tag(&valueTag);
#endif
    return action;
}

//Set Z probe head offset
mito::Action_t*
GcodeWorker::tagG29_1_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG29_1_t * vTag =  reinterpret_cast<sG29_1_t *>( arraytag->getTagValue(eG29_1));
    sG29_1_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

        for(int i=0;i<sgCode->param_number;i++){
            sGparam* gparam = &sgCode->param[i];
            QString value(clearNumValue(gparam->value));
            switch (gparam->group){
            case 'X':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.x = dvalue;
                break;

            case 'Y':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.y = dvalue;
                break;

            case 'Z':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.z = dvalue;
                break;

            case 'N':
                uint number = QString(gparam->value).toUInt(&ok);
                Q_ASSERT(ok);
                if(ok) {valueTag.n = number; }
                break;
            }
        }

        if(valueTag.n == 0)
            valueTag.n = linecounter;
        vTag->set(&valueTag);

        comproxy->sendG29_1Tag(vTag);
#endif
        return action;
}

mito::Action_t*
GcodeWorker::tagG29_2_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG29_2_t * vTag =  reinterpret_cast<sG29_2_t *>( arraytag->getTagValue(eG29_2));
    sG29_2_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

        for(int i=0;i<sgCode->param_number;i++){
            sGparam* gparam = &sgCode->param[i];
            QString value(clearNumValue(gparam->value));
            switch (gparam->group){
            case 'X':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.x = dvalue;
                break;

            case 'Y':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.y = dvalue;
                break;

            case 'Z':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.z = dvalue;
                break;

            case 'N':
                uint number = QString(gparam->value).toUInt(&ok);
                Q_ASSERT(ok);
                if(ok) {valueTag.n = number; }
                break;
            }
        }
        if(valueTag.n == 0)
            valueTag.n = linecounter;
        vTag->set(&valueTag);

        comproxy->sendG29_2Tag(vTag);
#endif

        return action;
}

//Single Z-Probe
// 5181-5189
// "G30" Home for X, Y, Z, A, B, C, U, V & W. Persistent.
//http://linuxcnc.org/docs/2.6/html/gcode/overview.html#sub:numbered-parameters
mito::Action_t*
GcodeWorker::tagG30_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG30_t * vTag =  reinterpret_cast<sG30_t *>( arraytag->getTagValue(eG30));
    sG30_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

        for(int i=0;i<sgCode->param_number;i++){
            sGparam* gparam = &sgCode->param[i];
            QString value(clearNumValue(gparam->value));
            switch (gparam->group){
            case 'X':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.x = dvalue;
                break;

            case 'Y':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.y = dvalue;
                break;

            case 'Z':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.z = dvalue;
                break;
            case 'A':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.a = dvalue;
                break;

            case 'D':
                valueTag.d = true;
                break;

            case 'E':
                valueTag.e = true;
                break;

            case 'H':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.h = dvalue;
                break;

            case 'I':
                valueTag.i = true;
                break;

            case 'P':
                dvalue = value.toDouble(&ok);
                Q_ASSERT(ok);
                valueTag.p = dvalue;
                break;

            case 'R':
                valueTag.r = true;
                break;

            case 'S':
                valueTag.s = true;
                break;

            case 'T':
                valueTag.t = true;
                break;

            case 'U':
                valueTag.u = true;
                break;

            case 'N':
                uint number = QString(gparam->value).toUInt(&ok);
                Q_ASSERT(ok);
                if(ok) {valueTag.n = number; }
                break;
            }
        }
        if(valueTag.n == 0)
            valueTag.n = linecounter;
        vTag->set(&valueTag);

        comproxy->sendG30Tag(vTag);
#endif
        return action;
}

// STUB
mito::Action_t*
GcodeWorker::tagG33_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
#if VERSION==1
    sG33_t * vTag =  reinterpret_cast<sG33_t *>( arraytag->getTagValue(eG33));
    sG33_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'C':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.c = dvalue;
            break;

        case 'E':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.e = dvalue;
            break;

        case 'F':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.f = dvalue;
            break;

        case 'P':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.p = dvalue;
            break;

        case 'S':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'T':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.t = dvalue;
            break;

        case 'V':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.v = dvalue;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);

//    comproxy->sendG30Tag(vTag);
#endif
    return action;
}

//Set to Absolute Positioning
mito::Action_t*
GcodeWorker::tagG90_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sG90_t * vTag =  reinterpret_cast<sG90_t *>( arraytag->getTagValue(eG90));
//    sG90_t valueTag ;
//    vTag->get(&valueTag);
    vTag->value = true;
    vTag->n = 0;
    bool ok = false;
    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {vTag->n = number; }
            break;
        }

    }
    if(vTag->n == 0)
        vTag->n = linecounter;
    comproxy->sendG90_Tag(vTag);
    return action;
}

//Set to Relative Positioning
mito::Action_t*
GcodeWorker::tagG91_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sG90_t * vTag =  reinterpret_cast<sG90_t *>( arraytag->getTagValue(eG90));
//    sG90_t valueTag ;
//    vTag->get(&valueTag);
    vTag->value = false;
    vTag->n = 0;
    bool ok = false;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {vTag->n = number; }
            break;
        }

    }
    if(vTag->n == 0)
        vTag->n = linecounter;

    comproxy->sendG90_Tag(vTag);
    return action;
}

mito::Action_t*
GcodeWorker::tagG92_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sG92_t * vTag =  reinterpret_cast<sG92_t *>( arraytag->getTagValue(eG92));
    sG92_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'X':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.x = dvalue;
            break;

        case 'Y':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.y = dvalue;
            break;

        case 'Z':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.z = dvalue;
            break;
        case 'E':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.e = dvalue;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }

    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);

    comproxy->sendG92Tag(vTag);
    return action;
}

mito::Action_t*
GcodeWorker::tagG92_1_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sG92_1_t * vTag =  reinterpret_cast<sG92_1_t *>( arraytag->getTagValue(eG92_1));
     sG92_1_t valueTag ;
//     vTag->get(&valueTag);
     bool ok = false;
     double dvalue;
      //TODO
     return action;
}

 //M104: Set Extruder Temperature
mito::Action_t*
GcodeWorker::tagM104_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM104_t * vTag =  reinterpret_cast<sM104_t *>( arraytag->getTagValue(eM104));
    sM104_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;
     for(int i=0;i<sgCode->param_number;i++){
         sGparam* gparam = &sgCode->param[i];
         QString value(clearNumValue(gparam->value));
         switch (gparam->group){
         case 'R':
             dvalue = value.toDouble(&ok);
             Q_ASSERT(ok);
             valueTag.r = dvalue;
             break;

         case 'S':
             dvalue = value.toDouble(&ok);
             Q_ASSERT(ok);
             valueTag.s = dvalue;
             break;

         case 'N':
             uint number = QString(gparam->value).toUInt(&ok);
             Q_ASSERT(ok);
             if(ok) {valueTag.n = number; }
             break;
         }
     }
     if(valueTag.n == 0)
         valueTag.n = linecounter;
     vTag->set(&valueTag);

     action = comproxy->sendM104Tag(vTag);
     return action;
}

//M106: Fan On
mito::Action_t*
GcodeWorker::tagM106_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM106_t *vTag =  reinterpret_cast<sM106_t *>( arraytag->getTagValue(eM106));
    sM106_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'S':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'P':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.p = dvalue;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }

    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    action = comproxy->sendM106_Tag(vTag);
    return action;
}

//M107: Fan Off
//Deprecated in Teacup firmware and in RepRapFirmware. Use M106 S0 instead.
//void
mito::Action_t*
GcodeWorker::tagM107_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    //Use M106 S0 instead.
    sM106_t *vTag =  reinterpret_cast<sM106_t *>( arraytag->getTagValue(eM106));
    sM106_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
//    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }

    valueTag.s = 0.0;
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    action = comproxy->sendM107_Tag(vTag);

    return action;
}

//M109: Set Extruder Temperature and Wait
mito::Action_t*
GcodeWorker::tagM109_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM109_t *vTag =  reinterpret_cast<sM109_t *>( arraytag->getTagValue(eM109));
    sM109_t valueTag ;
    vTag->get(&valueTag);
    valueTag.n = 0;
    bool ok = false;
    double dvalue;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'S':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.s = dvalue;
            break;

        case 'R':
            dvalue = value.toDouble(&ok);
            Q_ASSERT(ok);
            valueTag.r = dvalue;
            break;

        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {valueTag.n = number; }
            break;
        }
    }
    if(valueTag.n == 0)
        valueTag.n = linecounter;
    vTag->set(&valueTag);
    action = comproxy->sendM109_Tag(vTag);
    return action;
}

//M82: Set extruder to absolute mode
mito::Action_t*
GcodeWorker::tagM82_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM82_t *vTag =  reinterpret_cast<sM82_t *>( arraytag->getTagValue(eM82));
    vTag->a = true;
    vTag->n = 0;
    bool ok;

    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {vTag->n = number; }
            break;
        }
    }
    if(vTag->n == 0)
        vTag->n = linecounter;
    comproxy->sendM82_Tag(vTag);
    return action;
}

//M83: Set extruder to relative mode
mito::Action_t*
GcodeWorker::tagM83_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM82_t *vTag =  reinterpret_cast<sM82_t *>( arraytag->getTagValue(eM82));
    vTag->a = false;
    vTag->n = 0;
    bool ok;
    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {vTag->n = number; }
            break;
        }
    }
    if(vTag->n == 0)
        vTag->n = linecounter;

    comproxy->sendM82_Tag(vTag);
    return action;
}

mito::Action_t*
GcodeWorker::tagM84_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    sM84_t *vTag =  reinterpret_cast<sM84_t *>( arraytag->getTagValue(eM84));
    vTag->a = false;
    vTag->n = 0;
    bool ok;
    for(int i=0;i<sgCode->param_number;i++){
        sGparam* gparam = &sgCode->param[i];
        QString value(clearNumValue(gparam->value));
        switch (gparam->group){
        case 'N':
            uint number = QString(gparam->value).toUInt(&ok);
            Q_ASSERT(ok);
            if(ok) {vTag->n = number; }
            break;
        }
    }
    if(vTag->n == 0)
        vTag->n = linecounter;
    comproxy->sendM84_Tag(vTag);
    return action;
}



mito::Action_t*
GcodeWorker::tagF_Do(sGcode *sgCode)
{
    mito::Action_t * action = nullptr;
    uint number;
    bool ok;

    number = QString(sgCode->value).toUInt(&ok);
    Q_ASSERT(ok);

    //qDebug()<<__FILE__<<__LINE__<<"F:"<<number;
    return action;
}


QString
GcodeWorker::clearNumValue(QString value)
{
//    QString value(gparam->value);

//    qDebug()<<__FILE__<<__LINE__<<value;

    value.replace(',','.');

    int pos = value.indexOf(";");
    if(pos>=0){
        value.remove(pos,1);
//        qDebug()<<__FILE__<<__LINE__<<value;
    }
//    qDebug()<<__FILE__<<__LINE__<<value<<":"<<pos;

    QString::iterator it;
    pos = -1;
    int pnum = 0;
    for(it=value.begin();it!=value.end();++it){
        if(it->toLatin1() >0x39 || it->toLatin1()<0x2B){
            pos = pnum;
//            qDebug()<<__FILE__<<__LINE__<<it->toLatin1();
            break;
        }
        pnum++;
    }
    if(pos>=0)
        value.remove(pos,1);

    return value;
}

void GcodeWorker::syncXYZ(double x, double y, double z)
{
#if VERSION==1
    sG0_t * vTag0 =  reinterpret_cast<sG0_t *>( arraytag->getTagValue(eG0));
    vTag0->x = x;
    vTag0->y = y;
    vTag0->z = z;

    sG1_t * vTag1 =  reinterpret_cast<sG1_t *>( arraytag->getTagValue(eG1));
    vTag1->x = x;
    vTag1->y = y;
    vTag1->z = z;

    sG2_t * vTag2 =  reinterpret_cast<sG2_t *>( arraytag->getTagValue(eG2));
    vTag2->x = x;
    vTag2->y = y;
//    vTag2->z = z;

    sG3_t * vTag3 =  reinterpret_cast<sG3_t *>( arraytag->getTagValue(eG2));
    vTag3->x = x;
    vTag3->y = y;
//    vTag3->z = z;
#endif

}

void GcodeWorker::syncXY(double x, double y)
{
#if VERSION==1
    sG0_t * vTag0 =  reinterpret_cast<sG0_t *>( arraytag->getTagValue(eG0));
    vTag0->x = x;
    vTag0->y = y;
//    vTag0->z = z;

    sG1_t * vTag1 =  reinterpret_cast<sG1_t *>( arraytag->getTagValue(eG1));
    vTag1->x = x;
    vTag1->y = y;
//    vTag1->z = z;

    sG2_t * vTag2 =  reinterpret_cast<sG2_t *>( arraytag->getTagValue(eG2));
    vTag2->x = x;
    vTag2->y = y;
//    vTag2->z = z;

    sG3_t * vTag3 =  reinterpret_cast<sG3_t *>( arraytag->getTagValue(eG2));
    vTag3->x = x;
    vTag3->y = y;
//    vTag3->z = z;
#endif
}


void
GcodeWorker::fileOpen(QString filename)
{

//	char buffer[128];
    int result;
    linecounter = 0;

//	ComData* req_builder = new ComData();
	QFile file(filename);

	// Прочитать файл для проверки на наличие ошибок.
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    errorCounter = 0;

    QTextStream in(&file);
    while (!in.atEnd()) {

        memset(dst,0,sizeof(sGcode));

    	QString line = in.readLine();
        result = lexer->parcer(line);

//         qDebug()<<__FILE__<<__LINE__<<":"<<linecounter<<": line:"<<line; // Level1
         if(line.isEmpty())
             continue;

        if(result<=0){
            Q_ASSERT(result>=0);
            continue;
        }

        //checkBox_immediately
        buildAction(dst);

        linecounter++;
    }

    qDebug()<<__FILE__<<":"<<__LINE__ <<"\tlines:"<<linecounter<<"updated:"<<comproxy->getLine_counter();
    if(errorCounter>0)
        qWarning("Errors are in file :%d . Wrong Profile.",errorCounter);

    delete(lexer);

    file.close();
    qDebug() <<__FILE__<<__LINE__<< "File closed.";
//---------------------------- exit parcing

//     fclose(flog);

// /home/walery/Документы/3d-printer/ragel/exmple.gcode


/*
    QFileInfo floginfo(__log);
    qDebug() << "Scanning finished. Log file size:" << floginfo.size() ;
*/
}




//void
mito::Action_t*
GcodeWorker::readCommandLine()
{
    bool loop = true;
    int result;
    mito::Action_t* action = nullptr;

    QString line;// = _file->readLine();


    while(loop){
        // loop
        memset(dst,0,sizeof(sGcode));

        if(_file->atEnd()){
#if LEVEL==4
            timer->stop();
#endif
            emit sg_executeComplite();
            action = new mito::Action_t;
            action->a = eEOF;
             qDebug()<<__FILE__<<__LINE__<<"Stopped.";
            return (action);
        }

        line = _file->readLine(); linecounter++;

        if(line.isEmpty())
            continue;
        result = lexer->parcer(line);
#if LEVEL==1
        qDebug()<<__FILE__<<__LINE__<<line<<"\tline:"<<linecounter;
#endif

#if LEVEL == 1
        qDebug()<<__FILE__<<__LINE__<<"result:"<<result<<"\tgroup:"<<gcode.group;
#endif
        if(gcode.group==0)
            continue;

        char comment = ';';
        int k = strcmp( &gcode.group,&comment);
        if(k!=0 && result>0)
            loop = false;

    }
#if LEVEL==0
    qDebug()<<__FILE__<<__LINE__<<"result:"<<result<<"\tgroup:"<<gcode.group<<gcode.value;
#endif
    //checkBox_immediately
    action = buildAction(dst);

    return (action);
}


void GcodeWorker::setFileExecute(QFile &file)
{
//    bool loop = true;

//    this->in = &file;
//    QTextStream str(&file);
//    in = QTextStream(&file);
//    in = &str;

    _file = &file;
    linecounter = 0;

    QFileInfo info(file);
//    QString line = this->_file->readLine();
    qDebug()<<__FILE__<<__LINE__<<"File info:"<<info.size();
#if LEVEL==4
    timer->start(100);
#endif
// State is Programm mode.

}

void GcodeWorker::queueReady()
{
    readCommandLine();
}


