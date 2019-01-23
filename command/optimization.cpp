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

    blocks.end();
    bIterator = blocks.end();
    rIterator = action.queue.end();


    cout<<"\t value:"<<blocks.size()<<"\trequest:"<<action.queue.size();


    while(bIterator != blocks.begin())
    {
        --bIterator;
        --rIterator;

        _controlBlocks *sc = &bIterator->bb;

        sSegment* segment = &rIterator->payload.instrument1_parameter;

//        sControlBlocks* sco = bIterator;
        cout<<bIterator->bb[0].alfa<<"\tsement:"<<segment->head.linenumber;

//        ++bIterator;
//        ++rIterator;


    }
}
