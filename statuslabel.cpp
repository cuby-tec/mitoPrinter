#include "statuslabel.h"

#include <QDebug>

StatusLabel::StatusLabel(QWidget *parent) : QWidget(parent)
  , ui(new Ui::StatusLabel)
{
//    this->parent = parent;
    ui->setupUi(parent);

}

void StatusLabel::statusFailed()
{
//    qDebug()<<__FILE__<<__LINE__<<"statusFailed";
    indicateTemperature(eiFail,QString("Can't open device. maybe module not loaded. Use: $sudo insmod ./eclipse-workspace/usbtest/test1.ko \n \t or device dosn't connected."));

}

void StatusLabel::updateStatus(const Status_t* status)
{
//    qDebug()<<__FILE__<<__LINE__;
    indicateTemperature(eiGood,QString("now: %1").arg(status->temperature));
}

const QString message1("Can't open device. maybe module not loaded. Use: $sudo insmod ./eclipse-workspace/usbtest/test1.ko \n \t or device dosn't connected.");
const QString message2("Can't open device.");
const QString message3("Temperature in Hotend.(grad Celsium)");


void
StatusLabel::indicateTemperature(eIndicate ind, QString message)
{
//    QWidget* pa = plot->nativeParentWidget();
//    QLabel* label =  pa->findChild<QLabel *>("temperatureIcon");//temperatureLabel
//    QLabel * tempLabel = pa->findChild<QLabel*>("temperatureLabel");
//    qDebug()<<__FILE__<<__LINE__<<"indicateTemperature:";

    QLabel *label = ui->temperatureIcon;
    QLabel * tempLabel = ui->temperatureLabel;

    switch (ind)
    {
    case eiFail:
        if(label)
        {
            label->setPixmap(QPixmap(":/images/write.png"));
            label->setScaledContents(true);
            tempLabel->setText(message2);
            tempLabel->setToolTip(message1);

        }
        break;

    case eiGood:
        if(label)
        {
            label->setPixmap(QPixmap(":/images/copy.png"));
            label->setScaledContents(true);
             tempLabel->setText(message);
            tempLabel->setToolTip(message3);
        }

        break;
    }
}
