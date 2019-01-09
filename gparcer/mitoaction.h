#ifndef MITOACTION_H
#define MITOACTION_H

#include "links/ComDataReq_t.h"
#include <math.h>
#include <QQueue>

// Action_t.a - способ обработки этого действия.
enum eActionDo{
    eNext    // next string/command без допонительных действий; для команд типа set
    , eSend // отправить запрос или набор запросов из очереди.
	, eWaitSend	// Ожидание завершения очереди движения для отправки команды: все команды кроме команд движения.
    ,eSendWait // отправить запрос и ожидать достижения параметра.
    , eEOF // конец программы.файла
};

namespace mito {

struct Action_t{
    eActionDo a;
//    double b[20];
    QQueue<ComDataReq_t> queue;
    union {
        double_t d;
        float f;
        int32_t i;
    }param;
};

} // namespace name

#endif // MITOACTION_H
