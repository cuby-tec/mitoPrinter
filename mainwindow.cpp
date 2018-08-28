#include "mainwindow.h"

#include <QDebug>
#include <QtGui>
#include <QGLWidget>
#include <QtWidgets>


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
//----------


    statusBar()->showMessage("Starting ..");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
//TODO
//    qDebug()<<__FILE__<<__LINE__<<"resizeEvent";
}
