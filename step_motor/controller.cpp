/*
 * Controller.cpp
 *
 *  Created on: 09.07.2018
 *      Author: walery
 */

#include "controller.h"

#include <QScopedPointer>
#include <QtGlobal>
#include <QDebug>
#include <math.h>
#include <stdbool.h>

#include "profiles/profile.h"

#define cout qDebug()<<__FILE__<<__LINE__

#include <algorithm>

#define BUILDBLOCKVERSION   2

#define ACCELERATION    1
#define FLATMOTION      2
#define DECCELERATION   3
#define SCH(A,B,C) A|(B<<2)|(C<<4)

// "/-\"
#define SCHEMSTATE_1    SCH(ACCELERATION,FLATMOTION,DECCELERATION)
//"---"
#define SCHEMSTATE_2    SCH(FLATMOTION,FLATMOTION,FLATMOTION)
//"\_/"
#define SCHEMSTATE_3    SCH(DECCELERATION,FLATMOTION,ACCELERATION)





Controller::Controller()
{
    frequency = FREQUENCY;
    for(int i=0;i<N_AXIS;i++)
    {
        pcountertime[i] = nullptr;
    }

//    setupProfileData();
    Coordinatus* cord = Coordinatus::instance();
    profileData = cord->getProfileData();

    motor[X_AXIS] = new StepMotor(e17HS4401_pulley);

    motor[Y_AXIS] = new StepMotor(e17HS4401_pulley);

    motor[Z_AXIS] = new StepMotor(e17HS4401_shuft);

    motor[E_AXIS] = new StepMotor(e17HS4401_tooth_10_43);
#if BUILDBLOCKVERSION==2
    uploadMotorData();
#endif
}


Controller::~Controller()
{
        free(pcountertime);
}


void
Controller::buildCounterValue(uint32_t steps,uint8_t axis)
{
    if(pcountertime[axis]!=nullptr)
    {
        free(pcountertime[axis]);
        pcountertime[axis] = nullptr;
    }

//    pcountertime[axis] = (CounterTime_t*)(malloc(steps * sizeof(CounterTime_t)) );
    pcountertime[axis] = static_cast<CounterTime_t*>(malloc(steps * sizeof(CounterTime_t)) );

    Q_ASSERT(pcountertime[axis] !=nullptr);

    Q_ASSERT(motor[X_AXIS]->getAcceleration() != 0.0);

    for(uint32_t i=0;i<steps;i++)
    {
        if(i == 0)
        {
            //=SQRT(2*alfa/acceleration)*timer_frequency

        }
    }

}

double_t Controller::getPath_mm(uint8_t axis,int32_t steps) {
	double_t result;

	StepMotor* m = motor[axis];

	lines lm = m->getLineStep;

	result = ( m->*lm)(axis);

	result *= steps;
/*
        StepMotor* m = motor[i];
        lines lm = m->getLineStep;
//    	lines pstep = motor->getLineStep[i];
//        cord->nextBlocks[i].steps = fabs(path[i])/( motor->*pstep)(i);
        cord->nextBlocks[i].steps = fabs(path[i])/( m->*lm)(i);//TODO в сборку блока
*/
	return (result);
}

void Controller::buildCircleStep( Coordinatus* cord) {
    int32_t target_steps;
    block_state_t* blocks = cord->nextBlocks;
    for(uint32_t i=0;i<N_AXIS;++i){
        block_state_t* block = &blocks[i];
        memset(block,0,sizeof (block_state_t));
        StepMotor* m = motor[i];
        lines lm = m->getLineStep;
        double_t ds = ( m->*lm)(i);

        target_steps = static_cast<int32_t>(lround(cord->getNextValue(i)/ds));
        int32_t stp = target_steps-cord->position[i];


//        cord->nextBlocks[i].steps =  static_cast<uint32_t>( target_steps);
        block->steps = static_cast<uint32_t>( abs(stp));

        cord->position[i] = target_steps;
        if(stp > 0)
            block->direction_bits = edForward;
        else
            block->direction_bits = edBackward;

        if(block->steps > 0)
            block->axis_mask |= (1<<i);
        else
            continue;

        block->schem[0] = FLATMOTION;
        block->schem[1] = FLATMOTION;
        block->schem[2] = FLATMOTION;


        //radian_accel
        double_t racc = m->getAcceleration();
        //C0
        uint32_t cnt = static_cast<uint32_t>( frequency * sqrt(2.0 * m->getAlfa(i)/racc ) );

//        double_t G4 =   m->getAngular_velocity_rad_value();  //G4
//        uint32_t nominal_rate = static_cast<uint32_t>(frequency * m->getAlfa(i)/G4 );




        block->initial_rate =cnt;// nominal_rate;
        block->final_rate = cnt;//nominal_rate;
        block->nominal_rate = cnt;//nominal_rate;

        block->decelerate_after = block->steps + 1;

        block->microstep = 0;   //TODO Micro-step

    }

}

