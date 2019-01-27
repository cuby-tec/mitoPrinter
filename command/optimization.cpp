#include "optimization.h"

#include <QDebug>
#include <assert.h>


#define cout    qDebug()<<__FILE__<<__LINE__
#define MIN(A,B) A<B?A:B

Optimization::Optimization()
{
	coefficien = 1.0;
//	step = 0.0;
}

void Optimization::calc(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{
    QList<sControlBlocks>::iterator bIterator;// block iterator.

    QList<sControlBlocks>::iterator agIterator;

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.



    int32_t acs_change_x, acs_change_y;

#define BJ  acs_change_x
#define BK  acs_change_y
#define G4 bIterator->bb[X_AXIS].nominal_speed
#define block   bIterator
#define RAD_SPEED    opti_BI[counter].rad_speed

    struct _Opti{
        double_t    k;
        double_t rad_speed;
    };


//    blocks.end();
//START for all paires
    bIterator = blocks.end();
    agIterator = blocks.end();
    --agIterator;
    rIterator = action.queue.end();
    size_t counter = blocks.size();
    _Opti *opti_BI  = static_cast<_Opti*>( malloc( counter * sizeof (_Opti)));
    --counter;

    assert(opti_BI != nullptr);

    for(int i=0;i<blocks.size();i++)
    {
        opti_BI[i].k = 1.0;
    }


    cout<<"\t value:"<<blocks.size()<<"\trequest:"<<action.queue.size();

/*
    while(bIterator != blocks.begin()+1)
    {
        --bIterator;
        --rIterator;
        --agIterator;

        _controlBlocks *sc = &bIterator->bb;

        sSegment* segment = &rIterator->payload.instrument1_parameter;

//        sControlBlocks* sco = bIterator;
        cout<<bIterator->bb[0].steps<<"\tAG:"<<agIterator->bb[0].steps<<"\tsement:"<<segment->head.linenumber;

    }*/

    // Load optimization params

    bIterator = blocks.end();
    --bIterator;
/*
    params[CURRENTPARAM].alfa = bIterator->bb[X_AXIS].alfa;
    params[CURRENTPARAM].radAccel = bIterator->bb[X_AXIS].acceleration;
    params[CURRENTPARAM].radSpeed = bIterator->bb[X_AXIS].nominal_speed; //G4
    params[CURRENTPARAM].steps = bIterator->bb[X_AXIS].steps;
    step = bIterator->bb[X_AXIS].steps;

    --bIterator;
    params[PREVPARAM].alfa = bIterator->bb[X_AXIS].alfa;
    params[PREVPARAM].radAccel = bIterator->bb[X_AXIS].acceleration;
    params[PREVPARAM].radSpeed = bIterator->bb[X_AXIS].nominal_speed; //G4
    params[PREVPARAM].steps = bIterator->bb[X_AXIS].steps;
*/

    LoadParams(*bIterator ); //load first(LAST) segment/block
    --bIterator;
    LoadParams(*bIterator ); //load second(LAST-1) segment/block

    coefficien = 1.0;

    opt(1.0);


//START
    index = blocks.size();
    bIterator = blocks.end();// current source
    agIterator = blocks.end(); // prev source
    --agIterator;
    --index;

    rIterator = action.queue.end();// destination

    LoadParams(*agIterator ); //load first(LAST) segment/block

    cout<<"Start compare ";

    cout<<"\nsteps\tacc_path\tspeed_path\tdecel_path\tBJx";

    while(bIterator != blocks.begin()){
        --bIterator;
        if(agIterator != blocks.begin()){
            --agIterator;
            --index;
        }
        --rIterator;

        _controlBlocks *sc = &bIterator->bb;

        sSegment* segment = &rIterator->payload.instrument1_parameter;

        //=IF(steps_x>0, accsteps_x-AG8,0)
        if(bIterator != blocks.begin()){
            acs_change_x = static_cast<int32_t>( block->bb[X_AXIS].accelerate_until - agIterator->bb[X_AXIS].accelerate_until);//BJ
            acs_change_y = static_cast<int32_t>( block->bb[Y_AXIS].accelerate_until - agIterator->bb[Y_AXIS].accelerate_until);//BK
        }
        else {
            acs_change_x = static_cast<int32_t>( block->bb[X_AXIS].accelerate_until);
            acs_change_y = static_cast<int32_t>( block->bb[Y_AXIS].accelerate_until);
        }

        //block->acceleration->racc   block->nominal_speed
        //radian_accel
//        bIterator->bb[X_AXIS].alfa
        opti_BI[counter].rad_speed = G4;

        double_t acs = pow(G4,2.0)/(2.0* bIterator->bb[X_AXIS].alfa *bIterator->bb[X_AXIS].acceleration);
        double_t acl = block->bb[X_AXIS].steps*block->bb[X_AXIS].deceleration/(block->bb[X_AXIS].acceleration+block->bb[X_AXIS].acceleration);

//        double_t acs_change_x =
        //=IF((ABS(steps_y)>=ABS(BK34))AND(ABS(steps_x)>=ABS(BJ34)))
        bool BN = (block->bb[X_AXIS].steps >= static_cast<uint32_t>(abs(BJ))) && (block->bb[Y_AXIS].steps >= static_cast<uint32_t>(abs(BK)));

        LoadParams(*agIterator);//push
        if( isOptimized() )
            continue;
        //  Coefficient should be find. Start value 1.0.
        opt(1.0);

//        cout<<"accelerate until:"<< bIterator->bb[X_AXIS].accelerate_until <<"\tsteps:"<<bIterator->bb[X_AXIS].steps
//           <<"\tbj:"<<acs_change_x <<"\tAGsteps:"<<agIterator->bb[X_AXIS].steps;

//        cout<<"steps:"<<block->bb[X_AXIS].steps<<"\tBJx:"<<BJ<<"\tBKy:"<<BK<<"\tBN:"<<BN;
        qDebug()<<block->bb[X_AXIS].steps
               <<"\t"<<block->bb[X_AXIS].accelerate_until
               <<"\t"<< block->bb[X_AXIS].decelerate_after - block->bb[X_AXIS].accelerate_until
//               << "\t"<< block->bb[X_AXIS].steps -block->bb[X_AXIS].decelerate_after
              <<"\t"<<block->bb[Y_AXIS].steps
             <<"\t"<<block->bb[Y_AXIS].accelerate_until
            <<"\t"<<block->bb[Y_AXIS].decelerate_after - block->bb[Y_AXIS].accelerate_until
               << "\t"<<BJ
               <<"\t"<<BK
               <<"\t"<<BN
              <<"\t"<< (MIN(acs,acl))
              <<"\t"<<counter--;
    }


    free(opti_BI);
}

double_t
Optimization::accPath(sAccPathParam* params, double_t coef) {
	return ( pow( (coef * params->radSpeed) ,2.0)/(2.0*params->alfa*params->radAccel) );
}

double_t
Optimization::stepsChandSpeed(int axis, double_t coefX)
{
//	return ( (accPath_a>0)?(accPath_a - accPath_b):(accPath_a+accPath_b));
    double_t pathA = 0.0, pathB = 0.0, result;
#if OPTI_V==1
	pathA = accPath(&params[CURRENTPARAM], coefX);//19
	pathB = accPath(&params[PREVPARAM],coefficien);//18
#endif
    switch (axis) {
    case X_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_x,aParamOptim[CURRENTPARAM].para_x.koptim);
        pathB = accPath(&aParamOptim[PREVPARAM].para_x,coefX);//coefficien
        break;

    case Y_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_y,aParamOptim[CURRENTPARAM].para_y.koptim);
        pathB = accPath(&aParamOptim[PREVPARAM].para_y,coefX);
        break;

    case Z_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_z,aParamOptim[CURRENTPARAM].para_z.koptim);
        pathB = accPath(&aParamOptim[PREVPARAM].para_z,coefX);
        break;

    case E_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_e,aParamOptim[CURRENTPARAM].para_e.koptim);
        pathB = accPath(&aParamOptim[PREVPARAM].para_e,coefX);
        break;

    }
    result = ((pathA - pathB));

    return result;
}

