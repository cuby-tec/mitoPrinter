/*
 *
 * Характеристики двигателя.
 *
 */

#ifndef STEPMOTOR_H
#define STEPMOTOR_H

//#include <QString>
#include <stdint.h>
#include <stddef.h>

#include <math.h>
#include "links/msmotor/msport.h"
#include "myglobal.h"
#include <assert.h>     /* assert */

// "Крутящий момент(Н. см)" 40
//"Фиксированный крутящий момент(Н. см)" 2.2
//"Инерция ротора (Г. см2)" 54
//"Фаза индуктивность (Mh)" 2.8
//"Сопротивление фаза (Ом)" 1.5
//"Номинальный ток (A)"     1.7
//"Двигатель Длина (Мм)"    40
//"Шаг угол (Град)"         1.8
//"Двигатель Вес (G)"       280






#define _17HS4401

#define MENDEL	1

#if MENDEL == 1

#define PULLEY_DIAMETER	12.1 // pulley_diameter

#define SHAFT_PITCH	1.25 // Шаг винта[mm]

// диаметр вала экструдера с учётом проточки
#define shaft_diameter   6.6//7

#endif

#ifdef _17HS4401
#define ANGLE	1.8
#endif



class StepMotor;

// Скорость для оси.
typedef double_t (StepMotor::*convert)(double_t param);

// Длина шага для оси.
typedef double_t (StepMotor::*lines)(uint32_t axis);


enum eMotorType{
    e17HS4401_pulley, e17HS4401_shuft, e17HS4401_tooth_10_43
};


class StepMotor
{
public:
    StepMotor(eMotorType type);

    size_t getStepsPerRound() {return stepsPerRound;}

    void setStepsPerRound(size_t steps) {this->stepsPerRound = steps;}

	double_t getPulleyDiameter() {
		return pulley_diameter;
	}

	void setPulleyDiameter(double_t pulleyDiameter) {
		pulley_diameter = pulleyDiameter;
	}

	double_t getAngle() {
		return angle;
	}

	/**
	 * Для каждой оси может быть назначен свой микрошаг.
	 */
	double_t getAlfa(uint32_t axis){
		return (alfa/microstep[axis]);
	}

	/**
	 * linear velocity to angular_velocity
	 * in: mm/second
	 * out: radian/second
	 */
	double_t angular_velocity_tan(double_t tangential_speed);

	/**
	 * Round per min to angular velocity
	 * in: Round per min
	 * out: rad/sec
	 */
	double_t angular_velocity_rpm(double_t rpm);

	/**
	 * in: Round per min
	 * in: raccel rad/sec^2
	 * out: steps to achieve rpm.
	 */
	double_t steps_rpm(double_t rpm, double_t raccel);



    uint32_t getMicrostep(uint32_t axis) {
		return microstep[axis];
	}



    void setMicrostep(uint32_t axis, uint32_t _microstep){
    	assert(_microstep==1 || _microstep==2 || _microstep==4||_microstep==8||_microstep==16);
        microstep[axis] = _microstep;
	}

    // Угловое ускорение, рад/сек^2
	double_t getAcceleration() const {
        return acceleration;
	}

    double_t getDecceleration() const {
        return acceleration; // TODO
    }

	// Угловое ускорение, рад/сек^2
	void setAcceleration(double_t acceleration) {
		this->acceleration = acceleration;
	}

	// Линейное ускорение для заданного
	// максимального углового ускорения.
	double_t getLinearAcceleration();

    convert getLineSpeed;

    lines getLineStep;

//protected:

    //Длина шага для шкива
    double_t lineStep(uint32_t axis);

    // длина шага для винта
    double_t pulleyStep(uint32_t axis);

    //длина шага зубчатой передачи
    double_t gear10_43_Step(uint32_t axis);


	/**
	 * Преобразование вращательной скорости(об/мин) в линейную.
	 */
	double_t linespeed(double_t rpm);

	/**
	 * Для винта.
	 */
	double_t linespeed_pitch(double_t rpm);

    //для зубчатой передачи
    double_t linespeed_gear_10_43(double_t rpm);
    // radian per second
    double_t getAngular_velocity_rad_value() ;

private:

    size_t stepsPerRound;
    
    double_t pulley_diameter; // 12.1
    
    double_t angle; // 1.8град


    uint32_t microstep[N_AXIS];

    double_t alfa;

    // Угловое ускорение, рад/сек^2
    double_t acceleration;

// axis E small:10 tooths; large: 43 tooths. d 8-2*0.5=7mm
    // 3.14159*7 = 21,9911

    //Максимальная скорость вращения об/мин
    double_t angular_velocity_rpm_value;


};

#endif // STEPMOTOR_H

/*
 *
 *
*/