#define SET_ACCELERATION(A) motor[A]->setAcceleration(static_cast<double_t>(profileData->acceleration[A]))
#define SET_SPEEDRATE(A) motor[A]->setMaxSpeedrate( static_cast<double_t>(profileData->speedrate[A]));

void Controller::uploadMotorData() {
    SET_ACCELERATION(X_AXIS);
    SET_ACCELERATION(Y_AXIS);
    SET_ACCELERATION(Z_AXIS);
    SET_ACCELERATION(E_AXIS);

    SET_SPEEDRATE(X_AXIS);
    SET_SPEEDRATE(Y_AXIS);
    SET_SPEEDRATE(Z_AXIS);
    SET_SPEEDRATE(E_AXIS);

}

#define TARGETVERSION 1

#define MIN(A,B) A<B?A:B
/**
 * Заполнение полей разгона, торможения, и т.д.
 */
#if BUILDBLOCKVERSION == 2
bool
Controller::buildBlock(Coordinatus* cord) {
//    double_t path[N_AXIS];						//	B2
    int32_t target_steps[N_AXIS];
    double_t k;

    block_state_t* blocks = cord->nextBlocks;

    //[4] Длина линии в шагах		C23
    for(uint32_t i=0;i<N_AXIS;++i){
        block_state_t* block = &blocks[i];
        StepMotor* m = motor[i];
        lines lm = m->getLineStep;
        double_t ds = ( m->*lm)(i);

        target_steps[i] = static_cast<int32_t>(lround(cord->getNextValue(i)/ds));
        int32_t stp = target_steps[i]-cord->position[i];
        maxvector[i] = static_cast<uint32_t>(abs(stp));
//        cord->nextBlocks[i].steps = maxvector[i];
        block->steps = maxvector[i];
        cord->position[i] = target_steps[i];
        if(stp > 0)
            block->direction_bits = edForward;
        else
            block->direction_bits = edBackward;
    }

    // Наибольшая длина линии						C26
    uint32_t maxLenLine = *std::max_element(maxvector,maxvector+N_AXIS);
    if(maxLenLine == 0)
        return (false);
    Q_ASSERT(maxLenLine > 0);

    double_t angular_velocity = selectFeedrate(cord->getSpeedrate());

//    uint32_t accel_steps[N_AXIS];
    for(uint32_t i=0;i<N_AXIS;i++){

        block_state_t* block = &blocks[i];

        //=radian_speed*(D4)
        k = static_cast<double_t>( maxvector[i]) / static_cast<double_t>(maxLenLine);
//        double_t G4 =  ( k* motor[i]->getAngular_velocity_rad_value());  //G4 alternative
//        StepMotor* m = motor[i];
//        angularSpeedrate as = m->getAngularSpeedrate;
//        double_t G4 = k* (m->*as)(cord->getSpeedrate());
        double_t G4 =  ( k* angular_velocity);//TODO
#if LEVEL==1
        cout<<"G4:"<<G4;
#endif
        //radian_accel
        double_t racc = k * motor[i]->getAcceleration();

        //radian_deccel
        double_t rdcc = k * motor[i]->getDecceleration();

        //accel steps: max _ s _ lim
        double_t acs = pow(G4,2.0)/(2.0*motor[i]->getAlfa(i)*racc);

        //accel_lim
        double_t acl = maxvector[i] * racc/(racc + rdcc);

        //accel_path
        uint32_t accpath =  static_cast<uint32_t>( MIN(acs,acl) );

        //deccel_path
        uint32_t dccpath =  static_cast<uint32_t>( MIN(acs,acl) * racc/rdcc );

        //speed_path
        uint32_t speed_path = maxvector[i] - (accpath + dccpath);

        //schem
        if(accpath == 0){
            block->schem[0]=FLATMOTION;  //2 no acceleration
        }else{
            block->schem[0]=ACCELERATION;  //1 acceleration
        }

        if(speed_path == 0)
            block->schem[1] = DECCELERATION;	//3 decceleration
        else{
            block->schem[1] = FLATMOTION;	//2 равномерно
        }


        if(dccpath == 0){
            block->schem[2] = FLATMOTION; //2 no decceleration
        }else{
            block->schem[2] = DECCELERATION; //3 decceleration
        }

        uint32_t schemState = 0;
        schemState |= block->schem[0];
        schemState |=  static_cast<uint32_t>(block->schem[1])<<2;
        schemState |= static_cast<uint32_t>(block->schem[2])<<4;

        //C0
        // double_t cnt = sqrt(2*motor[i]->getAlfa(i)/accel[i])*frequency;
        uint32_t cnt = static_cast<uint32_t>( frequency * sqrt(2.0 * motor[i]->getAlfa(i)/racc ) );
        // nominal_rate
        uint32_t nominal_rate = static_cast<uint32_t>(frequency * motor[i]->getAlfa(i)/G4 );

        if(nominal_rate > 16777214){ // 0xfffffe
            nominal_rate = 0xfffffe;
            cout<<"NOMINAL RATE OUT OF RANGE, ASYNCRONOUS:"<<nominal_rate;
            qWarning("NOMINAL RATE OUT OF RANGE:%d",nominal_rate);
        }

        switch (schemState) {
        case SCHEMSTATE_1:
            block->initial_rate = cnt;
            block->final_rate = cnt;
            block->nominal_rate = nominal_rate;

            block->decelerate_after = block->steps - dccpath;

            break;

        case SCHEMSTATE_2:
            block->initial_rate = nominal_rate;
            block->final_rate = nominal_rate;
            block->nominal_rate = nominal_rate;

            block->decelerate_after = block->steps + 1;

            break;

        case SCHEMSTATE_3:
//TODO acceleration scheme
            break;

        }


//        block->initial_rate = cnt;
//        block->final_rate = cnt;

//        block->steps = maxvector[i];
        block->accelerate_until = accpath;
//        block->decelerate_after = block->steps - dccpath;

        block->microstep = 0;   //TODO Micro-step
        block->axis_mask = 0;
        if(block->steps > 0)
            block->axis_mask |= (1<<i);

        block->speedLevel = accpath;
    }

    return (true);
}// end buildBlock

