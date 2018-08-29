#include "mainwindow.h"

#include <QDebug>
#include <QtGui>
#include <QGLWidget>
#include <QtWidgets>
#include <QMenu>
#include <QAction>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

//----------
    setupMenu();
//------------
    statusBar()->showMessage("Starting ..");
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
//TODO
    //    qDebug()<<__FILE__<<__LINE__<<"resizeEvent";
}

void MainWindow::setupMenu()
{
    menuFile = ui->menuFile;
    actionOpenModel = new QAction(tr("OpenModel"),this);
    menuFile->addAction(actionOpenModel);
    connect(actionOpenModel,&QAction::triggered,this, &MainWindow::s_openFileDo);
}
