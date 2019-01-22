#ifndef SCONTROLBLOCKS_H
#define SCONTROLBLOCKS_H
#include "step_motor/block_state_t.h"
#include "links/msmotor/msport.h"

typedef block_state_t _controlBlocks[N_AXIS];
struct sControlBlocks{
//    block_state_t x_block;
//    block_state_t y_block;
//    block_state_t z_block;
//    block_state_t e_block;
    _controlBlocks bb;
};

#endif // SCONTROLBLOCKS_H
