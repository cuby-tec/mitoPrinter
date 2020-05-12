#include "autolevel.h"
#include "step_motor/controller.h"
#include "gparcer/coordinatus.h"

AutolevelWidget::AutolevelWidget(QWidget *parent) : QWidget(parent) , ui(new Ui::AutolevelWidget)
{
    ui->setupUi(parent);
}

void AutolevelWidget::statusFailed()
{
    //TODO
}

//#define COMMAND_AUTOLEVEL       (1<<2)
void AutolevelWidget::updateStatus(const Status_t *status)
{
    //TODO
//    double_t stepsAxis[4];

    Coordinatus* coordinatus = Coordinatus::instance();
    Controller *controller = new Controller;//microstep in coordinatus is set .

//    for(uint i=0; i<N_AXIS;i++ ){
//        stepsAxis[i] = static_cast<double_t>(stepsTable[coordinatus->getMicrostep(i)]);
//    }

    //"active":"passiv"
    ui->label_flagValue->setText((status->modelState.reserved2&(1<<6))?"passiv":"active");
/*    ui->label_levelValue->setText(QString("%1 (%2)[%3](%4)").arg(controller->getPath_mm(Z_AXIS,status->autolevel))
                                  .arg(status->autolevel).arg(coordinatus->getNextValue(Z_AXIS))
                                  .arg(coordinatus->position[Z_AXIS]));
*/
    ui->label_levelValue->setText(QString("%1 [%2]")
                                  .arg(controller->getPath_mm(Z_AXIS,status->autolevel))
//                                  .arg(status->autolevel)
                                  .arg(coordinatus->getNextValue(Z_AXIS)));
//                                  .arg(coordinatus->position[Z_AXIS]));


//    ui->label_levelValue->setText(QString("%1(%2)").arg(controller->getPath_mm(Z_AXIS,status->coordinatus[Z_AXIS]))
//                                  .arg(status->coordinatus[Z_AXIS]));

    //COMMAND_AUTOLEVEL-0x02 @ uint8_t reserved1;
    ui->label_activeValue->setText((status->modelState.reserved1&COMMAND_AUTOLEVEL)?"ON":"OFF" );

}
