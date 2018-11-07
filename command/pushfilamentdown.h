#ifndef PUSHFILAMENTDOWN_H
#define PUSHFILAMENTDOWN_H

#include "icommand.h"

#include <QObject>
#include <QFutureWatcher>
#include <QTimer>

class PushFilamentDown : public QObject, public ICommand
{
    Q_OBJECT
public:
    explicit PushFilamentDown(QObject *parent = nullptr);

    /**
     * @brief execute
     * Start move filament down.
     */
    void execute() override;

    /**
     * @brief stop
     * Stop move filament down.
     */
    void stop();

    void setDirection(bool value){
        direction = value;
    }

signals:

public slots:
    void makeStep();
    void statusLoaded();

private:
    QTimer* timer;

    int32_t counter;
    size_t step;

    bool direction;

    QFutureWatcher<Status_t*> statusLoader;


};

#endif // PUSHFILAMENTDOWN_H
