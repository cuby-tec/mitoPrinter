#ifndef ZEROPOINTCOMMAND_H
#define ZEROPOINTCOMMAND_H

#include "icommand.h"
#include "exchange/threadarc.h"
#include "links/Status_t.h"
#include <QObject>
#include <QFutureWatcher>
#include <QTimer>

#include "zeropoint_runnable.h"

/**
 * @brief The ZeroPointCommand class
 * Move Extruder to Zero position.
 */

#define Zero_VERTION    2

class ZeroPointCommand : public QObject, ICommand
{
    Q_OBJECT
public:
    explicit ZeroPointCommand(QObject *parent = nullptr);
    ~ZeroPointCommand();

    void execute() override;


signals:
#if Zero_VERTION==2
    void sg_commandDone();
#endif

public slots:
    void updateStatus(const Status_t* status);
    void failedStatus();
    void statusLoaded();
#if Zero_VERTION==2
    void checkStatus();
#endif

private:

    Status_t status;
#if Zero_VERTION==2
    QFutureWatcher<Status_t*> statusLoader;
    QTimer* timer;

    static Status_t*  _checkStatus();
    static Status_t*  moveExtruderZeroPoint();

#endif
#if Zero_VERTION==1
    ThreadArc threadarc;// = new ThreadArc;
#endif
#if Zero_VERTION==3
    ZeroPoint_runnable* zrun;
#endif

};

#endif // ZEROPOINTCOMMAND_H
