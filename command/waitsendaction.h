#ifndef WAITSENDACTION_H
#define WAITSENDACTION_H

#include "gparcer/mitoaction.h"
#include "icommand.h"

#include <QObject>
#include <QFutureWatcher>
#include <QTimer>

enum WaitSend_state{
    ews_waitQueue, ews_sendCommand
};


class WaitSendAction : public QObject, ICommand
{
    Q_OBJECT
public:
    explicit WaitSendAction(QObject *parent = nullptr, mito::Action_t* action = nullptr);

    void execute() override;

    static Status_t* _sendAction(mito::Action_t* action);



    uint32_t getSegment_number() ;
    void setSegment_number( uint32_t &value);

signals:
    void sg_commandDone();
    void sg_canceled();

public slots:
    void statusLoaded();
    void failedStatus();
    void checkStatus();

private:

    WaitSend_state state;
    mito::Action_t* action;

    uint32_t segment_number;    // current line number

    QFutureWatcher<Status_t*> statusLoader;
    QTimer* timer;

};

#endif // WAITSENDACTION_H
