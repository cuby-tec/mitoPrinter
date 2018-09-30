#ifndef MITOACTION_H
#define MITOACTION_H

#include "links/ComDataReq_t.h"

#include <QQueue>

// Action_t.a - способ обработки этого действия.
enum eActionDo{
    eNext    // next string/command без допонительных действий; для команд типа set
    , eSend // отправить запрос или набор запросов из очереди.
    , eEOF // конец программы.файла
};

namespace mito {

struct Action_t{
    eActionDo a;
//    double b[20];
    QQueue<ComDataReq_t> queue;
};

} // namespace name

#endif // MITOACTION_H
