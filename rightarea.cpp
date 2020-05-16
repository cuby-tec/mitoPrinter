#include "rightarea.h"

#include <QDebug>




RightArea::RightArea(const char* title , QWidget *parent ):QWidget(parent)
  , ui(new Ui::RightArea)
{
    ui->setupUi(this);

    if(parent != nullptr)
    {
//        ui->setupUi(parent);
        parent->setStyleSheet(nullptr);
    }

//    volatile QString st = parent->styleSheet();
//    parent->setStyle(QApplication::style());

    init();

    if(title != nullptr){
        QString ttl = QString(title);
        setWindowTitle(title);
    }

    parent->setMinimumWidth(this->width());

}


RightArea::RightArea(QWidget *parent) : QWidget(parent)
  , ui(new Ui::RightArea)
{
//    Q_ASSERT(parent);
//    ui->setupUi(parent);
    if(parent == nullptr)
        ui->setupUi(this);
    else{
//        ui->setupUi(parent);
        ui->setupUi(this);
        parent->setStyleSheet(nullptr);
    }


    init();

    parent->setMinimumWidth(this->width());
}

void
RightArea::init()
{

    check1 = ui->checkBox_1;    // Thermo
    check2 = ui->checkBox_2;    //coordinatus
    check3 = ui->checkBox_3;    // Console
    check4 = ui->checkBox_4;    //model statistic
    //====== autolevel
    check5 = ui->checkBox_5;    //autolel

    connect(check1,SIGNAL(stateChanged(int)), this, SLOT(hide1(int)));
    connect(check2, SIGNAL(stateChanged(int)), this, SLOT(hide2(int)));
    connect(check3, SIGNAL(stateChanged(int)), this, SLOT(hide3(int)) );
    connect(check4, SIGNAL(stateChanged(int)), this, SLOT(hide4(int)) );
    connect(check5, SIGNAL(stateChanged(int)), this, SLOT(hide5(int)) );


    QString styleSheet = "QCheckBox::indicator:unchecked "
                                    "{image: url(:/images/checkbox_unchecked.png); }"
                                    "QCheckBox::indicator:checked "
                                    "{image: url(:/images/checkbox_checked.png); }"
                                    ;
    check1->setStyleSheet(styleSheet);
    check2->setStyleSheet(styleSheet);
    check3->setStyleSheet(styleSheet);
    check4->setStyleSheet(styleSheet);
    check5->setStyleSheet(styleSheet);

//    QVBoxLayout *layout = ui->verticalLayout;
// ////    layout->setAlignment(check1,Qt::AlignTop);
//    layout->setAlignment(ui->line,Qt::AlignTop);
    QVBoxLayout * layout = ui->verticalLayout_2;
    layout->setAlignment(check2,Qt::AlignTop);
//    layout->setAlignment(layout,Qt::AlignTop);


    //--------- CoordinatusWidget
    QWidget *wd2 = ui->widget_2;
    widCoordinatus = new CoordinatusWidget(wd2);
    check2->setText("Coordinatus");
//    check2->setChecked(false);
//    layout->setAlignment(wd2,Qt::AlignTop);
//    layout->setAlignment(layout,Qt::AlignTop);

    //---------------- model statistic
    QWidget *wd4 = ui->widget_4;
    modelStatistic = new ModelStatistic(wd4);
    check4->setText("Model statistic");
    check4->setChecked(false);
    layout->setAlignment(Qt::AlignTop);

    //------------- Gcosole
    QWidget *wd3 = ui->widget_3;
    gconsole = new GConsole(wd3);
    check3->setText("GConsole");

    //----------- ThermoPlot
    QWidget *wd = ui->widget_1;
    plotter = new ThermoPlot(wd);
    connect(plotter,SIGNAL(sg_statusChanged(const Status_t*)),this, SLOT(updateStatus(const Status_t*)) );
    connect(plotter,SIGNAL(sg_statusFailed()), this,SLOT(failedStatus()) );
//    connect(plotter,SIGNAL(sg_statusChanged(const Status_t*)),widCoordinatus,SLOT(updateStatus(const Status_t*)) );
    check1->setText("ThermoPlot");

    //------------- auyolevel
    QWidget* auwd = ui->widget_5;
    autolevel = new AutolevelWidget(auwd);




    layout = ui->verticalLayout_3;
    layout->setAlignment(Qt::AlignTop);


    Messager* message = Messager::instance();
    connect( message, SIGNAL(sg_statusChanged(const Status_t*)),widCoordinatus, SLOT(updateStatus(const Status_t*)) );
    connect(message, SIGNAL(sg_statusChanged(const Status_t*)),autolevel, SLOT(updateStatus(const Status_t*)));
    connect(plotter, SIGNAL(sg_statusChanged(const Status_t*)),message, SLOT(putStatus(const Status_t*)));


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

void RightArea::hide5(int state)
{
    if(state == 0)
        ui->widget_5->hide();
    else {
        ui->widget_5->show();
    }
}




void RightArea::updateStatus(const Status_t *status)
{
    emit sg_statusChanged(status);
}

void RightArea::failedStatus()
{
    emit sg_statusFailed();
}