#endif

#if BUILDBLOCKVERSION == 1
void Controller::buildBlock(Coordinatus* cord) {

	//  Путь по X
	//	Путь по Y
	//	Путь по Z
    double_t path[N_AXIS];						//	B2
    int32_t target_steps[N_AXIS];


	uploadMotorData();

    for(size_t i=0;i<N_AXIS;i++){
		path[i] = cord->getNextValue(i) - cord->getCurrentValue(i);
//		qDebug()<<"Controller[74]"<<" path:"<< path[i];
	}
//qDebug()<<"Controller[122] pathX:"<<path[X_AXIS]<<"\tY:"<<path[Y_AXIS];

// Длина вектора.
  double_t s = 0;

    for(size_t i=0; i<M_AXIS;i++){
        s += pow( cord->getCurrentValue(i)-cord->getNextValue(i),2);
	}
	s = sqrt(s);

// Максимальная скорость по осям(линейная)
    double_t v[N_AXIS];

    for(int i=0;i<N_AXIS;++i){
//		v[i] = motor->linespeed(profileData.speed_rpm[i]);
//        v[i] = (motor->*m_struct[i])(profileData.speed_rpm[i]);

//        convert pf = motor->getLineSpeed[i];
//        v[i] = fabs((motor->*pf)(profileData.speed_rpm[i]));
        StepMotor* m = motor[i];
        convert pm = m->getLineSpeed;
        v[i] = fabs((m->*pm)(static_cast<double_t>(profileData->speed_rpm[i])));


//varant 2:     v[i] = (motor->*motor->m_struct[i])(profileData.speed_rpm[i]);

		//	v = cord->nextBlocks[X_AXIS].nominal_speed; // Число оборотов/мин
	}

    //Максимальная скорость для сегмента
    double_t maxs = *std::max_element(v,v+M_AXIS);	//E13

    //[3] Синусы направлений					C18
    double_t sins[M_AXIS];
    for(int i=0;i<M_AXIS;i++){
        sins[i] = fabs(path[i]/s);
    }

    //[4] Длина линии в шагах						C23
//    uint32_t lenline[M_AXIS];
    for(uint32_t i=0;i<N_AXIS;++i){

        StepMotor* m = motor[i];
        lines lm = m->getLineStep;
//    	lines pstep = motor->getLineStep[i];
//        cord->nextBlocks[i].steps = fabs(path[i])/( motor->*pstep)(i);
        double_t ds = ( m->*lm)(i);
        double_t dstep = fabs(path[i]/ds);
        dstep = round(dstep*pow(10,10))/pow(10,10);
//        cord->nextBlocks[i].steps = fabs(path[i])/( m->*lm)(i);//TODO в сборку блока
#if TARGETVERSION==1
        // Version2
        target_steps[i] = static_cast<int32_t>(lround(cord->getNextValue(i)/ds));
        cord->nextBlocks[i].steps = static_cast<uint32_t>(labs(target_steps[i]-cord->position[i]));
        cord->position[i] = target_steps[i];
#else

        cord->nextBlocks[i].steps = dstep;
#endif
//    	lenline[i] = cord->nextBlocks[i].steps;
//    	double_t ps = (motor->*pstep)(i);
//        double_t ps = (m->*lm)(i);

//        trapeze[i].length = abs(path[i])/( motor->*pstep)(i);
//    	double_t pa = path[i];
//    	if(pa<0){
//            pa = fabs(path[i]);
//    	}
//    	trapeze[i].length = pa/ps;
        trapeze[i].length = (dstep<0?fabs(dstep):dstep);
    }

    // Наибольшая длина линии						C26
//    uint32_t maxLenLine = *std::max_element(lenline,lenline+M_AXIS);

    //[5] Максимальное Угловое ускорение			C28
    for(size_t i=0;i<N_AXIS;++i){
        cord->nextBlocks[i].acceleration = static_cast<double_t>(profileData->acceleration[i]);// TODO в сборку блокаS
    }


    //[4] velocity for every axis, line		313.16 47.7 	C54
    double_t velocity[M_AXIS];
    for(int i=0;i<N_AXIS;i++){
    	velocity[i] = maxs * sins[i];
    }


    //[5] Угловая скорость							C59
    double_t tSpeed[M_AXIS];
    for(int i=0;i<M_AXIS;i++){
        tSpeed[i] = (velocity[i]/motor[i]->getPulleyDiameter())*2;
//    	qDebug()<<"Controller[142] "<< "speed:"<<tSpeed[i];
    }

    //[6] Определяющая координата					C33
/*
    bool isLongest[M_AXIS];
    for(int i=0;i<M_AXIS;i++){
    	isLongest[i] = (lenline[i] == maxLenLine);
    }
*/

    //[7] Число шагов разгона	34.11 0.79			C38
    double_t accelSteps[M_AXIS];
    for(uint32_t i=0;i<M_AXIS;i++){
        accelSteps[i] = pow(tSpeed[i],2)/( 2 * motor[i]->getAlfa(i) *  static_cast<double_t>(profileData->acceleration[i]) );
        trapeze[i].accPath = pow(tSpeed[i],2)/( 2 * motor[i]->getAlfa(i) * static_cast<double_t>(profileData->acceleration[i]) );
    }

    //Максимальное число шагов разгона				D41
//    double_t maxAccelSteps = *std::max_element(accelSteps,accelSteps+M_AXIS);

/*
    //[8] Прверка трапеции
    bool isTrapecia[M_AXIS];
    for(int i=0;i<M_AXIS;i++){
    	isTrapecia[i] = maxAccelSteps*2 < cord->nextBlocks[i].steps;
    }
*/

    // [9] Число шагов разгона пересчёт				C49

    // Построение трапеций для осей
    uint32_t index = calculateTrapeze();

    // motor[X_AXIS]
    double_t maxLineAccel = motor[index]->getLinearAcceleration();
Q_ASSERT(maxLineAccel != 0.0);
    // Время разгона для оси X
    double_t minimeAccelerationTime = velocity[index]/maxLineAccel;

#define ACCELERTION_BY_TIME
// 0,0002%

    //[12] Угловое ускорения для осей					C64
    double_t accel[M_AXIS];
    for(uint32_t i=0;i<M_AXIS;i++){
        if(trapeze[i].accPath!=0.0 && trapeze[i].length!=0.0){
#ifdef ACCELERTION_BY_TIME
            accel[i] = (2 * motor[i]->getAlfa(i)*(trapeze[i].accPath))/pow(minimeAccelerationTime,2); //  trapeze[i].accPath
#else
            accel[i] = pow(tSpeed[i],2)/(2 * motor[i]->getAlfa(i)* fabs(trapeze[i].accPath)); //  trapeze[i].accPath
#endif

        }else{

    		accel[i] = 0;

        }
//    	qDebug()<<"Controller[182"<< "  accel:"<<accel[i]<< "\taccPath:"<<trapeze[i].accPath;
    }

    //[13] Начальный счётчик
    uint32_t start_counter[M_AXIS];
    for(uint32_t i=0;i<M_AXIS;i++){
        if(accel[i]!=0.0){
            double_t cnt = sqrt(2*motor[i]->getAlfa(i)/accel[i])*frequency;
//    	    qDebug() << "buildBlock[180]"<< "  axis:"<< i << " counter:"<<start_counter[i]<< "acc:"<< trapeze[i].accPath;

            //MAX_COUNTER_VALUE
            if(cnt>= MAX_COUNTER_VALUE)
            	start_counter[i] = MAX_COUNTER_VALUE;
            else
                start_counter[i] = static_cast<uint32_t>(cnt);

    	}
    	else
    		start_counter[i] = 0;
    }

    //[14] Счётчик номинальной скорости		C90
    uint32_t norm_counter[M_AXIS];
    for(uint32_t i=0;i<M_AXIS;i++){
        if(tSpeed[i] != 0.0)
            norm_counter[i] = static_cast<uint32_t>(frequency * motor[i]->getAlfa(i)/fabs(tSpeed[i]));
    	else
    		norm_counter[i] = 0;
    }


    // calculate acceleration Построение таблицы шагов разгона.
    AccelerationTable* tableX = new AccelerationTable(static_cast<uint32_t>(trapeze[X_AXIS].accPath) + 1);
    tableX->buildTale(start_counter[X_AXIS]);
//    double_t accTime = tableX->getAccelerationTime(FREQUENCY);

/*
    	qDebug()<<" buildBlock[192] =========";
    for(int i=0;i<trapeze[X_AXIS].accPath;i++)
    {
    	qDebug()<<" step:"<<i <<"\tcount:"<< tableX->getCounter(i);
    }
*/
    AccelerationTable* accY = new AccelerationTable(static_cast<uint32_t>(trapeze[Y_AXIS].accPath)+1);
    accY->buildTale(start_counter[Y_AXIS]);

/*
    qDebug()<< "buildBlock[192]";
    qDebug()<< " acc:"<< trapeze[X_AXIS].accPath << " \t path:"<< trapeze[X_AXIS].length
    		<<"\tstart:"<<start_counter[X_AXIS]
    		<<"\tspeed:"<<tableX->getCounter(trapeze[X_AXIS].accPath)
			<<"\tflat:"<<norm_counter[X_AXIS];

    qDebug()<< " acc:"<< trapeze[Y_AXIS].accPath << " \t path:"<< trapeze[Y_AXIS].length
    		<<"\tstart:"<<start_counter[Y_AXIS]
    		<< "\tspeed:"<< accY->getCounter(trapeze[Y_AXIS].accPath)
			<<"\tflat:"<<norm_counter[Y_AXIS];

    qDebug()<< " acc:"<< trapeze[Z_AXIS].accPath << " \t path:"<< trapeze[Z_AXIS].length
    		<<"\t\tflat:"<<norm_counter[Z_AXIS];
*/

    // Сборка============>>>>>>>>>>

    block_state_t* blocks = cord->nextBlocks;
//    block->steps
//    block->accelerate_until
//    block->decelerate_after
//    block->initial_rate
//    block->nominal_rate
//    block->final_rate
//    block->axis_mask;
//    block->direction_bits
//    block->initial_speedLevel
//    block->final_speedLevel
//    block->schem

//TODO build block

    for(int i=0;i<M_AXIS;++i){
    	block_state_t* block = &blocks[i];
        block->steps = static_cast<word>(trapeze[i].length);
        block->speedLevel = static_cast<word>(trapeze[i].accPath);
        block->accelerate_until = static_cast<word>(trapeze[i].accPath);
        block->decelerate_after = block->steps - static_cast<word>(trapeze[i].accPath);
    	block->initial_rate = start_counter[i];
    	block->nominal_rate = norm_counter[i];
    	block->final_rate = start_counter[i];
    	block->entry_speed = 0;
    	block->nominal_speed = tSpeed[i];
    	block->path = path[i];
        block->schem[0] = 1;	// Разгон
        block->schem[1] = 2;	// равномерно
        block->schem[2] = 3;	// торможение
    	block->microstep = 0; //TODO Micro-step
        block->axis_mask = 0;
        if(block->steps > 0)
    		block->axis_mask |= (1<<i);

    }

    splinePath(cord->currentBlocks,blocks);
    if(cord->nextBlocks[E_AXIS].steps > 0)
        calculateExtruder(cord->nextBlocks);//TODOH


} //
#endif

