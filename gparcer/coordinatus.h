#ifndef COORDINATUS_H
#define COORDINATUS_H

#include "links/msmotor/msport.h"
#include "links/ComDataReq_t.h"

#include <QString>
#include <QObject>
#include "step_motor/block_state_t.h"
#include "step_motor/ProfileData.h"
#include <math.h>

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

    bool isAbsolute() {return absrel;}

    void setAbsolute(bool value){absrel = value;}

    void moveWorkToNext(){
    	memcpy(next,work,sizeof(work)); //*sizeof(double_t));
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

private:

    double_t current[N_AXIS];

    double_t next[N_AXIS];

   double_t work[N_AXIS];

    ProfileData_t profileData;

    bool absrel;

// Hotend state
    bool extruder_mode;// true - absolute, false - relative

    sHotendControl_t hotend;

    void sendSignalCoord();

    Coordinatus()
    {
        absrel = true; // true - absolute, false - relative
        extruder_mode = true; // true - absolute, false - relative
        hotend._switch.cooler = 0;
        hotend._switch.heater = 0;
        hotend.kd = static_cast<int32_t>(12.5*1000); //#define KD  12.5//0.1//0.013
        hotend.kp = static_cast<int32_t>(0.6*1000); //#define KP  0.6//0.75
        hotend.ki = static_cast<int32_t>(0.1*1000) ;//#define KI  0.1
        hotend.temperature = static_cast<int32_t>(40*10) ;//#define SETPOINT    40
    }

    ~Coordinatus() {}

    Coordinatus(Coordinatus const&) = delete;
    void operator = (Coordinatus const&) = delete;

};

#endif // COORDINATUS_H
