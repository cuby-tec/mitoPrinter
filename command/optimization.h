#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "scontrolblocks.h"
#include "gparcer/mitoaction.h"
#include <math.h>

struct sAccPathParam{
	double_t	radSpeed;
	double_t	radAccel;
	double_t	alfa;
	uint		steps;
	double_t	koptim;
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

class Optimization
{
public:
    Optimization();

    ~Optimization();

//QQueue<sControlBlocks> controlblocks;
    void calc(mito::Action_t &action, QQueue<sControlBlocks>& blocks);

    // acceleration path
    double_t accPath(sAccPathParam* params, double_t coef);

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



};

#endif // OPTIMIZATION_H