/**
 * Выравнивание скоростей в смежных сегментах.
 * Для одноразовой команды это не требуется.
 */
void Controller::splinePath(block_state_t* privBlock, block_state_t* currBlock) {
    double_t dif;

	 Recalculate_flag* flag;
	 flag = (Recalculate_flag*)&currBlock[X_AXIS].recalculate_flag;

	 if(flag->single == true)
		 return;

	dif = currBlock[X_AXIS].nominal_speed - privBlock[X_AXIS].nominal_speed;
	//TODO set scheme
}

/*                             STEPPER RATE DEFINITION
		                                     +--------+   <- nominal_rate
		                                    /          \
		    nominal_rate*entry_factor ->   +            \
		                                   |             + <- nominal_rate*exit_factor
		                                   +-------------+
		                                       time -->


	  	                              PLANNER SPEED DEFINITION
		                                     +--------+   <- current->nominal_speed
		                                    /          \
		         current->entry_speed ->   +            \
		                                   |             + <- next->entry_speed
		                                   +-------------+
		                                       time -->
 */
//#define CO		psettings->initial_rate/0.676
void
Controller::planner_recalculate(block_state* prev, block_state* curr)
{
	int16_t dlevel,d2;
	word meanlevel;
	word tmp_rate;

	double_t CO = 1.0;//TODO temporary value for debug.

	if(prev->speedLevel == curr->speedLevel) return;

    meanlevel = static_cast<word>((static_cast<int64_t>(prev->nominal_rate) + curr->nominal_rate)/2);

	if(prev->speedLevel>curr->speedLevel){
		// Снижение скорости
        dlevel = static_cast<int16_t>(prev->speedLevel - curr->speedLevel);
		d2 = dlevel/2;
		dlevel -= d2;
		prev->decelerate_after = prev->steps - d2;
		curr->accelerate_until = dlevel;	// todo if dlevel/2 > accelerate_until
		prev->schem[2] = 3;
		curr->schem[0] = 4;

		d2 = prev->speedLevel-d2;
		if(d2){
			prev->final_speedLevel = d2;
			tmp_rate = (word)(CO)*(sqrtf(d2+1)-sqrtf(d2));
			prev->final_rate = (tmp_rate>meanlevel)?(tmp_rate):(meanlevel);
		}
		dlevel += curr->speedLevel;
		if(dlevel){
			curr->initial_speedLevel = dlevel;
			tmp_rate = (word)CO*(sqrtf(dlevel+1)-sqrtf(dlevel));
			curr->initial_rate = (tmp_rate>meanlevel)?(tmp_rate):(meanlevel);

		}
	}else{
		// Увеличение скорости
		dlevel = curr->speedLevel - prev->speedLevel;
		word rest = prev->steps - prev->decelerate_after;
		if(rest>dlevel){
			d2 = dlevel/2;
		}else{
			d2 = rest;
		}
		dlevel -= d2;

		prev->decelerate_after = prev->steps - d2;
		curr->accelerate_until = dlevel;	// todo if dlevel/2 > accelerate_until
		prev->schem[2] = 6;
		curr->schem[0] = 1;
		d2 +=prev->speedLevel;
		if(d2){
			prev->final_speedLevel = d2;
			prev->final_rate = (word)CO*(sqrtf(d2+1)-sqrtf(d2));
		}
		dlevel = curr->speedLevel - dlevel;
		if(dlevel){
			curr->initial_rate = (word)CO*(sqrtf(dlevel+1)-sqrtf(dlevel));
			curr->initial_speedLevel = dlevel;
		}
	}
	//------------------------- check state go

}

