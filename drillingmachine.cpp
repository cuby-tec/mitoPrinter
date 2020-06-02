#include "drillingmachine.h"
#include "ui_drillingmachine.h"

DrillingMachine::DrillingMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrillingMachine)
{
    widCoordinatus = nullptr;

    ui->setupUi(this);

    init();
}

DrillingMachine::~DrillingMachine()
{
    delete ui;
}

void DrillingMachine::init()
{
    QCheckBox* check2 = ui->checkBox_2;    //coordinatus

    QCheckBox* check3 = ui->checkBox_3;    // Console

    QCheckBox*     check4 = ui->checkBox_4;    //model statistic
    //====== autolevel
    QCheckBox* check5 = ui->checkBox_5;    //autolel


    connect(check2, SIGNAL(stateChanged(int)), this, SLOT(hide2(int)));
    connect(check3, SIGNAL(stateChanged(int)), this, SLOT(hide3(int)) );
    connect(check4, SIGNAL(stateChanged(int)), this, SLOT(hide4(int)) );
    connect(check5, SIGNAL(stateChanged(int)), this, SLOT(hide5(int)) );

    QString styleSheet = "QCheckBox::indicator:unchecked "
                                    "{image: url(:/images/checkbox_unchecked.png); }"
                                    "QCheckBox::indicator:checked "
                                    "{image: url(:/images/checkbox_checked.png); }"
                                    ;
    check2->setStyleSheet(styleSheet);
    check3->setStyleSheet(styleSheet);
    check4->setStyleSheet(styleSheet);
    check5->setStyleSheet(styleSheet);

    //--------- CoordinatusWidget
    QWidget *wd2 = ui->widget_2;
    widCoordinatus = new CoordinatusWidget(wd2);
//    wd2->adjustSize();
//    widCoordinatus->show();
//    wd2->show();
    wd2->setMinimumSize(widCoordinatus->size());

    check2->setChecked(true);

    //---------------- model statistic
    QWidget *wd4 = ui->widget_4;
//    modelStatistic = new ModelStatistic(wd4);
    QWidget* drillingStatistic = new QWidget(wd4);
    QLabel* label1 = new QLabel(drillingStatistic);
    label1->setText("Drilling statistic");

    check4->setText("Model statistic");
    check4->setChecked(false);
    hide4(false);


    //------------- Gcosole
    QWidget *wd3 = ui->widget_3;
    gconsole = new GConsole(wd3);
    check3->setText("GConsole");
    check3->setChecked(true);

    //------------- autolevel
    QWidget* auwd = ui->widget_5;
    autolevel = new AutolevelWidget(auwd);

    check5->setChecked(false);
    hide5(0);





    QVBoxLayout * layout_2 = ui->verticalLayout_2;
//    layout_2->setAlignment(check2,Qt::AlignTop);
    layout_2->setAlignment(Qt::AlignTop);

    QVBoxLayout * layout_3 = ui->verticalLayout_3;
//    layout_3->setAlignment(check2,Qt::AlignTop);
    layout_3->setAlignment(Qt::AlignTop);


}

// slots
void DrillingMachine::hide2(int state)
{
    //TODO
    if(state == 0)
        ui->widget_2->hide();
    else
        ui->widget_2->setHidden(false);
}

void DrillingMachine::hide3(int state)
{
    //TODO
    if(state == 0)
        ui->widget_3->hide();
    else
        ui->widget_3->setHidden(false);
}

void DrillingMachine::hide4(int state)
{
    //TODO
    if(state == 0)
        ui->widget_4->hide();
    else
        ui->widget_4->setHidden(false);
}

void DrillingMachine::hide5(int state)
{
    //TODO
    if(state == 0)
        ui->widget_5->hide();
    else
        ui->widget_5->setHidden(false);
}



