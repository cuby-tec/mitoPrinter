#ifndef RIGHTAREA_H
#define RIGHTAREA_H

#include "ui_rightArea.h"

#include "thermoplot.h"
#include "coordinatuswidget.h"
#include "gconsole.h"
#include "modelstatistic.h"
#include "autolevel.h"
#include "taskstatus.h"

#include <QWidget>

namespace Ui{
class RightArea;
}

class RightArea : public QWidget
{
    Q_OBJECT
public:
    explicit RightArea(QWidget *parent = nullptr);

    explicit RightArea(const char* title , QWidget *parent = nullptr );

    void resizeEvent(QResizeEvent *event) override;

    CoordinatusWidget *getWidCoordinatus() const;

    ModelStatistic *getModelStatistic() { return modelStatistic;}

signals:
    void sg_statusChanged(const Status_t* status);
    void sg_statusFailed();

public slots:
    void hide1(int state);
    void hide2(int state);
    void hide3(int state);
    void hide4(int state);
    void hide5(int state);
    void updateStatus(const Status_t* status);
    void failedStatus();



private:
    Ui::RightArea *ui;

    QCheckBox * check1;
    QCheckBox * check2;
    QCheckBox * check3;
    QCheckBox * check4;
    QCheckBox * check5; // Autolevel

    ThermoPlot *plotter;

    CoordinatusWidget *widCoordinatus;

    GConsole * gconsole;

    ModelStatistic *modelStatistic;

    AutolevelWidget* autolevel;

    TaskStatus * taskstatus;

    //-------- function
    void init();

};

#endif // RIGHTAREA_H