void Controller::calculateExtruder(block_state_t *blocks)
{
    //TODOH
    block_state_t* eblock = &blocks[E_AXIS];
    qDebug()<<__FILE__<<__LINE__ <<"Extruder steps:" <<eblock->steps;

    // Исходные данные угловая скорость(рад/сек), угловое ускорение(рад/сек^2), путь в (мм)

}




#define TRAPEZE_V   1
// Длина участка принимается как целое число
// Расчёт трапеций для каждой оси.
/**
 * @brief Controller::calculateTrapeze
 * @return Индекс приоритетной оси.
 */
uint32_t
Controller::calculateTrapeze() {
	double_t lenline[M_AXIS];
	size_t index; 			//Главная трапеция.
	double_t coeffic;

	for(int i=0;i<M_AXIS;i++)
		lenline[i] = trapeze[i].length;

    // Наибольшая длина линии						C26
    double_t* pmaxLenLine = std::max_element(lenline,lenline+M_AXIS);

//    index = (pmaxLenLine - lenline)/sizeof(double_t);
    index = (pmaxLenLine - lenline);

    //Построение трапеции
#if TRAPEZE_V==1
    coeffic = trapeze[index].accPath/ floor(trapeze[index].length) ;
#else
    coeffic = trapeze[index].accPath/trapeze[index].length;
#endif
    // Есть трапеция Pass#1
    for(uint i=0;i<M_AXIS;++i)
    {
        if(i!=index){
#if TRAPEZE_V==1
            trapeze[i].accPath = floor(trapeze[i].length) * coeffic;
#else
            trapeze[i].accPath = trapeze[i].length * coeffic;
#endif
        }
    	// Pass#2
        if(trapeze[i].accPath*2>trapeze[i].length){
#if TRAPEZE_V==1
            trapeze[i].accPath = floor( trapeze[i].length )/2;
#else
            trapeze[i].accPath = trapeze[i].length/2;
#endif

        }
    }

    return index;
}


