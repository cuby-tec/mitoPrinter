#include "pushfilamentdown.h"
#include "gparcer/gcodeworker.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

#define cout qDebug()<<__FILE__<<__LINE__


PushFilamentDown::PushFilamentDown(QObject *parent) : QObject(parent)
{
    step = 1;
    counter = 0;
}

void PushFilamentDown::execute()
{
    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this, SLOT(makeStep()));
    connect(&statusLoader, SIGNAL(finished()), this, SLOT(statusLoaded()));

    QString gcommand("G0 E1");
    QString gcommand92("G92 E0");
    counter = 0;

    Status_t* st = PushFilamentDown::executeGCommand(gcommand92);
    statusLoader.setFuture(QtConcurrent::run(PushFilamentDown::executeGCommand,gcommand));

    timer->start(200);
    cout<<"execute";
}

void PushFilamentDown::stop()
{
    timer->stop();
    //TODO
    cout<<"Stop";
}

void PushFilamentDown::makeStep()
{
    //TODO

    counter += step;
    QString cmd = QString("G0 E%1").arg(counter);

    statusLoader.setFuture(QtConcurrent::run(PushFilamentDown::executeGCommand,cmd));
    cout<<"makeStep:"<<cmd;
}

void PushFilamentDown::statusLoaded()
{
    //TODO
    Status_t* st = statusLoader.result();
}
