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

#define M_CONC(A, B) M_CONC_(A, B)
#define M_CONC_(A, B) A##B
//#define enderValue(V) (M_CONC(status->modelState.reserved2,M_CONC( &,V)))?"ON":"OFF"
#define enderValue(V) (status->modelState.reserved2&V)?"ON":"OFF"

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

    /*
     *     ehIdle = 1, ehIwork
     *     ,ehEnderXmax, ehEnderXmin, ehEnderYmax, ehEnderYmin, ehEnderZmax, ehEnderZmin
     *     ,ehException, ehWait_instrument1, ehWait_instrument2
     */
//    const QString state1("ehIdle");
//    const QString state2("ehIwork");
    const QString state3("ehEnderXmax");
    const QString state[12] = {"unnown","ehIdle","ehIwork"
                               ,"ehEnderXmax","ehEnderXmin","ehEnderYmax","ehEnderYmin"
                               ,"ehEnderZmax","ehEnderZmin"
                               ,"ehException","ehWait_instrument1","ehWait_instrument2"};
    QString t_state;

    ms_state = status->modelState.modelState;

    t_state = state[ms_state];
//    switch (ms_state) {
//    case ehIdle:
//        t_state = state1;
//        break;

//    case ehIwork:
//        t_state = state2;
//        break;

//    default:
//        break;
//    }
//     ui->c_label_modelState->setText(QString("%1").arg(status->modelState.modelState));
    ui->c_label_modelState->setText(QString(t_state));

    ui->c_label_temperature->setText(QString("%1 C").arg(status->temperature,0,'g',4) );//&deg;
    int32_t derrior = static_cast<int32_t>(status->instrument2_parameter);
    int32_t out = static_cast<int32_t>(status->instrument3_parameter);
    int32_t dinteg = static_cast<int32_t>(status->instrument4_parameter);

//    ui->label_contiCnt->setText(QString("%1").arg(status->currentSegmentNumber) );
    ui->label_contiCnt->setText(QString("d: %1 integ: %2 out:%3").arg( static_cast<double_t>(derrior) /100.0).arg(static_cast<double_t>(dinteg)).arg(static_cast<double_t>(out)/10.0));


//    double_t d = comdata->getPath_mm();
    ui->c_label_posX_value->setText(QString("%1 (%2)[%3]").arg(controller->getPath_mm(X_AXIS,status->coordinatus[X_AXIS]))
                                    .arg(status->coordinatus[X_AXIS]).arg(coordinatus->getNextValue(X_AXIS)) );
    ui->c_label_posY_value->setText(QString("%1 (%2)[%3]").arg(controller->getPath_mm(Y_AXIS,status->coordinatus[Y_AXIS]))
                                    .arg(status->coordinatus[Y_AXIS]).arg(coordinatus->getNextValue(Y_AXIS)));
    ui->c_label_posZ_value->setText(QString("%1 (%2)[%3]").arg(controller->getPath_mm(Z_AXIS,status->coordinatus[Z_AXIS]))
                                    .arg(status->coordinatus[Z_AXIS]).arg(coordinatus->getNextValue(Z_AXIS)));
    ui->c_label_posE_value->setText(QString("%1 (%2)[%3]").arg(controller->getPath_mm(E_AXIS,status->coordinatus[E_AXIS]))
                                    .arg(status->coordinatus[E_AXIS]).arg(coordinatus->getNextValue(E_AXIS)));

    ui->label_ender_xmin_value->setText(enderValue(ENDER_X_MIN));
    ui->label_ender_xmax_value->setText(enderValue(ENDER_X_MAX));
    ui->label_ender_ymin_value->setText(enderValue(ENDER_Y_MIN));
    ui->label_ender_ymax_value->setText(enderValue(ENDER_Y_MAX));
    ui->label_ender_zmin_value->setText(enderValue(ENDER_Z_MIN));
    ui->label_ender_zmax_value->setText(enderValue(ENDER_Z_MAX));


}
