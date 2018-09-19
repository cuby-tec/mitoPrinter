#ifndef MITOACTION_H
#define MITOACTION_H

#include "links/ComDataReq_t.h"

#include <QQueue>

namespace mito {

struct Action_t{
    int a;
//    double b[20];
    QQueue<ComDataReq_t> queue;
};

} // namespace name

#endif // MITOACTION_H
