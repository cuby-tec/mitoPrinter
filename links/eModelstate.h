/*
 * eModelstate.h
 * Company: CUBY,Ltd
 * Project: freertos_demo
 *  Created on: 27 мая 2018 г.
 *      Author: walery
 */

#ifndef EXCHANGE_EMODELSTATE_H_
#define EXCHANGE_EMODELSTATE_H_

enum eModelstate{
    ehIdle = 1, ehIwork
    ,ehEnderXmax, ehEnderXmin, ehEnderYmax, ehEnderYmin, ehEnderZmax, ehEnderZmin
    ,ehException, ehWait_instrument1, ehWait_instrument2
};


#endif /* EXCHANGE_EMODELSTATE_H_ */
