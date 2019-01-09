#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stddef.h>

#include "countertime_t.h"
#include "stepmotor.h"
#include "links/msmotor/msport.h"
#include "block_state_t.h"
#include "gparcer/coordinatus.h"
#include "ProfileData.h"
#include "trapeze_t.h"
#include "accelerationtable.h"

#define FREQUENCY  50000000 //;frequency = 50000000;
// 0xFFFFFE
#define MAX_COUNTER_VALUE	16777214

class Controller
{
public:
    Controller();
/*
    Controller(StepMotor* motor){
    	this->motor = motor;
    	frequency = FREQUENCY;
    }
*/
    ~Controller();

    /**
      Построение таблицы значений счётчика для заданного количества шагов.
     * @brief buildCounterValue
     * @param steps
     */
    void buildCounterValue(uint32_t steps, uint8_t axis);

    /**
     * Заполнение полей разгона, торможения, и т.д.
     */
    bool buildBlock(Coordinatus* cord);

    double_t getPath_mm(uint8_t axis,int32_t steps);


    /**
     * Вчисляется минимальное значение шага.
     * !! Выполняется установка микрошага для данного двигателя.
     */
    double_t getPrecicion(uint8_t axis, uint8_t microstep);

    /**
     * Величина шага для двигателя оси.
     */
    double_t getPrecicion(uint8_t axis);

    //  Длительность работы счётчика.
    double_t getTimeOfCounter(uint32_t counter){
    	return ((double_t)counter/(double_t)frequency);
    }

    /**
     * @brief uploadPosition
     * Загрузка вектора position
     * from nextValue. Used by G92
     */
    void uploadPosition(Coordinatus *cord);

    void buildCircleStep(Coordinatus* cord );

    StepMotor** getMotors(){
        return motor;
    }

    double_t get_MMperStep(uint axis, Coordinatus *cord);

private:
    size_t frequency;

    CounterTime_t* pcountertime[N_AXIS];

    StepMotor* motor[N_AXIS];

    ProfileData_t* profileData;

    void setupProfileData();



    Trapeze_t trapeze[N_AXIS];

    // Расчёт трапеций для каждой оси.
    uint32_t calculateTrapeze();

    AccelerationTable* acctable;

    void splinePath(block_state_t* privBlock, block_state_t* nextBlock);

    // пересчёт ускорений для одной оси.
    void planner_recalculate(block_state* prev, block_state* curr);

    void calculateExtruder(block_state_t* blocks);

    /**
     * Загрузка данных(Угловое ускорение) из профиля.
     */
    void uploadMotorData();

    uint32_t maxvector[N_AXIS];
    /**
     * Выбор допустимого значсения feedrate.
     */
    double_t selectFeedrate(double_t cord_feedrate );
};

#endif // CONTROLLER_H
