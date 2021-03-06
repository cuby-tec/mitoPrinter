/*
 * ComDataReq_t.h
 *
 *  Created on: 10 сент. 2017 г.
 *      Author: walery
 */

#ifndef EXCHANGE_COMDATAREQ_T_H_
#define EXCHANGE_COMDATAREQ_T_H_

#include "msmotor/sSegment.h"
#include "sHotendControl.h"

#include "sProfile.h"

//------------- defs

enum eOrder{
    eoState,            // Запрос состояния устройства.
    eoProfile,      // Задание параметров Profile include Hotend
    eoSegment           // Семент движения.
    , eoHotendControl      // управление Экструдером.
    , bedTemperaure     // Температура стола.
    , eoG92			// G92: Set Position
	, eoM84			//  M84: Stop idle hold
};

/**
 * if 1 - execute immediately, 0 - store only
 */
#define EXECUTE_IMMEDIATELY		(1<<0)


struct sControlCommand_t {
    uint16_t order;      // Команда инструменту.
    uint8_t reserved; //  EXECUTE_IMMIDIATELY | COMMAND_ACKNOWLEDGED
    uint8_t instrument; // Тип инструмента в запросе.
};

struct ComDataReq_t{
    uint32_t    requestNumber;  // Номер запроса порядковый.
    uint8_t     size;           // Разме сообщения.
    uint8_t     instruments;    // Количество инструментов в запросе.
    struct sControlCommand_t    command;        // Команда управления контроллером
    //-------- payload
    union {
    struct sSegment instrument1_parameter;  // Сегмент движения.
    struct sHotendControl_t instrument_hotend;           // Управление инструментом Hotend.
    uint32_t    instrument2_paramter;       // или параметры инструмента 2
    uint32_t    instrument3_paramter;       // или параметры инструмента 3
    uint32_t    instrument4_paramter;       // или параметры инструмента 4
    struct  sProfile profile;
    struct sBedControl_t bedcontrol;
    }payload;
};


//-------------- vars


//--------------- function



#endif /* EXCHANGE_COMDATAREQ_T_H_ */
