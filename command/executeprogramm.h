#ifndef EXECUTEPROGRAMM_H
#define EXECUTEPROGRAMM_H

#include "icommand.h"
#include "gparcer/gcodeworker.h"
#include "gparcer/comdata.h"

#include <QTextStream>
//#include <QObject>

class ExecuteProgramm : public QObject, public ICommand
{

    Q_OBJECT

public:
    explicit ExecuteProgramm(QObject *parent = nullptr);
    virtual ~ ExecuteProgramm() override {}
    virtual void execute() override;
    void execute(QFile &stream);


    ComData* getComdata() const{
        return comdata;
    }

public slots:
    void finished();



signals:
    void sg_executionFinished();

private:
    GcodeWorker *gcodeworker;

    ComData * comdata;

};

#endif // EXECUTEPROGRAMM_H