//TODOH Precicion
double_t Controller::getPrecicion(uint8_t axis, uint8_t microstep) {

	double_t result;

	motor[axis]->setMicrostep(axis,microstep);
    StepMotor* m = motor[axis];
    lines lm = m->getLineStep;
    result = (m->*lm)(axis);	//( m->*lm)(i)
    return result;
}

double_t
Controller::getPrecicion(uint8_t axis)
{
	double_t result;
//	motor[axis]->setMicrostep(axis,microstep);
    StepMotor* m = motor[axis];
    lines lm = m->getLineStep;
    result = (m->*lm)(axis);	//( m->*lm)(i)
    return result;
}


void Controller::uploadPosition(Coordinatus* cord)
{
    for(uint32_t i=0;i<N_AXIS;++i){
        StepMotor* m = motor[i];
        lines lm = m->getLineStep;
        double_t ds = ( m->*lm)(i);
        cord->position[i] =static_cast<int32_t>(lround(cord->getNextValue(i)/ds));
    }
}
#define SELECTOR    1
double_t Controller::selectFeedrate( double_t cord_feedrate ) {
	//TODOH
//	bool path[N_AXIS];

	double_t maxfeedVector[N_AXIS];
    double_t maxvalue = 10000000.0;
    size_t index = 0;

    maxfeedVector[X_AXIS] = motor[X_AXIS]->getMaxSpeedrate();//FEEDRATE_PULLEY_MAX;
    maxfeedVector[Y_AXIS] = motor[Y_AXIS]->getMaxSpeedrate();//FEEDRATE_PULLEY_MAX;
    maxfeedVector[Z_AXIS] = motor[Z_AXIS]->getMaxSpeedrate();//FEEDRATE_SHAFT_MAX;
    maxfeedVector[E_AXIS] = motor[E_AXIS]->getMaxSpeedrate();//FEEDRATE_GEAR_MAX;

	for(size_t i=0;i<N_AXIS;i++){
        if(maxvector[i]==0){
			maxfeedVector[i] = 0.0;
        }else
#if SELECTOR==2
            if(maxvalue > maxfeedVector[i]){
                maxvalue = maxfeedVector[i];
                index = i;
            }
#else
        {
            StepMotor* m = motor[i];
        	angularSpeedrate as = m->getAngularSpeedrate;
        	maxfeedVector[i] = (m->*as)(maxfeedVector[i]);
            if(maxvalue > maxfeedVector[i]){
                maxvalue = maxfeedVector[i];
                index = i;
            }

        }

#endif

    }

#if SELECTOR==2
    if(maxvalue>cord_feedrate && cord_feedrate!=0.0)
		maxvalue = cord_feedrate;


	StepMotor* m = motor[index];
	angularSpeedrate as = m->getAngularSpeedrate;
	double_t G4 = (m->*as)(maxvalue);
#else
	double_t G4;
	StepMotor* m = motor[index];
	angularSpeedrate as = m->getAngularSpeedrate;
	double_t angular_cord =  (m->*as)(cord_feedrate);
    if((maxvalue > angular_cord) &&(cord_feedrate!=0.0))
		G4 = angular_cord;
	else
		G4 = maxfeedVector[index];
#endif
	return G4;
}//==

