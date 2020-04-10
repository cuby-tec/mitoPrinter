#include "optimization.h"
#include <cstdio>
#include <QDebug>
#include <assert.h>
#include <math.h>
#include <LinearMath/Vector3.h>
#include <float.h>

//-------------- defs

#define CHANGELEVEL_VERSION 4//3//2

#define REPORT_LEVEL    4

#define cout    qDebug()<<__FILE__<<__LINE__
#define MIN(A,B) A<B?A:B

#define BJ  acs_change_x
#define BK  acs_change_y
//#define G4 bIterator->bb[X_AXIS].nominal_speed
#define block   bIterator
#define RAD_SPEED    opti_BI[counter].rad_speed
#define OPTI_K		opti_BI[counter].k

#define agI  agIterator->bb
#define bI   bIterator->bb


struct accDecSteps{
    int acc;   // accelerate steps in next segment.
    int flat;  // flat steps in current segment.
    int dec;   // decelerate steps in current segment.
};

//---------- function

Optimization::Optimization(Controller *control)
{
    coefficien = 1.0;
    //	step = 0.0;
    recalc = false;
    this->controller = control;
    opti_BI = nullptr;
    _block = nullptr;
}

Optimization::~Optimization()
{
    if(opti_BI != nullptr)
        free(opti_BI);
    cout<<"Exit optimization.";
}


enum stateTable{
    P17=(1+(1<<4))       // a.steps > 1 & b.steps > 1
    , P33 = (1+(2<<4))   // a.steps > 1 & b.steps == 1
    , P49=(1+(3<<4))     // a.steps > 1 & b.steps == 0
    , P18 = (2+(1<<4))   // a.steps == 1 & b.steps > 1
    , P34=(2+(2<<4))     // a.steps == 1 & b.steps == 1
    , P50=(2+(3<<4))     // a.steps == 1 & b.steps == 0
    , P19= (3+(1<<4))    // a.steps == 0 & b.steps > 1
    , P35=(3+(2<<4))     // a.steps == 0 & b.steps == 1
    , P51=((3+(3<<4)))   // a.steps == 0 & b.steps == 0
};



