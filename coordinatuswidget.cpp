#include "coordinatuswidget.h"
#include "myglobal.h"
#include "links/eModelstate.h"
#include "step_motor/stepmotor.h"
#include "step_motor/stepstable.h"

#include <QtDebug>

#define cout qDebug()<<__FILE__<<__LINE__

CoordinatusWidget::CoordinatusWidget(QWidget *parent) : QWidget(parent)
  , ui(new Ui::CoordinatusWidget)
{
    ui->setupUi(parent);

    //layout_2->setAlignment(Qt::AlignTop);
    QVBoxLayout* l = ui->verticalLayout;
    l->setAlignment(Qt::AlignTop);

//    setGeometry(ui->verticalLayoutWidget->geometry());
    QWidget::setGeometry(ui->verticalLayoutWidget->geometry());
    QWidget::setMinimumHeight(ui->verticalLayoutWidget->height());

    coordinatus = Coordinatus::instance();
    controller = new Controller;// microstep in coordinatus dos't set yet.
}

#define M_CONC(A, B) M_CONC_(A, B)
#define M_CONC_(A, B) A##B
//#define enderValue(V) (M_CONC(status->modelState.reserved2,M_CONC( &,V)))?"ON":"OFF"
#define enderValue(V) (status->modelState.reserved2&V)?"ON":"OFF"

void CoordinatusWidget::updateStatus(const Status_t *status)
{
    uint8_t ms_state;
    double_t stepsAxis[4];
    if(status->modelState.modelState == ehException)
    {
        qFatal(" %s %d ========== State ehException ===========",__FILE__,__LINE__);
    }
    if(coordinatus->isAbsolute())
    {
        ui->c_label_positioning_value->setText(MyGlobal::msg_absolute);
        ui->c_label_positioning_value->setToolTip(MyGlobal::msg_abs_title);
    }else
    {
        ui->c_label_positioning_value->setText(MyGlobal::msg_relative);
        ui->c_label_positioning_value->setToolTip(MyGlobal::msg_rel_title);
    }

    ui->c_label_queueState->setText(QString("%1[%2]").arg(status->modelState.queueState).arg(status->freeSegments));
    ui->c_label_queueState->setToolTip("CommandQueue size[OrderlyQueue size]");

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

    ui->c_label_temperature->setText(QString("%1C [%2]").arg(static_cast<double_t>(status->temperature) ,0,'g',4).arg( static_cast<double_t>(status->bed_temperature),0,'g',4) );//&deg;
    int32_t derrior = static_cast<int32_t>(status->instrument2_parameter);
    int32_t out = static_cast<int32_t>(status->instrument3_parameter);
    int32_t dinteg = static_cast<int32_t>(status->instrument4_parameter);

    ui->label_contiCnt->setText(QString("%1").arg(status->currentSegmentNumber) );
//    ui->label_contiCnt->setText(QString("d: %1 integ: %2 out:%3").arg( static_cast<double_t>(derrior) /100.0).arg(static_cast<double_t>(dinteg)).arg(static_cast<double_t>(out)/10.0));
    ui->c_label_regulator_value->setText(QString("d: %1 integ: %2 out:%3").arg( static_cast<double_t>(derrior) /100.0).arg(static_cast<double_t>(dinteg)).arg(static_cast<double_t>(out)/10.0));
    for(uint i=0; i<N_AXIS;i++ ){
        stepsAxis[i] = static_cast<double_t>(stepsTable[coordinatus->getMicrostep(i)]);
    }

//    double_t d = comdata->getPath_mm();
    ui->c_label_posX_value->setText(QString("%1 (%2)[%3](%4)").arg(controller->getPath_mm(X_AXIS,status->coordinatus[X_AXIS])/stepsAxis[X_AXIS])
                                    .arg(status->coordinatus[X_AXIS]).arg(coordinatus->getNextValue(X_AXIS))
                                    .arg(coordinatus->position[X_AXIS]) );
    ui->c_label_posY_value->setText(QString("%1 (%2)[%3](%4)").arg(controller->getPath_mm(Y_AXIS,status->coordinatus[Y_AXIS])/stepsAxis[Y_AXIS])
                                    .arg(status->coordinatus[Y_AXIS]).arg(coordinatus->getNextValue(Y_AXIS))
                                    .arg(coordinatus->position[Y_AXIS]));
    ui->c_label_posZ_value->setText(QString("%1 (%2)[%3](%4)").arg(controller->getPath_mm(Z_AXIS,status->coordinatus[Z_AXIS])/stepsAxis[Z_AXIS])
                                    .arg(status->coordinatus[Z_AXIS]).arg(coordinatus->getNextValue(Z_AXIS))
                                    .arg(coordinatus->position[Z_AXIS]));
    ui->c_label_posE_value->setText(QString("%1 (%2)[%3]").arg(controller->getPath_mm(E_AXIS,status->coordinatus[E_AXIS])/stepsAxis[E_AXIS])
                                    .arg(status->coordinatus[E_AXIS]).arg(coordinatus->getNextValue(E_AXIS)));

    ui->label_ender_xmin_value->setText(enderValue(ENDER_X_MIN));
    ui->label_ender_xmax_value->setText(enderValue(ENDER_X_MAX));
    ui->label_ender_ymin_value->setText(enderValue(ENDER_Y_MIN));
    ui->label_ender_ymax_value->setText(enderValue(ENDER_Y_MAX));
    ui->label_ender_zmin_value->setText(enderValue(ENDER_Z_MIN));
    ui->label_ender_zmax_value->setText(enderValue(ENDER_Z_MAX));
#if LOG_LEVEL==1
    cout<<"posX:"<<controller->getPath_mm(X_AXIS,status->coordinatus[X_AXIS])/stepsAxis[X_AXIS];
#endif
    ui->c_label_microstep_value->setText(QString("X[%1] Y[%2] Z[%3] E[%4]")
                                         .arg(coordinatus->getMicrostep(X_AXIS)).arg(coordinatus->getMicrostep(Y_AXIS))
                                         .arg(coordinatus->getMicrostep(Z_AXIS)).arg(coordinatus->getMicrostep(E_AXIS))
                                         );
    Controller* controller = new Controller;
    double_t xds, yds, zds, eds;
    lines lm;

    xds = controller->get_MMperStep(X_AXIS, coordinatus);
    yds = controller->get_MMperStep(Y_AXIS, coordinatus);
    zds = controller->get_MMperStep(Z_AXIS, coordinatus);
    eds = controller->get_MMperStep(E_AXIS, coordinatus);
    ui->c_label_microstep_value->setToolTip(QString("X[%1] Y[%2] Z[%3] E[%4] millimeters per step")
                                            .arg(xds).arg(yds).arg(zds).arg(eds)
                                            );


    QLabel* label_connection = ui->c_label_connection_value;
    QPixmap pixmap(":/images/connection_connected.xpm");
    label_connection->setPixmap(pixmap);
    label_connection->setFrameShape(QFrame::NoFrame);

}

void CoordinatusWidget::failedStatus()
{
    QLabel* label_connection = ui->c_label_connection_value;
    QPixmap pixmap(":/images/connection_disconnected.xpm");
    label_connection->setPixmap(pixmap);
    label_connection->setToolTip(QString("disconnected now"));
    label_connection->setFrameShape(QFrame::Box);
}// end of updateStatus
