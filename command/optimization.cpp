#include "optimization.h"

#include <QDebug>

#define cout    qDebug()<<__FILE__<<__LINE__

Optimization::Optimization()
{

}

void Optimization::calc(mito::Action_t *action, QQueue<sControlBlocks> &blocks)
{
    QList<sControlBlocks>::iterator bIterator;

    blocks.end();
    bIterator = blocks.end();
    cout<<"\t value:"<<blocks.size();
    while(bIterator != blocks.begin())
    {
        --bIterator;
        _controlBlocks *sc = &bIterator->bb;
//        sControlBlocks* sco = bIterator;
        cout<<bIterator->bb[0].alfa;
    }
}
