#include "myglobal.h"

#include "qcustomplot.h"
//#include <gcode-lexer.h>
#include <gparcer/lexer.h>

///----------------- vars


int MyGlobal::mglobal = 0;

uint32_t MyGlobal::requestIndex = 0;

QMutex MyGlobal::mutex;

bool MyGlobal::useTermoLog = true;

//const QString G_logfileDir(QDir::homePath()+"/tmp");

QString MyGlobal::logfileDir = QString(QDir::homePath()+"/tmp");

//QString MyGlobal::profilefileDir = QString(QDir::currentPath());
QString MyGlobal::profilefileDir = QString(QDir::homePath()+"/mito");

QString MyGlobal::message4("File dos't selected.");

QString MyGlobal::message5("File corrupted.");

QString MyGlobal::msg_absolute("Abs");
QString MyGlobal::msg_relative("Rel");

QString MyGlobal::msg_abs_title("Absolute positioning.");
QString MyGlobal::msg_rel_title("Relative positioning.");


QString MyGlobal::logfileDelimiter(";");

QString MyGlobal::comment("#");

uint32_t MyGlobal::commandIndex = 1;

double_t const MyGlobal::PI = 3.1415926536 ;	//3.1415;


MyGlobal::MyGlobal()
{
//    MyGlobal::mglobal = 0;
//    sglobal.logfileDir = G_logfileDir;
}
void
MyGlobal::mPrint(QString line)
{
    QTextStream print(stdout, QIODevice::WriteOnly); // stdin
    print << line;
}

/*
 *
 *     int i;

    QString str_val = QString( param->value );

    i = str_val.indexOf(',');

    if(i>0)
    {
        str_val = str_val.replace(i,1,'.');
    }
 *
*/
QString
MyGlobal::formatFloat(QString fl)
{
    int i;

    i = fl.indexOf(',');
    if(i>0)
    {
        fl = fl.replace(i,1,'.');
    }
    return fl;
}

double_t
MyGlobal::DEGREES(double_t angle)
{
	return (angle*180/PI);
}

int
MyGlobal::parceString(char* src,sGcode* dst )
{
    int result = 0;
    size_t len = strlen(src);
    Lexer* le = new Lexer(dst);
    result = le->parcer(src, static_cast<int>(len));
    return result;
}

/*
  Name  : CRC-32
  Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение
   одинарных, двойных, пакетных и всех нечетных ошибок
   https://ru.wikibooks.org/wiki/%D0%A0%D0%B5%D0%B0%D0%BB%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8
    _%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D0%BE%D0%B2/
    %D0%A6%D0%B8%D0%BA%D0%BB%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9
    _%D0%B8%D0%B7%D0%B1%D1%8B%D1%82%D0%BE%D1%87%D0%BD%D1%8B%D0%B9
    _%D0%BA%D0%BE%D0%B4
*/
uint_least32_t
MyGlobal::crc32(unsigned char* buf, size_t len )
{
//#include <stddef.h>
//#include <stdint.h>
    uint_least32_t crc_table[256];
    uint_least32_t crc; int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

uint8_t
MyGlobal::Crc8(uint8_t *pcBlock, uint32_t len)
{
    uint8_t crc = 0xFF;
    unsigned int i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}