double_t Optimization::aStepsChangeSpeed(int axis)
{
    double_t pathA = 0.0, pathB = 0.0;
    switch (axis)
    {
    case X_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_x, aParamOptim[CURRENTPARAM].para_x.koptim);//19
        pathB = accPath(&aParamOptim[PREVPARAM].para_x, aParamOptim[PREVPARAM].para_x.koptim);//18
        break;
    case Y_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_y, aParamOptim[CURRENTPARAM].para_y.koptim);//19
        pathB = accPath(&aParamOptim[PREVPARAM].para_y, aParamOptim[PREVPARAM].para_y.koptim);//18
        break;
    case Z_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_z, aParamOptim[CURRENTPARAM].para_z.koptim);//19
        pathB = accPath(&aParamOptim[PREVPARAM].para_z, aParamOptim[PREVPARAM].para_z.koptim);//18
        break;
    case E_AXIS:
        pathA = accPath(&aParamOptim[CURRENTPARAM].para_e, aParamOptim[CURRENTPARAM].para_e.koptim);//19
        pathB = accPath(&aParamOptim[PREVPARAM].para_e, aParamOptim[PREVPARAM].para_e.koptim);//18
        break;
    }
    return ((pathA - pathB));
}

//coefficient
double_t Optimization::opt(double_t X0) {
    double_t Xnext = 0.99 * X0;
    double_t X1 = X0, X2;
    double_t diff=0.0;

    opt_m1:
    for( int axis=0;axis<N_AXIS;axis++)
    {
        if( hasSteps(axis))
        {
            if( !isOptimized(axis) )
             {
                for(size_t i=0;i<5;i++)
                {
                    X2 = X1 - fn(axis,X1)*(X1 - Xnext)/(fn(axis,X1)-fn(axis,Xnext));

                    diff = Xnext - X2;
                    X1 = Xnext;
                    Xnext = X2;
                    if(fabs(diff)< 0.01)
                    {
                        aParamOptim[PREVPARAM].para_x.koptim = Xnext;
                        aParamOptim[PREVPARAM].para_y.koptim = Xnext;
                        aParamOptim[PREVPARAM].para_z.koptim = Xnext;
                        aParamOptim[PREVPARAM].para_e.koptim = Xnext;
                        goto opt_m1;
//                        break;
                    }
                }
            }
        }
    }

	return Xnext;
}