void Optimization::_calcLevel(accCompriseOf data[N_AXIS],size_t i, bool isNoMoveNext, bool isNoMoveCurrent)
{
    accCompriseOf* couple = &data[i];
//    if(data[i].a.steps == 0)
//        return;

    uint16_t state;	// Состояние автомата
//    stateTable state;

    uint16_t S1,S2;

    int a,b,c;

    if(couple->a.steps > 1)
        S1 = 1;
    else if(couple->a.steps == 1){
        S1 = 2;
    }
    else {
        S1 = 3;
    }

    if(couple->b.steps > 1){
        S2 = 1;
    }else if(couple->b.steps == 1){
        S2 = 2;
    }else{
        S2 = 3;
    }

    state = S1 + (S2<<4);
    double_t k ;// = scSteps/(scSteps+ncSteps);
    int32_t dec;
    switch (state)
    {
    case P17: // a.steps > 1 & b.steps > 1
    	if(i != E_AXIS){
            couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
            k = static_cast<double_t>(couple->a.steps)/static_cast<double_t>(couple->a.steps+couple->b.steps);
    		if(couple->change >= 0)
    			dec =  static_cast<int32_t>( ceil(couple->change * k ));
    		else
    			dec = static_cast<int32_t>(round(couple->change * k));

            couple->a.Dec = dec;
            couple->b.Acc = couple->change - dec;
//    		couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;

    	}else{
    		if(isNoMoveNext || isNoMoveCurrent){

                couple->change = -couple->a.nominalLevel;
    			dec = couple->change;
                couple->a.Dec = dec;
                couple->b.Acc = couple->b.nominalLevel;
//                couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;

    		}else{
    			// usual calculation
                couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
                k = static_cast<double_t>(couple->a.steps)/static_cast<double_t>(couple->a.steps+couple->b.steps);
        		if(couple->change >= 0)
        			dec =  static_cast<int32_t>( ceil(couple->change * k ));
        		else
        			dec = static_cast<int32_t>(round(couple->change * k));

                couple->a.Dec = dec;
                couple->b.Acc = couple->change - dec;
//        		couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;

    		}
    	}
        couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
        break;


    case P33: // a.steps > 1 & b.steps == 1
        couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
    	dec = couple->change;
        couple->a.Dec = dec;
        couple->b.Acc = 0;
        couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
        break;

    case P34: // a.steps == 1 & b.steps == 1
    	couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
    	dec = couple->change;
    	couple->a.Dec = dec;//0
    	couple->b.Acc = 0;
    	couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
    	break;

    case P49: // a.steps > 1 & b.steps == 0
        couple->change = -couple->a.nominalLevel;
    	dec = couple->change;
        couple->a.Dec = dec;
        couple->b.Acc = 0;
        couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
   	    break;

    case P18: // a.steps == 1 & b.steps > 1
        couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
    	dec = couple->change;
        couple->a.Dec = 0;
        couple->b.Acc = dec;
        couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
    	break;

    case P19: // a.steps == 0 & b.steps > 1  move starting in next segment
        couple->a.enterLevel = 0;
        couple->change = couple->b.nominalLevel - couple->a.nominalLevel;
        couple->b.Acc = couple->change;
        couple->b.enterLevel = couple->b.nominalLevel - couple->b.Acc;
    	break;


    case P50: // a.steps == 1 & b.steps == 0
    	couple->change = - couple->a.nominalLevel;
        couple->a.Dec = 0;


    	break;

    case P51: // a.steps == 0 & b.steps == 0
    	couple->change = 0;
        couple->a.Acc = 0;
        couple->a.Dec = 0;
        couple->b.enterLevel = 0;
    	break;

    }// switch

    //=========

    if(couple->a.steps >0 )
    {
    	//G4 = sqrt(2.0*block->alfa * block->acceleration * accpath);

    	volatile double_t G4;
//    	volatile uint32_t nominal_rate;
    	uint32_t rate;

        couple->a.finalLevel = couple->a.nominalLevel + couple->a.Dec;

    	_block->speedLevel = couple->a.nominalLevel;
        if(couple->a.nominalLevel != 0){
            G4 = sqrt(2.0 *_block->alfa * _block->acceleration * _block->speedLevel);// couple->a.Acc);
    		rate = static_cast<uint32_t> (controller->calcAxisRate(i,G4));
    	}else{
    		rate = controller->calcInitRate(i,_block->acceleration);
    	}

        if(rate > 16777214){ // 0xfffffe
        	rate = 0xfffffe;
        }

//        if(nominal_rate == _block->nominal_rate)
    	_block->nominal_rate = rate;

        if(couple->a.enterLevel == 0){
    		rate = controller->calcInitRate(i,_block->acceleration);
    	}else{
            G4 = sqrt(2.0 *_block->alfa * _block->acceleration * couple->a.enterLevel);
    		rate = controller->calcAxisRate(i,G4);
    	}
        //        if(initRate == _block->initial_rate)
        if(rate > 16777214){ // 0xfffffe
        	rate = 0xfffffe;
        }

    	_block->initial_rate = rate;


        _block->initial_speedLevel = static_cast<uint32_t>(couple->a.enterLevel);
        _block->accelerate_until = static_cast<uint32_t>(abs(couple->a.Acc));
        _block->decelerate_after = static_cast<u_int32_t>( couple->a.steps - abs(couple->a.Dec) );

        assert(_block->accelerate_until<=_block->decelerate_after);

        _block->final_speedLevel = static_cast<u_int32_t>(couple->a.finalLevel); //( couple->a.nominalLevel + couple->a.Dec );

        if(_block->final_speedLevel != 0)
        {
        	double_t dcc = sqrt(2.0 *_block->alfa * _block->acceleration * _block->final_speedLevel);
        	rate = controller->calcAxisRate(i,dcc);
        }else {
            rate = _block->initial_rate;
        }

        if(rate > 16777214){ // 0xfffffe
        	rate = 0xfffffe;
        }

        _block->final_rate = rate;

        //schem
        if(couple->a.Acc > 0){
        	_block->schem[0] = ACCELERATION;
        }else if(couple->a.Acc < 0){
        	_block->schem[0] = DECCELERATION;
        }else{
        	_block->schem[0] = FLATMOTION;
        }


        if(couple->dec < 0)
        	_block->schem[2] = DECCELERATION;
        else if(couple->dec > 0)
        	_block->schem[2] = ACCELERATION;
        else{
        	_block->schem[2] = FLATMOTION;
        }

        uint32_t speed_path = _block->decelerate_after - _block->accelerate_until;
        if(speed_path > 0)
        	_block->schem[1] = FLATMOTION;
        else
        	_block->schem[1] = _block->schem[2];




    }// end if(couple->a.steps >0 )
#if REPORT_LEVEL == 3
    cout<<"axis:"<<i
       <<"steps:"<<_block->steps
       <<"enter:"<<_block->initial_speedLevel
      <<"acc:"<<_block->accelerate_until
    <<"speedLevel:"<<_block->speedLevel
     <<"dec:"<<_block->decelerate_after
	 <<"fLevel:" << _block->final_speedLevel
	<<"iniRate:"<< _block->initial_rate
	<<"rate:" << _block->nominal_rate
	<<"fRate:" << _block->final_rate
          ;
#endif
}



