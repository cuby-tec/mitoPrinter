#include "rightarea.h"

#include <QDebug>

RightArea::RightArea(QWidget *parent) : QWidget(parent)
  , ui(new Ui::RightArea)
{
    Q_ASSERT(parent);
    ui->setupUi(parent);


    check1 = ui->checkBox_1;
    check2 = ui->checkBox_2;
    check3 = ui->checkBox_3;
    check4 = ui->checkBox_4;

    connect(check1,SIGNAL(stateChanged(int)), this, SLOT(hide1(int)));
    connect(check2, SIGNAL(stateChanged(int)), this, SLOT(hide2(int)));
    connect(check3, SIGNAL(stateChanged(int)), this, SLOT(hide3(int)) );
    connect(check4, SIGNAL(stateChanged(int)), this, SLOT(hide4(int)) );


    QString styleSheet = "QCheckBox::indicator:unchecked "
                                    "{image: url(:/images/checkbox_unchecked.png); }"
                                    "QCheckBox::indicator:checked "
                                    "{image: url(:/images/checkbox_checked.png); }"
                                    ;
    check1->setStyleSheet(styleSheet);
    check2->setStyleSheet(styleSheet);
    check3->setStyleSheet(styleSheet);
    check4->setStyleSheet(styleSheet);

    QVBoxLayout *layout = ui->verticalLayout;
//    layout->setAlignment(check1,Qt::AlignTop);
    layout->setAlignment(ui->line,Qt::AlignTop);

    //----------- ThermoPlot

    QWidget *wd = ui->widget_1;
    plotter = new ThermoPlot(wd);
    connect(plotter,SIGNAL(sg_statusChanged(const Status_t*)),this, SLOT(updateStatus(const Status_t*)) );
    connect(plotter,SIGNAL(sg_statusFailed()), this,SLOT(failedStatus()) );

    check1->setText("ThermoPlot");


    //--------- CoordinatusWidget
    QWidget *wd2 = ui->widget_2;
    widCoordinatus = new CoordinatusWidget(wd2);
    check2->setText("Coordinatus");
    check2->setChecked(false);

    connect(plotter,SIGNAL(sg_statusChanged(const Status_t*)),widCoordinatus,SLOT(updateStatus(const Status_t*)) );

    Messager* message = Messager::instance();
    connect( message, SIGNAL(sg_statusChanged(const Status_t*)),widCoordinatus, SLOT(updateStatus(const Status_t*)) );

    //------------- Gcosole
    QWidget *wd3 = ui->widget_3;
    gconsole = new GConsole(wd3);
    check3->setText("GConsole");


    //---------------- model statistic

    QWidget *wd4 = ui->widget_4;
    modelStatistic = new ModelStatistic(wd4);
    check4->setText("Model statistic");
    check4->setChecked(false);
#if LEVEL==1
    qDebug()<<__FILE__<<__LINE__<<layout->count();
#endif
}

void RightArea::resizeEvent(QResizeEvent *event)
{
#if LEVEL==1
    qDebug()<<__FILE__<<__LINE__<<"resize.";
#endif
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

void RightArea::hide3(int state)
{
    if(state == 0)
        ui->widget_3->hide();
    else
        ui->widget_3->setHidden(false);
}

void RightArea::hide4(int state)
{
    if(state == 0)
        ui->widget_4->hide();
    else
        ui->widget_4->setHidden(false);
}




void RightArea::updateStatus(const Status_t *status)
{
    emit sg_statusChanged(status);
}

void RightArea::failedStatus()
{
    emit sg_statusFailed();
}
