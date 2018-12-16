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
#include "step_motor/settings.h"
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

    mito::Action_t *readCommandLine();

    mito::Action_t *buildAction(sGcode * src);

    uint getLinecounter() const;

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


    mito::Action_t* tagG0_Do(sGcode * sgCode);
    mito::Action_t* tagG1_Do(sGcode * sgCode);
    mito::Action_t* tagG2_Do(sGcode * sgCode);
    mito::Action_t* tagG3_Do(sGcode * sgCode);
    mito::Action_t* tagG4_Do(sGcode * sgCode);
    mito::Action_t* tagG6_Do(sGcode * sgCode);
    mito::Action_t* tagG10_Do(sGcode * sgCode);
    mito::Action_t* tagG20_Do(sGcode * sgCode);
    mito::Action_t* tagG21_Do(sGcode * sgCode);
    mito::Action_t* tagG28_Do(sGcode * sgCode);
    mito::Action_t* tagG29_1_Do(sGcode * sgCode);
    mito::Action_t* tagG29_2_Do(sGcode * sgCode);
    mito::Action_t* tagG30_Do(sGcode * sgCode);
    //Delta Auto Calibration
    mito::Action_t* tagG33_Do(sGcode * sgCode);
    mito::Action_t* tagG90_Do(sGcode * sgCode); //Set to Absolute Positioning
    mito::Action_t* tagG91_Do(sGcode * sgCode); //Set to Relative Positioning
    mito::Action_t* tagG92_Do(sGcode * sgCode); //Set Position
    mito::Action_t* tagG92_1_Do(sGcode * sgCode); //reset axis offsets to zero and set parameters 5211 - 5219 to zero
    mito::Action_t* tagM104_Do(sGcode * sgCode); //M104: Set Extruder Temperature
    mito::Action_t* tagM106_Do(sGcode * sgCode); //M106: Fan On
    mito::Action_t *tagM107_Do(sGcode * sgCode); //M107: Fan Off
    mito::Action_t* tagM109_Do(sGcode * sgCode); //M109: Set Extruder Temperature and Wait
    mito::Action_t* tagM140_Do(sGcode* sgCode);//M140: Set Bed Temperature (Fast)
    mito::Action_t* tagM190_Do(sGcode* sgCode);//M190: Wait for bed temperature to reach target temp
    mito::Action_t* tagM82_Do(sGcode * sgCode); //M82: Set extruder to absolute mode
    mito::Action_t* tagM83_Do(sGcode * sgCode); //M83: Set extruder to relative mode
    mito::Action_t* tagM84_Do(sGcode * sgCode); //M84: Stop idle hold

    mito::Action_t* tagF_Do(sGcode * sgCode);


    typedef mito::Action_t* (GcodeWorker::*tag)(sGcode* ); // void

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

    void syncXYZ(double_t x, double_t y, double_t z, double_t e);
    void syncXY(double x, double y);
    /**
     * @brief loadMovervalue
     * @param data
     * Load data from Coordinatus.
     */
    void loadMovervalue(sMover* data);

};
#undef EXAMPLE
#endif // GCODEWORKER_H
