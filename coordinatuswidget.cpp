#include "coordinatuswidget.h"
#include "myglobal.h"
#include "links/exchange/eModelstate.h"

CoordinatusWidget::CoordinatusWidget(QWidget *parent) : QWidget(parent)
  , ui(new Ui::CoordinatusWidget)
{
    ui->setupUi(parent);

    coordinatus = Coordinatus::instance();
    controller = new Controller;
}

void CoordinatusWidget::updateStatus(const Status_t *status)
{
    uint8_t ms_state;
    if(coordinatus->isAbsolute())
    {
        ui->c_label_positioning_value->setText(MyGlobal::msg_absolute);
        ui->c_label_positioning_value->setToolTip(MyGlobal::msg_abs_title);
    }else
    {
        ui->c_label_positioning_value->setText(MyGlobal::msg_relative);
        ui->c_label_positioning_value->setToolTip(MyGlobal::msg_rel_title);
    }

    ui->c_label_queueState->setText(QString("%1").arg(status->modelState.queueState)) ;

    const QString state1("ehIdle");
    const QString state2("ehIwork");

    QString t_state;

    ms_state = status->modelState.modelState;

    switch (ms_state) {
    case ehIdle:
        t_state = state1;
        break;

    case ehIwork:
        t_state = state2;
        break;

    default:
        break;
    }
//     ui->c_label_modelState->setText(QString("%1").arg(status->modelState.modelState));
    ui->c_label_modelState->setText(QString(t_state));

    ui->c_label_temperature->setText(QString("%1 C").arg(status->temperature,0,'g',4) );//&deg;

    ui->label_contiCnt->setText(QString("%1").arg(status->instrument2_parameter) );

//    double_t d = comdata->getPath_mm();
    ui->c_label_posX_value->setText(QString("%1 (%2)").arg(controller->getPath_mm(X_AXIS,status->coordinatus[X_AXIS])).arg(status->coordinatus[X_AXIS]));
    ui->c_label_posY_value->setText(QString("%1 (%2)").arg(controller->getPath_mm(Y_AXIS,status->coordinatus[Y_AXIS])).arg(status->coordinatus[Y_AXIS]));
    ui->c_label_posZ_value->setText(QString("%1 (%2)").arg(controller->getPath_mm(Z_AXIS,status->coordinatus[Z_AXIS])).arg(status->coordinatus[Z_AXIS]));
    ui->c_label_posE_value->setText(QString("%1").arg(status->coordinatus[E_AXIS]));


}
