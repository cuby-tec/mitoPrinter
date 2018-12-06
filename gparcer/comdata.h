#ifndef COMDATA_H
#define COMDATA_H



#include "mitoaction.h"
#include <sgcode.h>

#include "links/ComDataReq_t.h"
//#include "gparcer/sgcode.h"
#include "geometry/Arc.h"

#include "profiles/profile.h"
#include "coordinatus.h"
//#include "step_motor/stepmotor.h"
#include "step_motor/controller.h"
#include "step_motor/block_state_t.h"

#include "exchange/threadexchange.h"
#include "exchange/threadarc.h"

#include "gparcer/gcodeworker.h"
#include "messager.h"
#include "command/waittemperature.h"
#include "command/waitsendaction.h"

#include <QObject>
#include <QChar>
#include <QTimer>
#include <QProgressDialog>

#define DIRECTION_BIT    	1  // Port X Pin X
#define STEP_BIT         	2  // Port X pin X


#define X_DIRECTION_BIT    	0  // Port X Pin X
#define X_STEP_BIT         	1  // Port X pin X

#define Y_DIRECTION_BIT    	2  // Port Y Pin Y
#define Y_STEP_BIT         	3  // Port Y pin Y

#define Z_DIRECTION_BIT    	4  // Port Z Pin Z
#define Z_STEP_BIT         	5  // Port Z pin Z

#define E_DIRECTION_BIT    	6  // Port Z Pin Z
#define E_STEP_BIT         	7  // Port Z pin Z


#define X_DIRECTION_MASK 		(1<<Y_DIRECTION_BIT) // All direction bits
#define X_STEP_MASK 			(1<<Y_STEP_BIT) // All step bits

#define Y_DIRECTION_MASK 		(1<<Y_DIRECTION_BIT) // All direction bits
#define Y_STEP_MASK 			(1<<Y_STEP_BIT) // All step bits

#define Z_DIRECTION_MASK 		(1<<Y_DIRECTION_BIT) // All direction bits
#define Z_STEP_MASK 			(1<<Y_STEP_BIT) // All step bits

#define E_DIRECTION_MASK 		(1<<Y_DIRECTION_BIT) // All direction bits
#define E_STEP_MASK 			(1<<Y_STEP_BIT) // All step bits

enum eCDstate{
	ecdOne = 1, ecdCircle
};


enum runStates{
    ersError,ersRunning=1
    ,ersWaitParamTemperature
    , ersEOF // Конец файла.
    , ersStop   //emnergancy stop
};

enum goCondition{
    egcFile // Обработка файла
    , egcLine // обработка строки.
};


class ComData : public QObject
{

    Q_OBJECT

public:
    explicit ComData(QObject *parent = nullptr);

    ComDataReq_t* getRequest()
    {
        return &request;
    }

    void setRequestNumber(uint32_t num){
        request.requestNumber = num;
        if(request.requestNumber == 0)
            request.requestNumber++;
    }


//    ComDataReq_t* build(sGcode* sgcode);

    double_t getPath_mm(uint8_t axis, int32_t steps) { return (controller->getPath_mm(axis, steps)); }

    Controller* getController(){ return controller; }


    void buildComData(sGcode* sgcode, bool checkBox_immediately);


    void run(GcodeWorker *gworker);

    void stop();

signals:
    void sg_updateStatus(const Status_t* status);
    void sg_executeComplite();


public slots:
    void slot_fileComplite();   // file reach EOF.
//    void heatingCancel();

private slots:
    void updateStatus(const Status_t* status);
    void failedStatus();
    void sl_caceled();
    void waitParam();// Start circle of reading file.
    void waitTemp(mito::Action_t* action);
//    void testTimer();
    void temperatureDone();
    void waitsendDone();


private:

    goCondition condition;

    bool acknowledge_flag;
    int a;

    union {
        double_t d;
        float f;
        int32_t i;
    }param; // wait param;

    union {
        double_t d;
        float f;
        int32_t i;
    }statusParam; // wait param;



//    ThreadExchange thread;

    ThreadArc threadarc;


    ComDataReq_t request;

    Controller* controller;

    GcodeWorker *gworker;

    sGcode* sgCode;

    Profile* profile;

    Coordinatus* cord;

    Arc* arc;

    Messager* messager;

    QTimer waitTimer;

    QTimer testT;

    WaitTemperature* waitTemperature;

    int steps;

    QProgressDialog* pd;

    WaitSendAction* waitsendAction;

    mito::Action_t* _action;


//    block_state blocks[N_AXIS];

//    StepMotor* motor;

    // Указатель типа отправки.
    eCDstate state;

    void calculateAccel();

//    void buildGgroup();

    void buildMgroup();

//    void buildG0command();

    void setParam_coord(sGparam* param);

    void setWorkValue(QString value, size_t axis_num);

    void initWorkAray();

//    bool isPlaneHasSteps();

    void setProfileValue();

    void setDirection_bits();

//    void planner_recalculate();

    void buildComdata();

    void buildG2Command();

    void setupThread();

    runStates runState;

    void _run();

};

#endif // COMDATA_H
