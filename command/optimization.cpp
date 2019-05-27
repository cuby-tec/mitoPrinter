#include "optimization.h"
#include <cstdio>
#include <QDebug>
#include <assert.h>
#include <math.h>
#include <LinearMath/Vector3.h>
#include <float.h>

//-------------- defs

#define cout    qDebug()<<__FILE__<<__LINE__
#define MIN(A,B) A<B?A:B

#define BJ  acs_change_x
#define BK  acs_change_y
#define G4 bIterator->bb[X_AXIS].nominal_speed
#define block   bIterator
#define RAD_SPEED    opti_BI[counter].rad_speed
#define OPTI_K		opti_BI[counter].k

#define agI  agIterator->bb
#define bI   bIterator->bb


struct accDecSteps{
    int acc;   // accelerate steps
    int flat;  // flat steps
    int dec;   // decelerate steps
};

struct accCompriseOf {
    int32_t change;    // sum of dec and acc
    int32_t dec;   //decceleration
    int32_t acc;   //acceleration
    uint32_t speedlevel;
};

//---------- function

Optimization::Optimization(Controller *control)
{
	coefficien = 1.0;
    //	step = 0.0;
    recalc = false;
    this->controller = control;
}

Optimization::~Optimization()
{
    free(opti_BI);
    cout<<"Exit optimization.";
}

static uint32_t rowcounter = 0;
/**
 * @brief Optimization::smooth
 * @param action
 * @param blocks
 *                  TODO
 */