double_t Optimization::fn(int axis, double_t X) {
    double_t chang = stepsChandSpeed(axis, X);
//    double_t chang = aStepsChangeSpeed(X_AXIS);
    double_t step = 0.0;
	double_t result;
    switch (axis) {
    case X_AXIS:
        step = aParamOptim[CURRENTPARAM].para_x.steps;
        break;

    case Y_AXIS:
        step = aParamOptim[CURRENTPARAM].para_y.steps;
        break;

    case Z_AXIS:
        step = aParamOptim[CURRENTPARAM].para_z.steps;
        break;

    case E_AXIS:
        step = aParamOptim[CURRENTPARAM].para_e.steps;
        break;
    }


	result = chang>0?(chang - step):(chang+step);
    return result;
}

void Optimization::LoadParams(sControlBlocks &blocks)
{
    sParamOptim* paramoptim = &aParamOptim[PREVPARAM];//CURRENTPARAM
    sParamOptim* prev = &aParamOptim[CURRENTPARAM];

    memcpy(prev,paramoptim,sizeof(sParamOptim));

    paramoptim->para_x.alfa = blocks.bb[X_AXIS].alfa;
    paramoptim->para_x.steps = blocks.bb[X_AXIS].steps;
    paramoptim->para_x.radAccel = blocks.bb[X_AXIS].acceleration;
    paramoptim->para_x.radSpeed = blocks.bb[X_AXIS].nominal_speed;
    paramoptim->para_x.koptim = 1.0;

    paramoptim->para_y.alfa = blocks.bb[Y_AXIS].alfa;
    paramoptim->para_y.steps = blocks.bb[Y_AXIS].steps;
    paramoptim->para_y.radAccel = blocks.bb[Y_AXIS].acceleration;
    paramoptim->para_y.radSpeed = blocks.bb[Y_AXIS].nominal_speed;
    paramoptim->para_y.koptim = 1.0;

    paramoptim->para_z.alfa = blocks.bb[Z_AXIS].alfa;
    paramoptim->para_z.steps = blocks.bb[Z_AXIS].steps;
    paramoptim->para_z.radAccel = blocks.bb[Z_AXIS].acceleration;
    paramoptim->para_z.radSpeed = blocks.bb[Z_AXIS].nominal_speed;
    paramoptim->para_z.koptim = 1.0;

    paramoptim->para_e.alfa = blocks.bb[E_AXIS].alfa;
    paramoptim->para_e.steps = blocks.bb[E_AXIS].steps;
    paramoptim->para_e.radAccel = blocks.bb[E_AXIS].acceleration;
    paramoptim->para_e.radSpeed = blocks.bb[E_AXIS].nominal_speed;
    paramoptim->para_e.koptim = 1.0;

}


