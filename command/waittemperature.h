#ifndef WAITTEMPERATURE_H
#define WAITTEMPERATURE_H

#include "icommand.h"

#include <QObject>
#include <QFutureWatcher>
#include <QTimer>
#include <QProgressDialog>

enum WaitTemperature_state{
    e_waitQueue, e_sendCommand, e_waitTemperature
};


class WaitTemperature : public QObject, ICommand
{
    Q_OBJECT
public:
    explicit WaitTemperature(QObject *parent = nullptr, double_t temperature = 0);

    void execute() override;

signals:
    void sg_commandDone();
    void sg_canceled();

public slots:
    void statusLoaded();
    void failedStatus();
    void checkStatus();
    void heatingCancel();

private:

    WaitTemperature_state state;


    double_t targetTemperature;
    QFutureWatcher<Status_t*> statusLoader;
    QTimer* timer;
    QProgressDialog* pd;
};

#endif // WAITTEMPERATURE_H
