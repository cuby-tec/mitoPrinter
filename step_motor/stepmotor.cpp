

#include "stepmotor.h"

#include "math.h"

//--------- defs


StepMotor::StepMotor(eMotorType type)
    : angular_velocity_rpm_value(500.0)
{
#ifdef _17HS4401

    angle = ANGLE;	//1.8;

    stepsPerRound = 360/ANGLE;

   this->acceleration = 1250;	//NULL;

#endif

    for(int i=0;i<N_AXIS;i++)
    {
    	microstep[i] = 1;
    }

    alfa = 2*MyGlobal::PI/stepsPerRound;

   pulley_diameter = PULLEY_DIAMETER;
   

   switch(type){
   case e17HS4401_pulley:
	   getLineSpeed = &StepMotor::linespeed;
	   getLineStep = &StepMotor::lineStep;
	   getAngularSpeedrate = &StepMotor::angularSpeedrate_pulley;
	   break;

   case e17HS4401_shuft:
	   getLineSpeed = &StepMotor::linespeed_pitch;
	   getLineStep = &StepMotor::pulleyStep;
	   getAngularSpeedrate = &StepMotor::angularSpeedrate_pitch;
	   break;
   case e17HS4401_tooth_10_43:
       getLineSpeed = &StepMotor::linespeed_gear_10_43;
       getLineStep = &StepMotor::gear10_43_Step;//0.0256
       getAngularSpeedrate = &StepMotor::angularSpeedrate_gear_10_43;
       break;
   }


/*
#if MENDEL == 1
   getLineSpeed[X_AXIS] = &StepMotor::linespeed;
   getLineSpeed[Y_AXIS] = &StepMotor::linespeed;
   getLineSpeed[Z_AXIS] = &StepMotor::linespeed_pitch;

   getLineStep[X_AXIS] = &StepMotor::lineStep;
   getLineStep[Y_AXIS] = &StepMotor::lineStep;
   getLineStep[Z_AXIS] = &StepMotor::pulleyStep;

#endif
*/
}

/*
double_t
StepMotor::getPulleyDiameter() const {
	return pulley_diameter;
}
*/

/*
void
StepMotor::setPulleyDiameter(double_t pulleyDiameter) {
	pulley_diameter = pulleyDiameter;
}
*/

double_t
StepMotor::angular_velocity_tan(double_t tangential_speed)
{
	return ((tangential_speed/pulley_diameter)*2);
}

double_t StepMotor::angular_velocity_rpm(double_t rpm) {
	return (rpm*MyGlobal::PI/30);
}

//double_t
//StepMotor::getAlfa(uint32_t axis) {
//	return (alfa/microstep[axis]);
//}

double_t StepMotor::steps_rpm(double_t rpm, double_t raccel) {
	double_t aps = angular_velocity_rpm(rpm); // angular per second
	return (pow(aps,2)/(2*alfa*raccel));
}

// для шкива
double_t StepMotor::linespeed(double_t rpm) {

	//Число оборотов (об/мин)		500
	double_t radianps = MyGlobal::PI/30*rpm;
	radianps *= pulley_diameter/2; // radius
	return radianps;
}

// для винта
double_t StepMotor::linespeed_pitch(double_t rpm) {
	double_t v;
	v = rpm/60;
	v*= SHAFT_PITCH;
    return v;
}

double_t StepMotor::linespeed_gear_10_43(double_t rpm)
{
    //Число оборотов (об/мин)		500
    double_t radianps = MyGlobal::PI/30*rpm;
    radianps *= shaft_diameter/2.0*(10.0/43.0); // radius
    return radianps;
}

double_t StepMotor::getAngular_velocity_rad_value()
{
    return angular_velocity_rpm( angular_velocity_rpm_value);
}

double_t StepMotor::lineStep(uint32_t axis) {
    double_t alfa = getAlfa(axis);
    alfa *= pulley_diameter/2;
	return alfa;
}

double_t StepMotor::pulleyStep(uint32_t axis) {
	double_t result;
	result = SHAFT_PITCH/(microstep[axis] * stepsPerRound);
    return result;
}

double_t StepMotor::gear10_43_Step(uint32_t axis)
{
    double_t result;
    //TODO
    double_t alpha = getAlfa(axis);
    result = alpha*shaft_diameter/2.0*(GEAR_LITTLE/GEAR_LARGE);
    return result;

}

// Линейное ускорение для заданного
// максимального углового ускорения.
double_t StepMotor::getLinearAcceleration() {
    double_t result;
    result = acceleration*pulley_diameter/2; // 1250
    return result;
}

double_t StepMotor::angularSpeedrate_pulley(double_t speedrate) {
	//TODO
	double_t result = speedrate/60;
	result /= pulley_diameter/2;
	return result;
}

double_t StepMotor::angularSpeedrate_pitch(double_t speedrate) {
	//TODO
	double_t result = speedrate/60;
	result *= 2*MyGlobal::PI;
	result /= SHAFT_PITCH;
	return result;
}

double_t StepMotor::angularSpeedrate_gear_10_43(double_t speedrate) {
	//TODO
	double_t result = speedrate/60;
	double_t extruder_gear = GEAR_LITTLE/GEAR_LARGE;
	extruder_gear *= shaft_diameter/2;
	result /= extruder_gear;
	return result;
}


// EOF


