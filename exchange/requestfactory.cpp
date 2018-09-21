#include "requestfactory.h"

#include <QDebug>


#define cout qDebug()<<__FILE__<<__LINE__

RequestFactory::RequestFactory()
{

}

void RequestFactory::build(ComDataReq_t *comdata, eOrder order)
{
    /* UsbExchange::buildComData(ComDataReq_t *comdata, eOrder order)
     * eoState,            // Запрос состояния устройства.
     * eoHotendParams,    // Задание параметров Hotend
     *  eoSegment
     * */
    switch (order) {
    case eoState:
        //TODO request Status only
        comdata->size = sizeof(struct ComDataReq_t);
//        comdata->requestNumber = ++MyGlobal::requestIndex;
        comdata->command.order = eoState;
//        sendRequest(comdata);
        break;

    case eoProfile:
/*        buildProfile(&comdata->payload.profile);
        comdata->size = sizeof(struct ComDataReq_t);
//        comdata->requestNumber = ++MyGlobal::requestIndex;
        comdata->instruments = N_AXIS;

        comdata->command.order = eoProfile;

        sendRequest(comdata); // Get request in Controller. */
        break;

    case eoSegment:
//        buildComData(comdata);
        break;
    }

}

ComDataReq_t*
RequestFactory::build(uint linenumber)
{

    Coordinatus *coordinatus = Coordinatus::instance();

    //    ComDataReq_t* req = &request;
        ComDataReq_t* req = new ComDataReq_t;
        sSegment* segment;
        sControl* control;
        block_state_t* bstates = coordinatus->nextBlocks;

        memset(req,0,sizeof(ComDataReq_t));

        req->instruments = 1;   // TODO
        req->command.order = eoSegment;
        req->command.instrument = 1;
        req->command.reserved = 0;

        segment = &req->payload.instrument1_parameter;

        //------------ payload =============================
        segment->head.linenumber = linenumber;//sgCode->line ;
        segment->head.axis_number = M_AXIS;//0;

        //if(segment->head.reserved == EXIT_CONTINUE)
        //    ms_finBlock = continueBlock;
        //else
        //    ms_finBlock = exitBlock;
        segment->head.reserved &= ~EXIT_CONTINUE;

        for(int i=0;i<M_AXIS;i++)
        {
            if(bstates[i].steps>0)
                segment->head.axis_mask |= (1<<i);
            else
                segment->head.axis_mask &= ~(1<<i);
        }
        //======== sControl =========

        for(int i =0;i<M_AXIS;i++){
            control = &segment->axis[i];
            block_state_t* bstate = &bstates[i];

            control->accelerate_until = bstate->accelerate_until;
            control->decelerate_after = bstate->decelerate_after;

            if(bstate->path>0)
                control->direction = forward;
            else
                control->direction = backward;

            control->final_rate = bstate->final_rate;
            control->initial_rate = bstate->initial_rate;
            control->nominal_rate = bstate->nominal_rate;
            control->final_speedLevel = bstate->final_speedLevel;

            control->speedLevel = bstate->accelerate_until;// TODO Attention

            control->microsteps = bstate->microstep;

            for(int j=0;j<3;j++){
                control->schem[j] = bstate->schem[j];
            }

            control->steps = bstate->steps;
            control->axis = bstate->axis_mask;

        }
        cout<<segment->axis[X_AXIS].steps<<"\t"<<segment->axis[Y_AXIS].steps<<"\tline"<<segment->head.linenumber;

        return req;
}
