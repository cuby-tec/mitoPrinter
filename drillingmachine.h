#ifndef DRILLINGMACHINE_H
#define DRILLINGMACHINE_H

#include <QWidget>

#include "coordinatuswidget.h"
#include "gconsole.h"
#include "autolevel.h"

namespace Ui {
class DrillingMachine;
}

class DrillingMachine : public QWidget
{
    Q_OBJECT

public:
    explicit DrillingMachine(QWidget *parent = nullptr);
    ~DrillingMachine();


public slots:
    void hide2(int state);  //coordinatus
    void hide3(int state);  // Console
    void hide4(int state);  //model statistic
    void hide5(int state);  //autolel
    void action1();

private:

    Ui::DrillingMachine *ui;

    void init();

    CoordinatusWidget* widCoordinatus;

    GConsole* gconsole;

    AutolevelWidget* autolevel;

};

#endif // DRILLINGMACHINE_H
