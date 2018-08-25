#include "rightarea.h"

#include <QDebug>

RightArea::RightArea(QWidget *parent) : QWidget(parent)
  , ui(new Ui::RightArea)
{
    Q_ASSERT(parent);
    ui->setupUi(parent);

    QWidget *wd = ui->widget_1;
//    oneform = new OneForm(wd);

//    secondForm = new SecondForm(ui->widget_2);
    plotter = new ThermoPlot(wd);


    check1 = ui->checkBox_1;
    check2 = ui->checkBox_2;

    connect(check1,SIGNAL(stateChanged(int)), this, SLOT(hide1(int)));
    connect(check2, SIGNAL(stateChanged(int)), this, SLOT(hide2(int)));

    QString styleSheet = "QCheckBox::indicator:unchecked "
                                    "{image: url(:/images/checkbox_unchecked.png); }"
                                    "QCheckBox::indicator:checked "
                                    "{image: url(:/images/checkbox_checked.png); }"
                                    ;
    check1->setStyleSheet(styleSheet);

    QVBoxLayout *layout = ui->verticalLayout;
//    layout->setAlignment(check1,Qt::AlignTop);
    layout->setAlignment(ui->line,Qt::AlignTop);

    qDebug()<<__FILE__<<__LINE__<<layout->count();

}

void RightArea::resizeEvent(QResizeEvent *event)
{
    qDebug()<<__FILE__<<__LINE__<<"resize.";
}

void RightArea::hide1(int state)
{
    if(state == false)
        ui->widget_1->hide();
    else
        ui->widget_1->show();
}

void RightArea::hide2(int state)
{
    if(state == 0)
        ui->widget_2->hide();
    else
        ui->widget_2->setHidden(false);
}
