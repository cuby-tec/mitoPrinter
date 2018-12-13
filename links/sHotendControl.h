/*
 * sHotendControl.h
 *
 *  Created on: 22 апр. 2018 г.
 *      Author: walery
 */

#ifndef EXCHANGE_SHOTENDCONTROL_H_
#define EXCHANGE_SHOTENDCONTROL_H_

#include <stdint.h>

//------------- defs
#define use_float


/**
 * Включение/выключение нагревателя/вентилятора. false/true.
 */
struct sHotendSwitch {
    uint16_t cooler;
    uint16_t heater;
//    void operator = (sHotendSwitch& src)
    void operator = (sHotendSwitch* src)
    {
        cooler = src->cooler;
        heater = src->heater;
    }
};

struct sHotendHead{
    uint32_t    linenumber;
};

/**
 * Передача параметров управления инструментом Hotend.
 */
#ifdef use_float_no
struct sHotendControl_t {
    float temperature;
    float kp;     // Коэффициент пропорциональной составляющей.
    float ki;     // Коэффициент интегральной составляющей.
    float kd;     // Коэффициент дифференциальной составляющей.
    struct sHotendSwitch _switch;    // Включение/выключение нагревателя/вентилятора. false/true.
    void operator = (sHotendControl_t* src)
    {
        temperature = src->temperature;
        kp = src->kp;
        ki = src->ki;
        kd = src->kd;
        _switch = src->_switch;
    }
};
#else
struct sHotendControl_t {
    sHotendHead head;
    int32_t temperature;
    int32_t kp;     // Коэффициент пропорциональной составляющей.
    int32_t ki;     // Коэффициент интегральной составляющей.
    int32_t kd;     // Коэффициент дифференциальной составляющей.
    struct sHotendSwitch _switch;    // Включение/выключение нагревателя/вентилятора. false/true.
    sHotendControl_t& operator = (const sHotendControl_t* src)
    {
        temperature = src->temperature;//Внутреннее значение в 10 раз больше действительного, т.к. integer.
        kp = src->kp;
        ki = src->ki;
        kd = src->kd;
        _switch = src->_switch;
        return *this;
    }

};

struct sBedControl_t{
    int32_t temperature;
    int32_t kp;     // Коэффициент пропорциональной составляющей.
    int32_t ki;     // Коэффициент интегральной составляющей.
    int32_t kd;     // Коэффициент дифференциальной составляющей.
};

#endif

//-------------- vars


//--------------- function



#endif /* EXCHANGE_SHOTENDCONTROL_H_ */