#ifdef REMOVED
/**
 * Загрузка данных из профиля.
 */
void Controller::setupProfileData() {
	Profile* profile = Profile::instance();
	if(!profile->isProfileActive())
		profile->init_profile();

	//setSpeedLevel(block, psettings->seekSpeed);
	//    float seekspeed;
	    bool ok;

	    QString fspeed; // rpm unit
	    float speed;

	    QString facceleration;
	    float acceleration;


	    for(int i=0;i<N_AXIS;i++)
	    {
	//        block_state* block = &blocks[i];
//	        block_state* block = &cord->nextBlocks[i];

	        switch(i)
	        {
	        case X_AXIS:
	            fspeed = MyGlobal::formatFloat(profile->getX_MAX_RATE());
	            facceleration = MyGlobal::formatFloat(profile->getX_ACCELERATION());
	            break;

	        case Y_AXIS:
	            fspeed = MyGlobal::formatFloat(profile->getY_MAX_RATE());
	            facceleration = MyGlobal::formatFloat(profile->getY_ACCELERATION());
	            break;

	        case Z_AXIS:
	            fspeed = MyGlobal::formatFloat(profile->getZ_MAX_RATE());
	            facceleration = MyGlobal::formatFloat(profile->getZ_ACCELERATION());
	            break;

	        case E_AXIS:
	            fspeed = MyGlobal::formatFloat(profile->getE_MAX_RATE());
	            facceleration = MyGlobal::formatFloat(profile->getE_ACCELERATION());
	            break;
	        }

	        speed = fspeed.toFloat(&ok);
	        Q_ASSERT(ok);
//	        block->nominal_speed = speed;
	        profileData.speed_rpm[i] = speed;

	        acceleration = facceleration.toFloat(&ok);
	        Q_ASSERT(ok);
//	        block->acceleration = acceleration;
	        profileData.acceleration[i] = acceleration;

	    }
}
#endif



//============ EOF

