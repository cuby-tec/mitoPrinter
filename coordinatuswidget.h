#ifndef COORDINATUSWIDGET_H
#define COORDINATUSWIDGET_H

#include "links/Status_t.h"
#include "ui_coordinatuswidget.h"
#include "gparcer/coordinatus.h"
//#include "gparcer/comdata.h"
#include "step_motor/controller.h"


#include <QWidget>
//Ui_CoordinatusWidget
namespace Ui {
    class CoordinatusWidget;
}


class CoordinatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoordinatusWidget(QWidget *parent = nullptr);

signals:

public slots:
    void updateStatus(const Status_t *status);

private:
    Ui::CoordinatusWidget *ui;

    Coordinatus* coordinatus;

//    ComData* comdata;
    Controller *controller;

};

#endif // COORDINATUSWIDGET_H