void Optimization::smooth(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{
    QList<sControlBlocks>::iterator bIterator;// block iterator.
    QList<sControlBlocks>::iterator agIterator;

    volatile uint32_t size_sControl = sizeof (svSegment);

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.

    bIterator = blocks.begin(); // current block

    agIterator = blocks.begin();
    agIterator++;                  // next block

    rIterator = action.queue.begin();   // destination.

    struct accCompriseOf acco[N_AXIS]; // vector acceleration/deceleration

    // Main circle analize optimizaation
    while(agIterator != blocks.end()){


        sControlBlocks sc = *bIterator;//  source data:current block
        sControlBlocks nc = *agIterator;    // next data: next block

        svSegment* segment = &rIterator->payload.instrument1_parameter;// destination data

        //For every axis calculate steps to change speed.
        // axis circle.
        for(uint32_t i=0;i<N_AXIS;i++){
//            cout<<"i:"<<i<<"\tsteps:"<<sc.bb[i].steps;
            if(sc.bb[i].steps == 0 && nc.bb[i].steps == 0)
                continue;
//            double_t currBlockSpeed = sc.bb[i].nominal_speed;
//            double_t nextBlockSpeed = nc.bb[i].nominal_speed;
            double_t currBlockSpeed = 0;
            if(sc.bb[i].steps > 1 )
                currBlockSpeed =  sqrt( sc.bb[i].alfa*2.0*sc.bb[i].acceleration*sc.bb[i].accelerate_until );
            else if (sc.bb[i].steps == 1) {
                currBlockSpeed =  sqrt( sc.bb[i].alfa*2.0*sc.bb[i].acceleration* 1.0 );
            }

            if(sc.bb[i].steps == 0){
                sc.bb[i].speedLevel = 0;
            }else if (sc.bb[i].steps < 4 ) {
                sc.bb[i].speedLevel = 1;
            }

            if(nc.bb[i].steps == 0){
                nc.bb[i].speedLevel = 0;
            }else if (nc.bb[i].steps < 4 ) {
                nc.bb[i].speedLevel = 1;
            }



            // Calculate changeLevel
            double_t nextBlockSpeed = 0;
//            double_t changLevel = 0;
            int32_t changLevel = 0;

            if(nc.bb[i].steps > 1)
                    nextBlockSpeed = sqrt( nc.bb[i].alfa*2.0*nc.bb[i].acceleration*nc.bb[i].accelerate_until );
            else if (nc.bb[i].steps == 1) {
                nextBlockSpeed = sqrt( nc.bb[i].alfa*2.0*nc.bb[i].acceleration * 1.0 );
            }

            uint32_t scSteps = sc.bb[i].steps, ncSteps = nc.bb[i].steps;

            if(scSteps <= 1 && ncSteps <= 1 )
            {
                if((scSteps == 1 && ncSteps == 1 )||(scSteps == 0 && ncSteps == 0) )
                    changLevel = 0;
                else if (scSteps == 0 && ncSteps == 1  ) {
                    changLevel = 1;
                }else {
                    changLevel = -1;
                }
            }else{
//                changLevel = (pow(nextBlockSpeed,2.0) - pow(currBlockSpeed,2.0))/(sc.bb[i].alfa*2.0*sc.bb[i].acceleration);
                changLevel =  static_cast<int32_t>(nc.bb[i].speedLevel) - static_cast<int32_t>(sc.bb[i].speedLevel);
            }

            acco[i].change = changLevel;
            acco[i].speedlevel = sc.bb[i].speedLevel;

            if(abs(acco[i].change) < static_cast<int32_t>( scSteps )){
                acco[i].acc = 0;
                acco[i].dec = acco[i].change;
            }else {
                double_t k = scSteps/(scSteps+ncSteps);
                if(changLevel >= 0){     //if positive
                    acco[i].dec =  static_cast<int32_t>( ceil(acco[i].change*k ));
                }else { // negative
                    acco[i].dec =  static_cast<int32_t>( round(acco[i].change*k ));
                }
                acco[i].acc = acco[i].change - acco[i].dec;
            }

             uint32_t cnt;
            if(nextBlockSpeed >= DBL_EPSILON)// if(nextBlockSpeed != 0)
                uint32_t cnt = controller->calcAxisRate(i,nextBlockSpeed);
            else {
                 cnt = 0xfffffe;
            }


#if DEBUG_LEVEL == 0
//            if(i==1)
//      cout<<"axis:"<<i<<"  changLevel:" << changLevel<<"("<<sc.bb[i].speedLevel<<")"<<"\tsteps:"<<sc.bb[i].steps<<"\tspeed:"<<currBlockSpeed<<"("<<nextBlockSpeed<<")";// <<"\tcnt:"<<cnt;
//            cout<<"axis:"<<i<<"  changLevel:" << changLevel<<"("<<sc.bb[i].speedLevel<<")"
//               <<"\tsteps:"<<sc.bb[i].steps<<"["<<sc.bb[i].accelerate_until<<":"<< sc.bb[i].decelerate_after<<"]"
//               <<sc.bb[i].schem[0]<<":"<<sc.bb[i].schem[1]<<":"<<sc.bb[i].schem[2];

            cout<<"axis:"<<i <<"  speedLevel:" << acco[i].speedlevel
               <<" steps:"<<sc.bb[i].steps
              << acco[i].change<<"["<<acco[i].dec<<":"<<acco[i].acc<<"]"
              <<" rate:"<< segment->axis[i].nominal_rate <<" level:"<<segment->axis[i].speedLevel
              <<"row:"<<rowcounter;

#endif


        }

        rowcounter++;// debug value
        agIterator++;
        bIterator++;
        rIterator++;
    }

    bIterator = blocks.end();
    --bIterator;
    sControlBlocks sc = *bIterator ; //*agIterator;

    rIterator = action.queue.end();   // destination.
    --rIterator;
    svSegment* segment = &rIterator->payload.instrument1_parameter;// destination data

    // last row in queue
    for(uint32_t i=0;i<N_AXIS;i++){
        cout<<"axis:"<<i <<"  speedLevel:" << sc.bb[i].speedLevel
           <<"\tsteps:"<<sc.bb[i].steps
          << acco[i].change<<"["<<acco[i].acc<<":"<<"none]"
          << acco[i].change<<"["<<acco[i].dec<<":"<<acco[i].acc<<"]"
          <<" rate:"<< segment->axis[i].nominal_rate <<" level:"<<segment->axis[i].speedLevel
          <<"row:"<<rowcounter;
    }


}


void Optimization::calc(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{

    QList<sControlBlocks>::iterator bIterator;// block iterator.

    QList<sControlBlocks>::iterator agIterator;

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.

//    int32_t acs_change_x, acs_change_y;
    size_t maxIndex = static_cast<size_t>( blocks.size() ) - 1;

    counter = static_cast<size_t>( blocks.size());

    opti_BI  = static_cast<_Opti*>( malloc( counter * sizeof (_Opti)));
    assert(opti_BI != nullptr);

    for(int i=0;i<blocks.size();i++)
    {
        opti_BI[i].k = 1.0;

    }

    // Load optimization params
    // Initial data
    bIterator = blocks.end();
    agIterator = blocks.end(); // prev source
    --agIterator;
    while(bIterator != blocks.begin()){
        --bIterator;

        for (uint i = 0; i < N_AXIS; ++i) {
            bI[i].optimization = 1.0;
        }


        if(agIterator != blocks.begin()){
            --agIterator;
            // Load break points
            if((agI[X_AXIS].direction_bits != bI[X_AXIS].direction_bits)
                    || agI[Y_AXIS].direction_bits != bI[Y_AXIS].direction_bits
                    || agI[Z_AXIS].direction_bits != bI[Z_AXIS].direction_bits
                    || agI[E_AXIS].direction_bits != bI[E_AXIS].direction_bits )
            {
                for (uint i = 0; i < N_AXIS; ++i) {
                    bI[i].enter_speed = 0.0;
                }
            }else{
                for (uint i = 0; i < N_AXIS; ++i) {
                    bI[i].enter_speed = agI[i].nominal_speed;
                }
            }
        }else{
            for (uint i = 0; i < N_AXIS; ++i) {
                bI[i].enter_speed = 0.0;
            }
        }
    }

    cout<<"\t value:"<<blocks.size()<<"\trequest:"<<action.queue.size();

    coefficien = 1.0;

    //vars for all paires
//    bIterator = blocks.end();
//    agIterator = blocks.end();
//    --agIterator;
//    rIterator = action.queue.end();

//START
    opti_start: // //////////////////////////

	counter = static_cast<size_t>( blocks.size());

    recalc = false;

//    bIterator = blocks.end();
//    --bIterator;

//    LoadParams(*bIterator ); //load first(LAST) segment/block
//
//    --bIterator;
//    LoadParams(*bIterator ); //load second(LAST-1) segment/block


//    opt(1.0);


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
        if(agIterator != blocks.begin())
        {
            --agIterator;
        }

        _controlBlocks *sc = &bIterator->bb;//  source data
        svSegment* segment = &rIterator->payload.instrument1_parameter;// destination data


        //block->acceleration->racc   block->nominal_speed
        //radian_accel


//        double_t angle = calcAngle(*bIterator, *agIterator);
//        cout<<"angle:"<<angle*180/3.1415;
        double_t co = 1.0;

        LoadParams(*agIterator);//
        if( !isOptimized() ){
            //  Coefficient should be found. Start value 1.0.
            co = opt(1.0);
            assert(! std::isnan(co) );
            cout<<"Modified:"<<counter<<"\tcoefficient:"<<OPTI_K;

            if(bI[X_AXIS].enter_speed > 0.0)
            {
                for (uint i = 0; i < N_AXIS; ++i) {
//                    agI[i].nominal_speed *= co; //?
                    agI[i].optimization = co; // optimization coefficient
                }

            }else {
                for (uint i=0;i < N_AXIS;++i) {
                    bI[i].optimization = co;
                }
            }
        }


        // Count value of transition steps from enter speed to nominal.
        double_t sv;
        accDecSteps adSteps;

        for (uint i=0 ;i<N_AXIS;++i) {
            if(bI[i].steps == 0)
            {
                adSteps.acc = 0;
                adSteps.dec = 0;
                adSteps.flat = 0;
                continue;
            }

            double_t _enter_speed;
            if(bI[i].enter_speed > 0){
                 _enter_speed = agI[i].nominal_speed*agI[i].optimization;
//                sv = stepsTransition(i, bI[i].enter_speed, bI[i].nominal_speed);
            }else {
                _enter_speed = bI[i].enter_speed;
            }
            sv = stepsTransition(i, _enter_speed, bI[i].nominal_speed);
            sv = trunc(sv); //round


            if(counter == maxIndex)
            {// Last segment
                adSteps.acc = static_cast<int>(sv);

                _enter_speed = bI[i].nominal_speed * bI[i].optimization;
                sv = stepsTransition(i,_enter_speed,0.0);

                adSteps.dec = static_cast<int>(sv);
//                adSteps.flat = static_cast<int>( bI[i].steps) - abs(adSteps.acc) - abs(adSteps.dec);

            }else{
                if(blocks.at(counter+1).bb[i].enter_speed == 0.0)
                {
                    // sv = stepsTransition(i,bI[i].nominal_speed,0.0);
                    _enter_speed = bI[i].nominal_speed * bI[i].optimization;
                    sv = stepsTransition(i,_enter_speed,0.0);
                    adSteps.dec = static_cast<int>(sv);
                    // adSteps.flat = static_cast<int>( bI[i].steps) - abs(adSteps.acc) - abs(adSteps.dec);
                }else{
//                    sv = stepsTransition(i, bI[i].enter_speed, bI[i].nominal_speed);
                    adSteps.acc = static_cast<int>(sv);
                    adSteps.dec = 0;
                }
            }
            adSteps.flat = static_cast<int>( bI[i].steps) - abs(adSteps.acc) - abs(adSteps.dec);

            cout<<"count:"<<counter <<"axis:"<<i
               <<" acc:"<<adSteps.acc <<" flat:"<<adSteps.flat <<" dcc:"<<adSteps.dec <<" steps:"<<bI[i].steps
              <<" coptim:"<<bI[i].optimization <<" enterS:"<<bI[i].enter_speed <<"nomS:"<<bI[i].nominal_speed;

            assert(adSteps.flat>=0);
//            std::printf( "sv:%7.2f enter:%7.2f nom:%7.2f acc:%d   " ,sv,bIterator->bb[i].enter_speed, bIterator->bb[i].nominal_speed, bIterator->bb[i].accelerate_until  );
        }
        // Calculate acceleration/deceleration scheme


    }

    if(recalc == true)
    {
        //next loop recalculation needed.
        goto opti_start;
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
        if(aParamOptim[CURRENTPARAM].para_x.enterSpeed != 0.0)
        {
            path =  pow(aParamOptim[CURRENTPARAM].para_x.radSpeed * aParamOptim[CURRENTPARAM].para_x.koptim,2.0) ;
            if( (aParamOptim[CURRENTPARAM].para_x.radSpeed - aParamOptim[PREVPARAM].para_x.radSpeed) != 0.0 )
    //            path -= pow(aParamOptim[PREVPARAM].para_x.radSpeed * coef,2.0) ;
                path -= pow( aParamOptim[CURRENTPARAM].para_x.enterSpeed * coef, 2.0);

        }else {
            path =  pow(aParamOptim[CURRENTPARAM].para_x.radSpeed * coef ,2.0) ;
        }
        path /= 2.0*aParamOptim[CURRENTPARAM].para_x.alfa*aParamOptim[CURRENTPARAM].para_x.radAccel ;

        break;

    case Y_AXIS:
        if(aParamOptim[CURRENTPARAM].para_y.enterSpeed != 0.0)
        {
            path =  pow(aParamOptim[CURRENTPARAM].para_y.radSpeed * aParamOptim[CURRENTPARAM].para_y.koptim,2.0) ;
            if( (aParamOptim[CURRENTPARAM].para_y.radSpeed - aParamOptim[PREVPARAM].para_y.radSpeed) != 0.0 )
                //  path -= pow(aParamOptim[PREVPARAM].para_y.radSpeed * coef,2.0) ;
                path -= pow( aParamOptim[CURRENTPARAM].para_y.enterSpeed * coef, 2.0);
        }else {
            path =  pow(aParamOptim[CURRENTPARAM].para_y.radSpeed * coef ,2.0) ;
        }
        path /= 2.0*aParamOptim[CURRENTPARAM].para_y.alfa*aParamOptim[CURRENTPARAM].para_y.radAccel ;

        break;

    case Z_AXIS:
        if(aParamOptim[CURRENTPARAM].para_z.enterSpeed != 0.0)
        {
            path =  pow(aParamOptim[CURRENTPARAM].para_z.radSpeed * aParamOptim[CURRENTPARAM].para_z.koptim,2.0) ;
            if( (aParamOptim[CURRENTPARAM].para_z.radSpeed - aParamOptim[PREVPARAM].para_z.radSpeed) != 0.0 )
    //            path -= pow(aParamOptim[PREVPARAM].para_z.radSpeed * coef,2.0) ;
                path -= pow( aParamOptim[CURRENTPARAM].para_z.enterSpeed * coef, 2.0);
        }else {
            path =  pow(aParamOptim[CURRENTPARAM].para_z.radSpeed * coef ,2.0) ;
        }

        path /= 2.0*aParamOptim[CURRENTPARAM].para_z.alfa*aParamOptim[CURRENTPARAM].para_z.radAccel ;

        break;

    case E_AXIS:
        if(aParamOptim[CURRENTPARAM].para_e.enterSpeed != 0.0)
        {
            path =  pow(aParamOptim[CURRENTPARAM].para_e.radSpeed * aParamOptim[CURRENTPARAM].para_e.koptim,2.0) ;
            if( (aParamOptim[CURRENTPARAM].para_e.radSpeed - aParamOptim[PREVPARAM].para_e.radSpeed) != 0.0 )
                //            path -= pow(aParamOptim[PREVPARAM].para_e.radSpeed * coef,2.0) ;
                path -= pow( aParamOptim[CURRENTPARAM].para_e.enterSpeed * coef, 2.0);
        }else
        {
            path =  pow(aParamOptim[CURRENTPARAM].para_e.radSpeed * coef ,2.0) ;
        }

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
    bool self = false;

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
//                    double_t f1 = fn(axis,X1);
//                    f1 *= (X1-Xnext);
//                    double_t f2 = fn(axis,X1);
//                    double_t f3 = fn(axis,Xnext);
//                    f2 -= f3;

                    X2 = X1 - fn(axis,X1)*(X1 - Xnext)/(fn(axis,X1)-fn(axis,Xnext));

                    diff = Xnext - X2;
                    X1 = Xnext;
                    Xnext = X2;
                    if(fabs(diff)< 0.01)
                    {
                        switch (axis) {
                        case X_AXIS:
                            if(aParamOptim[CURRENTPARAM].para_x.enterSpeed == 0.0 )
                            {
                                self = true;
                            }
                            break;

                        case Y_AXIS:
                            if(aParamOptim[CURRENTPARAM].para_y.enterSpeed == 0.0 )
                            {
                                self = true;
                            }
                            break;

                        case Z_AXIS:
                            if(aParamOptim[CURRENTPARAM].para_z.enterSpeed == 0.0 )
                            {
                                self = true;
                            }

                            break;

                        case E_AXIS:
                            if(aParamOptim[CURRENTPARAM].para_e.enterSpeed == 0.0 )
                            {
                                self = true;
                            }
                            break;

                        }

                        if( self == true)
                        {
                            aParamOptim[CURRENTPARAM].para_x.koptim = Xnext;
                            aParamOptim[CURRENTPARAM].para_y.koptim = Xnext;
                            aParamOptim[CURRENTPARAM].para_z.koptim = Xnext;
                            aParamOptim[CURRENTPARAM].para_e.koptim = Xnext;
                            recalc = true;

                        }else
                        {
                            aParamOptim[PREVPARAM].para_x.koptim = Xnext;
                            aParamOptim[PREVPARAM].para_y.koptim = Xnext;
                            aParamOptim[PREVPARAM].para_z.koptim = Xnext;
                            aParamOptim[PREVPARAM].para_e.koptim = Xnext;
                        }

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
    paramoptim->para_x.enterSpeed = blocks.bb[X_AXIS].enter_speed;
    paramoptim->para_x.koptim = 1.0;

    paramoptim->para_y.alfa = blocks.bb[Y_AXIS].alfa;
    paramoptim->para_y.steps = blocks.bb[Y_AXIS].steps;
    paramoptim->para_y.radAccel = blocks.bb[Y_AXIS].acceleration;
    paramoptim->para_y.radSpeed = blocks.bb[Y_AXIS].nominal_speed;
    paramoptim->para_y.enterSpeed = blocks.bb[Y_AXIS].enter_speed;
    paramoptim->para_y.koptim = 1.0;

    paramoptim->para_z.alfa = blocks.bb[Z_AXIS].alfa;
    paramoptim->para_z.steps = blocks.bb[Z_AXIS].steps;
    paramoptim->para_z.radAccel = blocks.bb[Z_AXIS].acceleration;
    paramoptim->para_z.radSpeed = blocks.bb[Z_AXIS].nominal_speed;
    paramoptim->para_z.enterSpeed = blocks.bb[Z_AXIS].enter_speed;
    paramoptim->para_z.koptim = 1.0;

    paramoptim->para_e.alfa = blocks.bb[E_AXIS].alfa;
    paramoptim->para_e.steps = blocks.bb[E_AXIS].steps;
    paramoptim->para_e.radAccel = blocks.bb[E_AXIS].acceleration;
    paramoptim->para_e.radSpeed = blocks.bb[E_AXIS].nominal_speed;
    paramoptim->para_e.enterSpeed = blocks.bb[E_AXIS].enter_speed;
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
//    acch =  trunc(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_x.koptim));

    switch (axis) {
    case X_AXIS:
        if(aParamOptim[CURRENTPARAM].para_x.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_x.enterSpeed >0){
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_x.koptim));
            }
            else {
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[CURRENTPARAM].para_x.koptim));
            }

            if (aParamOptim[CURRENTPARAM].para_x.steps >= fabs(acch))
                result = true;
            else
            { result = false;  }
        }else { result = true; }
        break;

    case Y_AXIS:
        if(aParamOptim[CURRENTPARAM].para_y.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_y.enterSpeed >0){
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_y.koptim));
            }
            else {
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[CURRENTPARAM].para_y.koptim));
            }

            if(aParamOptim[CURRENTPARAM].para_y.steps >= fabs(acch))
                result = true;  else { result = false;  }
        }else{  result = true; }

        break;

    case Z_AXIS:
        if(aParamOptim[CURRENTPARAM].para_z.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_z.enterSpeed >0){
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_z.koptim));
            }
            else {
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[CURRENTPARAM].para_z.koptim));
            }
            if(aParamOptim[CURRENTPARAM].para_z.steps >= fabs(acch))
                result = true;  else { result = false;  }
        }else{ result = true; }
        break;

    case E_AXIS:
        if(aParamOptim[CURRENTPARAM].para_e.steps>0)
        {
            if(aParamOptim[CURRENTPARAM].para_e.enterSpeed >0){
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[PREVPARAM].para_e.koptim));
            }
            else {
                acch =  trunc(stepsChandSpeed(axis, aParamOptim[CURRENTPARAM].para_e.koptim));
            }
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
