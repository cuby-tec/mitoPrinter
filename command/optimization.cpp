#include "optimization.h"

#include <QDebug>
#include <assert.h>
#include <math.h>

#define cout    qDebug()<<__FILE__<<__LINE__
#define MIN(A,B) A<B?A:B

Optimization::Optimization()
{

}

void Optimization::calc(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{
    QList<sControlBlocks>::iterator bIterator;// block iterator.

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.

    QList<sControlBlocks>::iterator agIterator;

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

//START
    bIterator = blocks.end();
    agIterator = blocks.end();
    --agIterator;
    rIterator = action.queue.end();
    cout<<"Start compare ";

    cout<<"\nsteps\tacc_path\tspeed_path\tdecel_path\tBJx";

    while(bIterator != blocks.begin()){
        --bIterator;
        if(agIterator != blocks.begin())
            --agIterator;
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