void
Optimization::fillSegment(block_state_t* bs, sControl& sg)
{
    // fill segment
    sg.accelerate_until = bs->accelerate_until;
    sg.decelerate_after = bs->decelerate_after;
    sg.final_rate = bs->final_rate;
    sg.final_speedLevel = bs->final_speedLevel;
    sg.initial_rate = bs->initial_rate;
    sg.initial_speedLevel = bs->initial_speedLevel;
    sg.nominal_rate = bs->nominal_rate;
    sg.speedLevel = bs->speedLevel;
    sg.schem[0] = bs->schem[0];
    sg.schem[1] = bs->schem[1];
    sg.schem[2] = bs->schem[2];

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


    assert(action.queue.size() == blocks.size());

    QList<sControlBlocks>::iterator bIterator;// block iterator.
    QList<sControlBlocks>::iterator agIterator;

    volatile uint32_t size_sControl = sizeof (svSegment);

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.
    QList<ComDataReq_t>::iterator   nrIterator;// request iterator.

    bIterator = blocks.begin(); // current block

    agIterator = blocks.begin();
    agIterator++;                  // next block

    rIterator = action.queue.begin();   // destination.
    nrIterator = action.queue.begin();   // next destination.
    ++nrIterator;

    struct accCompriseOf acco[N_AXIS]; // vector acceleration/deceleration

    // set initial speed level
    for(size_t i=0;i<N_AXIS;++i){
        sControlBlocks sc = *bIterator;
        acco[i].a.steps = sc.bb[i].steps;
        if(acco[i].a.steps != 0){
            acco[i].a.enterLevel = static_cast<int>(sc.bb[i].initial_speedLevel);
            acco[i].a.nominalLevel = static_cast<int>(sc.bb[i].speedLevel);
            acco[i].a.finalLevel = static_cast<int>(sc.bb[i].final_speedLevel);
            acco[i].a.Acc = acco[i].a.nominalLevel - acco[i].a.enterLevel;
            acco[i].a.Dec = acco[i].a.nominalLevel-acco[i].a.finalLevel;
        }else {
            acco[i].a.enterLevel = 0;
            acco[i].a.nominalLevel = 0;
            acco[i].a.Acc = 0;
            acco[i].a.finalLevel = 0;
            acco[i].a.Dec = 0;

        }
    }

    bool isNoMoveNext;
    bool isNoMoveCurrent;

  ///////////////////////////////// Main circle analyze optimization //////////////////
    while(agIterator != blocks.end()){

        sControlBlocks sc = *bIterator;//  source data:current block
        sControlBlocks nc = *agIterator;    // next data: next block

        svSegment* segment = &rIterator->payload.instrument1_parameter;// destination data
        svSegment* nsegment = &nrIterator->payload.instrument1_parameter;// destination data
        uint32_t segnum = segment->head.linenumber;


#if REPORT_LEVEL == 4
        cout<<"segnum:" <<segnum;
#endif

        for(size_t i=0;i<N_AXIS;++i){
//             sControlBlocks sc = *bIterator;
             acco[i].b.steps = nc.bb[i].steps;
             if(acco[i].b.steps != 0){
                 acco[i].b.enterLevel = static_cast<int>(nc.bb[i].initial_speedLevel);
                 acco[i].b.nominalLevel = static_cast<int>(nc.bb[i].speedLevel);
                 acco[i].b.finalLevel = static_cast<int>(nc.bb[i].final_speedLevel);
                 acco[i].b.Acc = acco[i].b.nominalLevel - acco[i].b.enterLevel;
                 acco[i].b.Dec = acco[i].b.nominalLevel-acco[i].b.finalLevel;
             }else {
                 acco[i].b.enterLevel = 0;
                 acco[i].b.nominalLevel = 0;
                 acco[i].b.finalLevel = 0;
                 acco[i].b.Acc = 0;
                 acco[i].b.Dec = 0;

             }
         }

        // Если нет перемещения инструмента, то и материал не оптимизируется.
        isNoMoveNext = (acco[X_AXIS].b.steps == 0) && (acco[Y_AXIS].b.steps == 0)&& (acco[Z_AXIS].b.steps == 0);
        isNoMoveCurrent = (acco[X_AXIS].a.steps == 0) && (acco[Y_AXIS].a.steps == 0)&& (acco[Z_AXIS].a.steps == 0);

        //-------------- PART 2 end

        //For every axis calculate steps to change speed.
        // axis loop.
        for(uint32_t i=0;i<N_AXIS;i++){
        	_block = &sc.bb[i];

#if CHANGELEVEL_VERSION == 3
            if(acco[i].a.steps !=0){
                acco[i].a.nominalLevel = static_cast<int>(sc.bb[i].speedLevel);
                acco[i].a.finalLevel = static_cast<int>(sc.bb[i].final_speedLevel);
                acco[i].a.Acc = acco[i].a.nominalLevel - acco[i].a.enterLevel;
                acco[i].a.Dec = acco[i].a.finalLevel-acco[i].a.nominalLevel;
                acco[i].a.steps = static_cast<int> (sc.bb[i].steps);
            }

            if(acco[i].b.steps != 0){
                acco[i].b.nominalLevel = static_cast<int>(nc.bb[i].speedLevel);
                acco[i].b.Acc = acco[i].b.nominalLevel - acco[i].b.enterLevel;
                acco[i].b.finalLevel = static_cast<int>(nc.bb[i].final_speedLevel);
                acco[i].b.Dec = acco[i].b.finalLevel-acco[i].b.nominalLevel;
                acco[i].b.steps = static_cast<int> (nc.bb[i].steps);
            }

            _calcLevel(acco,i,isNoMoveNext, isNoMoveCurrent);

            // fill segment
            fillSegment(_block, segment->axis[i]);
#endif
#if CHANGELEVEL_VERSION == 4
            _calcLevel(acco,i,isNoMoveNext, isNoMoveCurrent);

            // fill segment
            fillSegment(_block, segment->axis[i]);

#endif

        }

        // next data block.
        for(size_t j=0;j<N_AXIS;j++)
        	memcpy(&acco[j].a,&acco[j].b,sizeof(segmentDescription));

        rowcounter++;// debug value
        ++agIterator;
        ++bIterator;
        ++rIterator;
        ++nrIterator;
    }// end while()

    // last row in queue
    bIterator = blocks.end();
    --bIterator;
    sControlBlocks sc = *bIterator ; //*agIterator;

    rIterator = action.queue.end();   // destination.
    --rIterator;
    svSegment* segment = &rIterator->payload.instrument1_parameter;// destination data

#if REPORT_LEVEL==0
    cout<<"axis:"<<i <<"  speedLevel:" << sc.bb[i].speedLevel
    		<<"\tsteps:"<<sc.bb[i].steps
			<< acco[i].change<<"["<<acco[i].acc<<":"<<"none]"
			//          << acco[i].change<<"["<<acco[i].dec<<":"<<acco[i].acc<<"]"
			<<" rate:"<< segment->axis[i].nominal_rate
			//         <<" level:"<<segment->axis[i].speedLevel
			<<"final:"<<finalRate
			<<"init:"<<sc.bb[i].initial_rate
			<<"row:"<<rowcounter;
#endif

    // load steps in axis  PART 2
    for(size_t i=0;i<N_AXIS;++i)
    {
        acco[i].b.steps =0;
        acco[i].b.enterLevel = 0;
        acco[i].b.nominalLevel = 0;
        acco[i].b.Acc = 0;
        acco[i].b.finalLevel = 0;
        acco[i].b.Dec = 0;
    }
    // Если нет перемещения инструмента, то и материал не оптимизируется.
    isNoMoveNext = (acco[X_AXIS].b.steps == 0) && (acco[Y_AXIS].b.steps == 0)&& (acco[Z_AXIS].b.steps == 0);
    isNoMoveCurrent = (acco[X_AXIS].a.steps == 0) && (acco[Y_AXIS].a.steps == 0)&& (acco[Z_AXIS].a.steps == 0);

    for(uint32_t i=0;i<N_AXIS;i++){
    	_block = &sc.bb[i];
        _calcLevel(acco,i,isNoMoveNext, isNoMoveCurrent);
        fillSegment(_block, segment->axis[i]);
    }
}// END OF smoot()


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
