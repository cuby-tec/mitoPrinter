#include "mainwindow.h"
#include "profiles/profiledialog.h"
#include "profiles/thermopiddialog.h"
#include "thermolog/thermologdialog.h"
#include "aboutwindow.h"

#include <QDebug>
#include <QtGui>
#include <QGLWidget>
#include <QtWidgets>
#include <QMenu>
#include <QAction>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    executeProgramm(new ExecuteProgramm)
{
    ui->setupUi(this);

//------------------
    //rWidget
    rightArea = new RightArea(ui->rightWidget); // rWidget
//------------------
//    QLabel* statusl = ui->statusLabel;
    statusLabel = new StatusLabel(ui->statusLabel);

    connect(rightArea,SIGNAL(sg_statusChanged(const Status_t*)),statusLabel,SLOT(updateStatus(const Status_t*)) );
    connect(rightArea,SIGNAL(sg_statusFailed()),statusLabel,SLOT(statusFailed()) );

//--------- openGL
    view = ui->graphicsView;
    view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    scene = new OpenGLScene;
    view->setScene(scene);

    //    OpenGLScene * scene = static_cast<OpenGLScene*>( view->scene());
    connect(scene,SIGNAL(modelFileOpened(Model*)),this,SLOT(modelLoaded(Model*)));

    connect(executeProgramm,SIGNAL(sg_executionFinished()), this, SLOT(on_gprogrammFinish()) );

//----------
    setupMenu();


    fileLabel = new QLabel();
    fileLabel->setText("not opened");
    QString style("background-color: rgb(203, 237, 191);");
    fileLabel->setStyleSheet(style);
    statusBar()->addPermanentWidget(fileLabel);

//------------
    statusBar()->showMessage("Starting ..");

    messager = Messager::instance();
    connect(messager, SIGNAL(sg_Message(QString)),this, SLOT(on_message(QString)));
    //sg_executeComplite
    connect(messager,SIGNAL(sg_executeComplite()),this, SLOT(on_gprogrammFinish()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::s_openFileDo()
{
    qint64 lineLength;
    char buf[256];
//    QString filename("filepath.txt");
    QString path("/home/walery/Документы/3d-printer/Assembled_Prusa_IT2_OpenSCAD/");
    QFile path_file("filepath.txt");
    QFileInfo *info = new QFileInfo(path_file);
    if (!path_file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<<__FILE__<<__LINE__<<"File dosn't opened."<<info->absoluteFilePath();
        return;
    }
    qDebug()<<__FILE__<<__LINE__<<"File opened:"<<info->absoluteFilePath();

    lineLength = path_file.readLine(buf,255);
    if (lineLength != -1) {
        // the line is available in buf
        path=QString(buf);
    }

    QString filename = QFileDialog::getOpenFileName(this, tr("Choose model"),path,tr(" stl (*.stl *.ast *.obj)") );
    if(filename.isNull()){
        qDebug()<<__FILE__<<__LINE__ << "File don't selected.";
    }else{
        info = new QFileInfo(filename);
        std::string str = info->path().toStdString(); //filename.toStdString();
        strncpy(buf,str.c_str(),255);
        path_file.seek(0);
        path_file.write(buf,filename.size());
        path_file.close();
//        qDebug()<< "Open STL file :"<<filename;
        //TODO

//        ModelSTL* model = new ModelSTL(filename); // #24
//        glwidget->setupData(model->constData(),model->count()); // #24

//        Model * model = new Model(filename);
//        glwidget->setGl_mode(model);
        scene->loadModel(filename);

        qDebug()<<__FILE__<<__LINE__<<"Model ready. "<<filename;
        statusBar()->showMessage( QString("Model ready. %1").arg(filename) );

    }
//---------------
//    qDebug()<<__FILE__<<__LINE__<<"s_openFileDo";

}

void MainWindow::modelLoaded(Model *model)
{
    qDebug()<<__FILE__<<__LINE__<<"modelLoaded";

    ModelStatistic * modelstatistic = rightArea->getModelStatistic();

    modelstatistic->setModelStatistic(model);
}

void MainWindow::editProfile()
{
    //TODO
    ProfileDialog* options = new ProfileDialog(this);

    qDebug() <<__FILE__<<__LINE__<< "editProfile";
    if(options->exec())
    {
        options->baseSize();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
//TODO
    //    qDebug()<<__FILE__<<__LINE__<<"resizeEvent";
}

//----------- Menu ACtions

const QString actionName1(QObject::tr("&Thermo"));
const QString actionName2(QObject::tr("&Config"));
const QString actionName3(QObject::tr("PID params"));
const QString actionName4(QObject::tr("&Open"));
const QString actionName5(QObject::tr("About"));

const QString statusToolTip1(QObject::tr("Open file with G-Code content."));

void MainWindow::setupMenu()
{
    menuFile = ui->menuFile;
    actionOpenModel = new QAction(tr("OpenModel"),this);
    menuFile->addAction(actionOpenModel);
    connect(actionOpenModel,&QAction::triggered,this, &MainWindow::s_openFileDo);

//--------------------MWNU Options
        optionsMenu = ui->menuOptions;

        optionEditAct = new QAction(tr("&Edit"),this);

        optionEditAct->setStatusTip(tr("Edit profile"));

        optionsMenu->addAction(optionEditAct);

        connect(optionEditAct,&QAction::triggered,this,&MainWindow::editProfile);

        //------------------ MENU Tools
        menuTools = ui->menuTools;

        actThermo = new QAction(actionName1,this); // Thermo
        actThermo->setStatusTip("View thermo log.");
        menuTools->addAction(actThermo);
        //            connect(actThermo,&QAction::triggered,this,&MainWindow::actTermoDo);
        connect(actThermo,SIGNAL(triggered()),this,SLOT(actTermoDo()) );


        actConfig = new QAction(actionName2,this); // Config

        //    QList<QAction *> act = menuTools->findChildren<QAction *>();// Config

        //    qDebug() << "MainWindow[61]" << act[0]->text();

        pidParams = new QAction(actionName3,this);//
        pidParams->setStatusTip("setting up PID parameters in current session.");
        menuTools->addAction(pidParams);
        connect(pidParams,&QAction::triggered,this,&MainWindow::pidParamsDo);

        //------------------- HelpMenu
        menuHelp = ui->menuHelp;
        QAction* aboutAction = new QAction(actionName5,this);
        menuHelp->addAction(aboutAction);
//        connect(aboutAction,&QAction::triggered,w,&MainWindow::aboutWindowDo);
        connect(aboutAction,SIGNAL(triggered()),this,SLOT(aboutWindowDo()));
// --------- Execute menu

        QAction *executeProgramAction = ui->actionRun;
        executeProgramAction->setEnabled(false);
        connect(executeProgramAction, SIGNAL(triggered()),this,SLOT(on_commandExecuteProgram()));

        QAction *openAction = ui->actionOpen_GCode;
        connect(openAction, SIGNAL(triggered()),this, SLOT(on_commandOpenFile()));

}

void
MainWindow::pidParamsDo()
{
    ThermoPIDDialog* tpidDialg = new ThermoPIDDialog(this);
    tpidDialg->show();
}

void
MainWindow::actTermoDo()
{

    qDebug() << "MainWindow::actTermoDo()[103]";

    ThermoLogDialog* thermolog = new ThermoLogDialog(this);

    thermolog->show();

}

// About window
void
MainWindow::aboutWindowDo()
{
    Aboutwindow* about = new Aboutwindow(this);

    if(about->exec()){
        about->baseSize();
    }
}

void MainWindow::on_commandExecuteProgram()
{
 //TODO

    qDebug()<<__FILE__<<__LINE__;
    QAction *action = ui->actionRun;
    action->setEnabled(false);

    action = ui->actionOpen_GCode;
    action->setEnabled(false);
//    QTextStream stream(&gcodeFile);
    statusBar()->showMessage("Program executing ...");
    executeProgramm->execute(gcodeFile);
}

void MainWindow::on_commandOpenFile()
{
    QString folder("/home/walery/Документы/3d-printer/ragel"); //home/walery/Документы/3d-printer/ragel/exmple.gcode
    QString filename = QFileDialog::getOpenFileName(this, tr("Open G-Code file"),folder,tr("Gcode (*.gcode *.ngx);;All (*.*)"));//,nullptr,QFileDialog::DontUseNativeDialog

    if(filename.isNull())
    {
        qDebug() << "File don't selected.";
    }else{

        qDebug()<< "Open file with G-code file:"<<filename;
        gcodeFile.setFileName(filename);
        if(gcodeFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug()<<__FILE__<<__LINE__<<"File opened:"<<filename;

            QAction *actionRun = ui->actionRun;
            actionRun->setEnabled(true);
            //TODO Check file for errors.
//            statusBar()->showMessage(QString("Opened: %1").arg(gcodeFile.fileName()));
            fileLabel->setText(gcodeFile.fileName());
        }

    }

}

void MainWindow::on_gprogrammFinish()
{
    statusBar()->showMessage("Programm finished.");
    QAction *actionRun = ui->actionRun;
    actionRun->setEnabled(true);
    QAction* action = ui->actionOpen_GCode;
    action->setEnabled(true);
}

void MainWindow::on_message(QString msg)
{
    /*
     * 		printf("Can't open device. maybe module not loaded. Use: $sudo insmod ./eclipse-workspace/usbtest/test1.ko \n"
                "or device dosn't connected.\n");
     */
    QString err("Can't open device. maybe module not loaded."
                " maybe module not loaded."
                "Use: $sudo insmod ./eclipse-workspace/usbtest/test1.ko");

    statusBar()->showMessage(msg);
    QMessageBox box;
    box.setText(err);
    QAction *actionRun = ui->actionRun;
    actionRun->setEnabled(true);
    int ret = box.exec();
} //


