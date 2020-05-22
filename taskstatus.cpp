#include "taskstatus.h"
#include "ui_taskstatus.h"

TaskStatus::TaskStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskStatus)
{
    ui->setupUi(this);
}

TaskStatus::~TaskStatus()
{
    delete ui;
}
