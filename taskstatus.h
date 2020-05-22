#ifndef TASKSTATUS_H
#define TASKSTATUS_H

#include <QWidget>

namespace Ui {
class TaskStatus;
}

class TaskStatus : public QWidget
{
    Q_OBJECT

public:
    explicit TaskStatus(QWidget *parent = nullptr);
    ~TaskStatus();

private:
    Ui::TaskStatus *ui;
};

#endif // TASKSTATUS_H