/*
    =( ABS( INDEX(step,$A5,1)) >= ABS( INDEX(acch,$A5,1 )))
     AND (ABS (INDEX(step,$A5,2))>= ABS( INDEX(acch,$A5,2)))
     AND (ABS(INDEX(step,$A5,3))>=ABS( INDEX(step,$A5,3)))
     AND (ABS( INDEX(step,$A5,4)) >=ABS( INDEX(acch,$A5,4)))
*/
bool Optimization::isOptimized()
{
    bool result = false ;
    double_t acch;

    acch = aStepsChangeSpeed(X_AXIS);
    if(aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
        result = true;  else { result = false;  }

    if(result == true){
        acch = aStepsChangeSpeed(Y_AXIS);
        if(aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
            result = true;  else { result = false;  }
    }

    if(result == true){
        acch = aStepsChangeSpeed(Z_AXIS);
        if(aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
            result = true;  else { result = false;  }
    }

    if(result == true){
        acch = aStepsChangeSpeed(E_AXIS);
        if(aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
            result = true;  else { result = false;  }
    }

    return result;
}

bool Optimization::isOptimized(int axis)
{
    bool result = false ;
    double_t acch;
    acch =  round(1000.0* aStepsChangeSpeed(axis))/1000.0;

    switch (axis) {
    case X_AXIS:
        if(aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
            result = true;  else { result = false;  }
        break;

    case Y_AXIS:
        if(aParamOptim[CURRENTPARAM].para_y.steps >= fabs(acch))
            result = true;  else { result = false;  }
        break;

    case Z_AXIS:
        if(aParamOptim[CURRENTPARAM].para_z.steps >= fabs(acch))
            result = true;  else { result = false;  }
        break;

    case E_AXIS:
        if(aParamOptim[CURRENTPARAM].para_e.steps >= fabs(acch))
            result = true;  else { result = false;  }
        break;
    }

    return result;
}

bool Optimization::hasSteps(int axis)
{
    uint steps = 0;
    switch (axis)
    {
    case X_AXIS:
        steps = aParamOptim[CURRENTPARAM].para_x.steps;
        break;
    case Y_AXIS:
        steps = aParamOptim[CURRENTPARAM].para_y.steps;
        break;
    case Z_AXIS:
        steps = aParamOptim[CURRENTPARAM].para_z.steps;
        break;
    case E_AXIS:
        steps = aParamOptim[CURRENTPARAM].para_e.steps;
        break;

    }
   return(steps != 0);
}




