#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "scontrolblocks.h"
#include "gparcer/mitoaction.h"
#include <math.h>

#include "step_motor/controller.h"

struct sAccPathParam{
	double_t	radSpeed;
	double_t	radAccel;
	double_t	alfa;
    double_t	koptim;
    double_t    enterSpeed;
    uint		steps;
    uint        tmp;
};

struct sParamOptim{
	sAccPathParam para_x;
	sAccPathParam para_y;
	sAccPathParam para_z;
	sAccPathParam para_e;
};

#define PREVPARAM		1	// for X1
#define CURRENTPARAM	0 	// for X0

//using namespace tf;


//-------defs

struct segmentDescription{
    int enterLevel; 		// enter speedLevel in current segment.
    int Acc;		// changing speed level in start part of current segment.
    int nominalLevel; 	// level in current segment.
    int Dec;		// changing speed in final part of current segment.
    int finalLevel; 		// level at the end of current segment.
    int steps;			// steps in current segment

};


struct accCompriseOf {
    int32_t change;   		// sum of dec and acc.
    int32_t dec;   			// deceleration in current segment.
    int32_t acc;   			// acceleration in next segment.
    uint32_t speedlevel;
/*
    int a_enterLevel; 		// enter speedLevel in current segment.
    int a_Acc;		// changing speed level in start part of current segment.
    int a_nominalLevel; 	// level in current segment.
    int a_Dec;		// changing speed in final part of current segment.
    int a_finalLevel; 		// level at the end of current segment.
    int a_steps;			// steps in current segment

    int b_enterLevel; 		// enter speedLevel in next segment.
    int b_Acc;		// changing speed level in start part of next segment.
    int b_nominalLevel; 	// level in next segment.
    int b_Dec;		// changing speed in final part of next segment.
    int b_finalLevel; 		// level at the end of next segment.
    int b_steps;			// steps in next segment
*/
    struct segmentDescription a;
    struct segmentDescription b;
};

//-------- function


class Optimization
{
public:
    Optimization(Controller* control);

    ~Optimization();


    void smooth(mito::Action_t &action, QQueue<sControlBlocks>& blocks);

//QQueue<sControlBlocks> controlblocks;
    void calc(mito::Action_t &action, QQueue<sControlBlocks>& blocks);
/*
    // acceleration path
    double_t accPath(sAccPathParam* params, double_t coef);
*/
    double_t stepsTransition(uint axis, double_t enterSpeed, double_t nominalSpeed);

    //  steps for change speed
    double_t stepsChandSpeed(int axis, double_t coefX=1.0);

    // speed correction coefficient
    double_t opt(double_t X0);

    double_t fn(int axis, double_t X);

//    sAccPathParam params[2];

//    double_t step;

    double_t coefficien;//18

    sParamOptim aParamOptim[2];

    void LoadParams( sControlBlocks & blocks);

    // check array sParamOptim aParamOptim[2]
    bool isOptimized();
    bool isOptimized(int axis);

    bool hasSteps(int axis);

    int index;// DEBUG

    size_t counter;//OPTI_K

    struct _Opti{
        double_t    k;
        double_t rad_speed;
    };
    _Opti *opti_BI;

    double_t calcAngle(sControlBlocks& curr, sControlBlocks& next);

    void _calcLevel(accCompriseOf data[N_AXIS],size_t i, bool isNoMoveNext, bool isNoMoveCurrent);


    bool recalc; // need recalc - true,

    Controller* controller;

private:
    block_state_t* _block;

    void fillSegment(block_state_t* bs, sControl& sg);


};

#endif // OPTIMIZATION_H
