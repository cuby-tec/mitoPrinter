#ifndef COORDINATUS_H
#define COORDINATUS_H

#include "links/msmotor/msport.h"
#include "links/ComDataReq_t.h"

#include <QString>
#include <QObject>
#include "step_motor/block_state_t.h"
#include "step_motor/ProfileData.h"
#include <math.h>
#include <assert.h>     /* assert */

// Singleton
class Coordinatus//:public QObject
{
//    Q_OBJECT

public:
    static Coordinatus* instance()
    {
        static Coordinatus p;
        return &p;
    }

    // coping from current array to work array.
    void initWork();

    void setWorkValue(size_t axis, double_t value);

    double_t getWorkvalue(size_t axis);

    double_t getCurrentValue(size_t axis);

    double_t getNextValue(size_t axis);

    // true - absolute, false - relative
    bool isAbsolute() {return absrel;}

    void setAbsolute(bool value){absrel = value;}

    void moveWorkToNext(){
    	memcpy(next,work,sizeof(work)); //*sizeof(double_t));
    }

    void moveCurrentToWork(){
    	memcpy(work,current,sizeof(work));
    }


    void moveNextToCurrent(){
        memcpy(current,next,sizeof(current)); //*sizeof(double_t));
    }

    block_state_t currentBlocks[N_AXIS];

    block_state_t nextBlocks[N_AXIS];

    ProfileData_t* getProfileData(){ return &profileData; }

    /**
     * Загрузка данных из профиля.
     */
    void setupProfileData();


//signals:
    void sg_coordUpdated();

    double_t getFan_value() const;
    void setFan_value(const double_t &value);

    double getTemperature() const;
    void setTemperature(double value);

    bool getExtruder_mode() const;
    void setExtruder_mode(bool value);

    sHotendControl_t* getHotend() { return &hotend;  }

    int32_t position[N_AXIS];          // The planner position of the tool in absolute steps. Kept separate
                                       // from g-code position for movements requiring multiple line motions,
                                       // i.e. arcs, canned cycles, and backlash compensation.

    uint8_t getUnits() {
        return units;
    }

    void setUnits( uint8_t &value){
         units = value;
    }

    double_t getSpeedrate() const;
    void setSpeedrate(const double_t &value);


    void setMicrostep(uint32_t axis, uint32_t step){
    	assert(axis<N_AXIS);
    	assert(step<=4);
    	microstep[axis] = step;
    }

    uint32_t getMicrostep(uint32_t axis){
    	assert(axis<N_AXIS);
    	return (microstep[axis]);
    }

private:


    double_t current[N_AXIS];

    double_t next[N_AXIS];

    double_t work[N_AXIS];

    ProfileData_t profileData;

    bool absrel;

    //Units from now on are in inches.
    // 1 - millimeters, 2 - inches
    uint8_t units;	//TODO usage: now not used.

    //Microstep
    uint32_t microstep[N_AXIS];


// Hotend state
    bool extruder_mode;// true - absolute, false - relative
    double_t speedrate;
    sHotendControl_t hotend;

    void sendSignalCoord();

#define KD  0.12
#define KP  0.8//0.75
#define KI  1.0 - (KP + KD)


    Coordinatus()
    {
        double_t ki = KI;
        absrel = true; // true - absolute, false - relative
        extruder_mode = true; // true - absolute, false - relative
        units = 1;
        hotend._switch.cooler = 0;
        hotend._switch.heater = 0;
        hotend.kd = static_cast<int32_t>(KD*1000); //#define KD 0.12  12.5//0.1//0.013
        hotend.kp = static_cast<int32_t>(KP*1000); //#define KP  0.6//0.75
        hotend.ki = static_cast<int32_t>(ki*1000) ;//#define KI  0.1
        hotend.temperature = static_cast<int32_t>(40*10) ;//#define SETPOINT    40
        speedrate = 0.0;
        for (int i=0;i<N_AXIS;i++)
        	microstep[i] = 0;

    }

    ~Coordinatus() {}

    Coordinatus(Coordinatus const&) = delete;
    void operator = (Coordinatus const&) = delete;

};

#endif // COORDINATUS_H
