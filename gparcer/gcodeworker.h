/**
  File : gcodeworker.h
  Company: CUBY,Ltd
  date: 25.05.2018
 */


#ifndef GCODEWORKER_H
#define GCODEWORKER_H

//-------
//#include "gparcer/araytag.h"
#include "lexer.h"
#include "comdataproxy.h"

//#include <cstdio>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTimer>
#include <QObject>

#define VERSION 1

class GcodeWorker: public QObject
{

    Q_OBJECT

public:
    explicit GcodeWorker(QObject *parent = nullptr);

//    ~GcodeWorker(){
//#if VERSION==1
//        delete comproxy;
//#endif
//    }

    void fileOpen(QString filename);

    void setFileExecute(QFile &file);

    bool isFileOpened(){
        return _file->isOpen();
    }

    void readCommandLine();

public slots:
    void queueReady();

signals:
    void sg_executeComplite();

private:

//    std::FILE* fp;
//    std::FILE* flog;
    QFile fp;
    QFile flog;
#if VERSION==1
    ArrayTag *arraytag; // Data storege
#endif

    void buildAction(sGcode * src);

    void tagG0_Do(sGcode * sgCode);
    void tagG1_Do(sGcode * sgCode);
    void tagG2_Do(sGcode * sgCode);
    void tagG3_Do(sGcode * sgCode);
    void tagG4_Do(sGcode * sgCode);
    void tagG6_Do(sGcode * sgCode);
    void tagG10_Do(sGcode * sgCode);
    void tagG20_Do(sGcode * sgCode);
    void tagG21_Do(sGcode * sgCode);
    void tagG28_Do(sGcode * sgCode);
    void tagG29_1_Do(sGcode * sgCode);
    void tagG29_2_Do(sGcode * sgCode);
    void tagG30_Do(sGcode * sgCode);
    //Delta Auto Calibration
    void tagG33_Do(sGcode * sgCode);
    void tagG90_Do(sGcode * sgCode); //Set to Absolute Positioning
    void tagG91_Do(sGcode * sgCode); //Set to Relative Positioning
    void tagG92_Do(sGcode * sgCode); //Set Position
    void tagG92_1_Do(sGcode * sgCode); //reset axis offsets to zero and set parameters 5211 - 5219 to zero
    void tagM104_Do(sGcode * sgCode); //M104: Set Extruder Temperature
    void tagM106_Do(sGcode * sgCode); //M106: Fan On
    void tagM107_Do(sGcode * sgCode); //M107: Fan Off
    void tagM109_Do(sGcode * sgCode); //M109: Set Extruder Temperature and Wait
    void tagM82_Do(sGcode * sgCode); //M82: Set extruder to absolute mode
    void tagM83_Do(sGcode * sgCode); //M83: Set extruder to relative mode
    void tagM84_Do(sGcode * sgCode); //M84: Stop idle hold

    void tagF_Do(sGcode * sgCode);


    typedef void (GcodeWorker::*tag)(sGcode* );

    tag callTagRef[31];

    uint errorCounter;
#if VERSION==1
    ComdataProxy* comproxy;
#endif

//    QTextStream *in;

    QFile *_file;

    uint linecounter;


    sGcode gcode;
    sGcode* dst = &gcode;
    Lexer* lexer;
#if LEVEL==4
    QTimer *timer;
#endif

    QString clearNumValue(QString value);

    void syncXYZ(double x, double y, double z);
    void syncXY(double x, double y);

};
#undef EXAMPLE
#endif // GCODEWORKER_H
