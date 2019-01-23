#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "scontrolblocks.h"
#include "gparcer/mitoaction.h"

class Optimization
{
public:
    Optimization();

//QQueue<sControlBlocks> controlblocks;
    void calc(mito::Action_t &action, QQueue<sControlBlocks>& blocks);

};

#endif // OPTIMIZATION_H
