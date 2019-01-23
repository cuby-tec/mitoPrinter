#include "optimization.h"

#include <QDebug>

#define cout    qDebug()<<__FILE__<<__LINE__

Optimization::Optimization()
{

}

void Optimization::calc(mito::Action_t &action, QQueue<sControlBlocks> &blocks)
{
    QList<sControlBlocks>::iterator bIterator;// block iterator.

    QList<ComDataReq_t>::iterator   rIterator;// request iterator.

    QList<sControlBlocks>::iterator agIterator;

//    blocks.end();

    bIterator = blocks.end();
    agIterator = blocks.end();
    --agIterator;

    rIterator = action.queue.end();


    cout<<"\t value:"<<blocks.size()<<"\trequest:"<<action.queue.size();


    while(bIterator != blocks.begin()+1)
    {
        --bIterator;
        --rIterator;
        --agIterator;

        _controlBlocks *sc = &bIterator->bb;

        sSegment* segment = &rIterator->payload.instrument1_parameter;

//        sControlBlocks* sco = bIterator;
        cout<<bIterator->bb[0].steps<<"\tAG:"<<agIterator->bb[0].steps<<"\tsement:"<<segment->head.linenumber;

//        ++bIterator;
//        ++rIterator;


    }
}
