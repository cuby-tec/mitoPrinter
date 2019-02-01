#include "optimization.h"
#include <cstdio>
#include <QDebug>
#include <assert.h>
#include <LinearMath/Vector3.h>

#define cout    qDebug()<<__FILE__<<__LINE__
#define MIN(A,B) A<B?A:B

Optimization::Optimization()
{
	coefficien = 1.0;
    //	step = 0.0;
}

Optimization::~Optimization()
{
    free(opti_BI);
    cout<<"Exit optimization.";
}

#define BJ  acs_change_x
#define BK  acs_change_y
#define G4 bIterator->bb[X_AXIS].nominal_speed
#define block   bIterator
#define RAD_SPEED    opti_BI[counter].rad_speed
#define OPTI_K		opti_BI[counter].k

void Optimization::calc(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{

    QList<sControlBlocks>::iterator bIterator;// block iterator.

    QList<sControlBlocks>::iterator agIterator;

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.

    int32_t acs_change_x, acs_change_y;

    //vars for all paires
    bIterator = blocks.end();
    agIterator = blocks.end();
    --agIterator;
    rIterator = action.queue.end();

    counter = blocks.size();

    opti_BI  = static_cast<_Opti*>( malloc( counter * sizeof (_Opti)));

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

    LoadParams(*bIterator ); //load first(LAST) segment/block

    --bIterator;
    LoadParams(*bIterator ); //load second(LAST-1) segment/block

    coefficien = 1.0;

    opt(1.0);


//START
//    index = blocks.size();
    bIterator = blocks.end();// current source
    agIterator = blocks.end(); // prev source
    --agIterator;
//    --counter;
//    --index;

    rIterator = action.queue.end();// destination

    LoadParams(*agIterator ); //load first(LAST) segment/block
    // First segment
//    agIterator->bb[X_AXIS].enter_speed = 0.0;
//    agIterator->bb[Y_AXIS].enter_speed = 0.0;
//    agIterator->bb[Z_AXIS].enter_speed = 0.0;
//    agIterator->bb[E_AXIS].enter_speed = 0.0;

    cout<<"Start optimization ";

//    cout<<"\nsteps\tacc_path\tspeed_path\tdecel_path\tBJx";

    // Main circle analize optimizaation
    while(bIterator != blocks.begin()){
        --bIterator;
        --rIterator;
        --counter;

        _controlBlocks *sc = &bIterator->bb;//  source data
        sSegment* segment = &rIterator->payload.instrument1_parameter;// destination data

        if(agIterator != blocks.begin()){
            --agIterator;
//            --index;
            //Enter speed
            bIterator->bb[X_AXIS].enter_speed = agIterator->bb[X_AXIS].nominal_speed;
            bIterator->bb[Y_AXIS].enter_speed = agIterator->bb[Y_AXIS].nominal_speed;
            bIterator->bb[Z_AXIS].enter_speed = agIterator->bb[Z_AXIS].nominal_speed;
            bIterator->bb[E_AXIS].enter_speed = agIterator->bb[E_AXIS].nominal_speed;
        }else {
            bIterator->bb[X_AXIS].enter_speed = 0.0;
            bIterator->bb[Y_AXIS].enter_speed = 0.0;
            bIterator->bb[Z_AXIS].enter_speed = 0.0;
            bIterator->bb[Z_AXIS].enter_speed = 0.0;
        }

        //block->acceleration->racc   block->nominal_speed
        //radian_accel

//        double_t acs = pow(G4,2.0)/(2.0* bIterator->bb[X_AXIS].alfa *bIterator->bb[X_AXIS].acceleration);
//        double_t acl = block->bb[X_AXIS].steps*block->bb[X_AXIS].deceleration/(block->bb[X_AXIS].acceleration+block->bb[X_AXIS].acceleration);
//        bool BN = (block->bb[X_AXIS].steps >= static_cast<uint32_t>(abs(BJ))) && (block->bb[Y_AXIS].steps >= static_cast<uint32_t>(abs(BK)));

        double_t angle = calcAngle(*bIterator, *agIterator);
        cout<<"angle:"<<angle*180/3.1415;
        double_t co = 1.0;
        LoadParams(*agIterator);//push
        if( !isOptimized() ){
            //  Coefficient should be find. Start value 1.0.
            co = opt(1.0);
            cout<<"Modified:"<<counter<<"-1\tcoefficient:"<<OPTI_K;
        }
//            continue;

        agIterator->bb[X_AXIS].nominal_speed *= co;
        agIterator->bb[Y_AXIS].nominal_speed *= co;
        agIterator->bb[Z_AXIS].nominal_speed *= co;
        agIterator->bb[E_AXIS].nominal_speed *= co;

        bIterator->bb[X_AXIS].enter_speed = agIterator->bb[X_AXIS].nominal_speed;
        bIterator->bb[Y_AXIS].enter_speed = agIterator->bb[Y_AXIS].nominal_speed;
        bIterator->bb[Z_AXIS].enter_speed = agIterator->bb[Z_AXIS].nominal_speed;
        bIterator->bb[E_AXIS].enter_speed = agIterator->bb[E_AXIS].nominal_speed;

        // Count value of transition steps from enter speed to nomnal.
        double_t sv;

        for (uint i=0 ;i<N_AXIS;++i) {
            sv = stepsTransition(i, bIterator->bb[i].enter_speed, bIterator->bb[i].nominal_speed);
//            cout<<"sv:"<<sv<<"\tenter:"<<bIterator->bb[i].enter_speed
//            std::printf( "sv:%7.2f enter:%7.2f nom:%7.2f acc:%d   " ,sv,bIterator->bb[i].enter_speed, bIterator->bb[i].nominal_speed, bIterator->bb[i].accelerate_until  );
            cout<< "axis:"<<i<<" sv:"<<sv<<" enter:"<<bIterator->bb[i].enter_speed<<"nom:"
                << bIterator->bb[i].nominal_speed<<" acc:"<< bIterator->bb[i].accelerate_until;
        }



//        cout<<"accelerate until:"<< bIterator->bb[X_AXIS].accelerate_until <<"\tsteps:"<<bIterator->bb[X_AXIS].steps
//           <<"\tbj:"<<acs_change_x <<"\tAGsteps:"<<agIterator->bb[X_AXIS].steps;

//        cout<<"steps:"<<block->bb[X_AXIS].steps<<"\tBJx:"<<BJ<<"\tBKy:"<<BK<<"\tBN:"<<BN;
      /*
       *   qDebug()<<block->bb[X_AXIS].steps
               <<"\t"<<block->bb[X_AXIS].accelerate_until
               <<"\t"<< block->bb[X_AXIS].decelerate_after - block->bb[X_AXIS].accelerate_until
//               << "\t"<< block->bb[X_AXIS].steps -block->bb[X_AXIS].decelerate_after
              <<"\t"<<block->bb[Y_AXIS].steps
             <<"\t"<<block->bb[Y_AXIS].accelerate_until
            <<"\t"<<block->bb[Y_AXIS].decelerate_after - block->bb[Y_AXIS].accelerate_until
//               << "\t"<<BJ
//               <<"\t"<<BK
//               <<"\t"<<BN
//              <<"\t"<< (MIN(acs,acl))
              <<"\t"<<counter;
        */
    }
    cout<<"End optimization.";
    //TODOH  data Synthesis

    //    free(opti_BI); << in destructor
}

double_t Optimization::stepsTransition(uint axis, double_t enterSpeed, double_t nominalSpeed)
{
    double_t result = pow(nominalSpeed,2.0);
    result -= pow(enterSpeed,2.0);
    switch (axis) {
    case X_AXIS:
        result /= 2.0*aParamOptim[CURRENTPARAM].para_x.alfa*aParamOptim[CURRENTPARAM].para_x.radAccel ;
        break;

    case Y_AXIS:
        result /= 2.0*aParamOptim[CURRENTPARAM].para_y.alfa*aParamOptim[CURRENTPARAM].para_y.radAccel ;
        break;

    case Z_AXIS:
        result /= 2.0*aParamOptim[CURRENTPARAM].para_z.alfa*aParamOptim[CURRENTPARAM].para_z.radAccel ;
        break;

    case E_AXIS:
        result /= 2.0*aParamOptim[CURRENTPARAM].para_e.alfa*aParamOptim[CURRENTPARAM].para_e.radAccel ;
        break;
    }
    return result;
}
/*
double_t
Optimization::accPath(sAccPathParam* params, double_t coef) {
	return ( pow( (coef * params->radSpeed) ,2.0)/(2.0*params->alfa*params->radAccel) );
}
*/
/**
 * @brief Optimization::aStepsChangeSpeed
 * ROUNDDOWN( 1/(2*alfaX*INDEX(radaccel,$A7,1))*(INDEX(radspeed,$A7,2)^2-INDEX(radspeed,$A7,1)^2) )
 * @param axis
 * @return
 */
double_t
Optimization::stepsChandSpeed(int axis, double_t coef)
{
//	return ( (accPath_a>0)?(accPath_a - accPath_b):(accPath_a+accPath_b));
//    double_t pathA = 0.0, pathB = 0.0, result;
    double_t path = 0.0;
#if OPTI_V==1
    pathA = accPath(&params[CURRENTPARAM], coef);//19
	pathB = accPath(&params[PREVPARAM],coefficien);//18
#endif
    switch (axis) {
    case X_AXIS:
//        pathA = accPath(&aParamOptim[CURRENTPARAM].para_x,aParamOptim[CURRENTPARAM].para_x.koptim);
//        pathB = accPath(&aParamOptim[PREVPARAM].para_x,coef);//coefficien
        path =  pow(aParamOptim[CURRENTPARAM].para_x.radSpeed * aParamOptim[CURRENTPARAM].para_x.koptim,2.0) ;
        if( (aParamOptim[CURRENTPARAM].para_x.radSpeed - aParamOptim[PREVPARAM].para_x.radSpeed) != 0.0 )
            path -= pow(aParamOptim[PREVPARAM].para_x.radSpeed * coef,2.0) ;
        path /= 2.0*aParamOptim[CURRENTPARAM].para_x.alfa*aParamOptim[CURRENTPARAM].para_x.radAccel ;

        break;

    case Y_AXIS:
//        pathA = accPath(&aParamOptim[CURRENTPARAM].para_y,aParamOptim[CURRENTPARAM].para_y.koptim);
//        pathB = accPath(&aParamOptim[PREVPARAM].para_y,coef);
        path =  pow(aParamOptim[CURRENTPARAM].para_y.radSpeed * aParamOptim[CURRENTPARAM].para_y.koptim,2.0) ;
        if( (aParamOptim[CURRENTPARAM].para_y.radSpeed - aParamOptim[PREVPARAM].para_y.radSpeed) != 0.0 )
            path -= pow(aParamOptim[PREVPARAM].para_y.radSpeed * coef,2.0) ;
        path /= 2.0*aParamOptim[CURRENTPARAM].para_y.alfa*aParamOptim[CURRENTPARAM].para_y.radAccel ;

        break;

    case Z_AXIS:
//        pathA = accPath(&aParamOptim[CURRENTPARAM].para_z,aParamOptim[CURRENTPARAM].para_z.koptim);
//        pathB = accPath(&aParamOptim[PREVPARAM].para_z,coef);
        path =  pow(aParamOptim[CURRENTPARAM].para_z.radSpeed * aParamOptim[CURRENTPARAM].para_z.koptim,2.0) ;
        if( (aParamOptim[CURRENTPARAM].para_z.radSpeed - aParamOptim[PREVPARAM].para_z.radSpeed) != 0.0 )
            path -= pow(aParamOptim[PREVPARAM].para_z.radSpeed * coef,2.0) ;
        path /= 2.0*aParamOptim[CURRENTPARAM].para_z.alfa*aParamOptim[CURRENTPARAM].para_z.radAccel ;

        break;

    case E_AXIS:
//        pathA = accPath(&aParamOptim[CURRENTPARAM].para_e,aParamOptim[CURRENTPARAM].para_e.koptim);
//        pathB = accPath(&aParamOptim[PREVPARAM].para_e,coef);
        path =  pow(aParamOptim[CURRENTPARAM].para_e.radSpeed * aParamOptim[CURRENTPARAM].para_e.koptim,2.0) ;
        if( (aParamOptim[CURRENTPARAM].para_e.radSpeed - aParamOptim[PREVPARAM].para_e.radSpeed) != 0.0 )
            path -= pow(aParamOptim[PREVPARAM].para_e.radSpeed * coef,2.0) ;
        path /= 2.0*aParamOptim[CURRENTPARAM].para_e.alfa*aParamOptim[CURRENTPARAM].para_e.radAccel ;

        break;

    }
//    result = ((pathA - pathB));
//    return result;

    return path;
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
                Xnext = 0.99 * X0;
                X1 = X0;

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
                        OPTI_K = Xnext;
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
//    double_t acch;

    result = isOptimized(X_AXIS);

    if(result == true){
        result = isOptimized(Y_AXIS);
    }

    if(result == true){
        result = isOptimized(Z_AXIS);
    }

    if(result == true){
        result = isOptimized(E_AXIS);
    }

    return result;
}

bool Optimization::isOptimized(int axis)
{
    bool result = false ;
    double_t acch;
//    acch =  round(1000.0* aStepsChangeSpeed(axis))/1000.0;
    acch =  round(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_x.koptim));

    switch (axis) {
    case X_AXIS:
        if(aParamOptim[CURRENTPARAM].para_x.steps>0)
        {
            if (aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
                result = true;
            else
            { result = false;  }
        }else { result = true; }
        break;

    case Y_AXIS:
        if(aParamOptim[CURRENTPARAM].para_y.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_y.steps >= fabs(acch))
                result = true;  else { result = false;  }
        }else{  result = true; }

        break;

    case Z_AXIS:
        if(aParamOptim[CURRENTPARAM].para_z.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_z.steps >= fabs(acch))
                result = true;  else { result = false;  }
        }else{ result = true; }
        break;

    case E_AXIS:
        if(aParamOptim[CURRENTPARAM].para_e.steps>0)
        {
        if(aParamOptim[CURRENTPARAM].para_e.steps >= fabs(acch))
            result = true;  else { result = false;  }
        }else { result = true; }
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

/**
 * Вычисление угла между векторами.
 */
double_t Optimization::calcAngle(sControlBlocks& curr, sControlBlocks& next) {
	double_t result=0.0;

	tf2::Vector3 vectorCur;
	tf2::Vector3 vectorNext;

	vectorCur.setX(curr.bb[X_AXIS].path);
	vectorCur.setY(curr.bb[Y_AXIS].path);
	vectorCur.setZ(curr.bb[Z_AXIS].path);

	vectorNext.setX(next.bb[X_AXIS].path);
	vectorNext.setY(next.bb[Y_AXIS].path);
	vectorNext.setZ(next.bb[Z_AXIS].path);

	result = vectorCur.angle(vectorNext);

	return result;
}
