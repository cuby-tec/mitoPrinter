#ifndef ZEROPOINTCOMMAND_H
#define ZEROPOINTCOMMAND_H

#include "icommand.h"
#include "exchange/threadarc.h"
#include "links/Status_t.h"
#include <QObject>
#include <QFutureWatcher>

/**
 * @brief The ZeroPointCommand class
 * Move Extruder to Zero position.
 */


class ZeroPointCommand : public QObject, ICommand
{
    Q_OBJECT
public:
    explicit ZeroPointCommand(QObject *parent = nullptr);
    ~ZeroPointCommand();

    void execute() override;


signals:

public slots:
    void updateStatus(const Status_t* status);
    void failedStatus();
    void statusLoaded();

private:

    Status_t status;

    QFutureWatcher<Status_t*> statusLoader;

//    ThreadArc threadarc;// = new ThreadArc;


};

#endif // ZEROPOINTCOMMAND_H
