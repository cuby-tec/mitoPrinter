/*
 * eModelstate.h
 * Company: CUBY,Ltd
 * Project: freertos_demo
 *  Created on: 27 мая 2018 г.
 *      Author: walery
 */

#include <QString>

#ifndef EXCHANGE_EMODELSTATE_H_
#define EXCHANGE_EMODELSTATE_H_

enum eModelstate{
    ehIdle = 1, ehIwork
    ,ehEnderXmax, ehEnderXmin, ehEnderYmax, ehEnderYmin, ehEnderZmax, ehEnderZmin
    ,ehException, ehWait_instrument1, ehWait_instrument2
};

class ModelState{
public:
    static QString state(uint number){
        switch (number) {
        case 1:
            return(QString("ehIdle"));
            break;
        case 2:
            return (QString("ehIwork"));
            break;
        case 3:
            return QString("ehEnderXmax");
            break;
        case 4:
            return QString("ehEnderXmin");
            break;
        case 5:
            return QString("ehEnderYmax");
            break;
        case 6:
            return QString("ehEnderYmin");
            break;
        case 7:
            return QString("ehEnderZmax");
            break;
        case 8:
            return QString("ehEnderZmin");
            break;
        case 9:
            return QString("ehException");
            break;
        case 10:
            return QString("ehWait_instrument1");
            break;
        case 11:
            return QString("ehWait_instrument2");
            break;
        default:
            return QString("unknown");

        }
    }
};

#endif /* EXCHANGE_EMODELSTATE_H_ */
