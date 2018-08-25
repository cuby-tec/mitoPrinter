#ifndef MYGLOBAL_H
#define MYGLOBAL_H

#include <QThread>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <QDir>

#include <math.h>

class MyGlobal
{
public:
    MyGlobal();

    static int mglobal;

    static uint32_t requestIndex;

    static QMutex mutex;

    static bool useTermoLog;    // Вести или не вести лог температурного датчика Hotend.

    static QString logfileDir;

    static QString logfileDelimiter;

    static QString comment;


    static QString profilefileDir;


    static QString message4;

    static QString message5;

    static QString msg_absolute;
    static QString msg_relative;

    static QString msg_abs_title;
    static QString msg_rel_title;

    static const double_t PI;// = 3.1415926536 ;	//3.1415;

    //-------- exchange

    static u_int32_t commandIndex;

    static void mPrint(QString line);

    static QString formatFloat(QString fl);

    static double_t DEGREES(double_t angle);

};

#endif // MYGLOBAL_H
