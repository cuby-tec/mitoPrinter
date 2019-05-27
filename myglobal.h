#ifndef MYGLOBAL_H
#define MYGLOBAL_H

#include <sgcode.h>

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

    static int parceString(char* src,sGcode* dst );

    static uint_least32_t crc32(unsigned char* buff, size_t len);


    /*
      Name  : CRC-8
      Poly  : 0x31    x^8 + x^5 + x^4 + 1
      Init  : 0xFF
      Revert: false
      XorOut: 0x00
      Check : 0xF7 ("123456789")
      MaxLen: 15 байт(127 бит) - обнаружение
        одинарных, двойных, тройных и всех нечетных ошибок
   https://ru.wikibooks.org/wiki/%D0%A0%D0%B5%D0%B0%D0%BB%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8
    _%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D0%BE%D0%B2/
    %D0%A6%D0%B8%D0%BA%D0%BB%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9
    _%D0%B8%D0%B7%D0%B1%D1%8B%D1%82%D0%BE%D1%87%D0%BD%D1%8B%D0%B9
    _%D0%BA%D0%BE%D0%B4
    */
    static uint8_t Crc8(uint8_t *pcBlock, uint32_t len);

};

#endif // MYGLOBAL_H
