#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rightarea.h"
#include "statuslabel.h"
#include "opengl/graphicsview.h"
#include "opengl/openglscene.h"
#include "opengl/model.h"
#include <QMainWindow>
#include "ui_mainwindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;

    RightArea *rightArea;

    StatusLabel* statusLabel;

    GraphicsView *view;

    OpenGLScene *scene;

};

#endif // MAINWINDOW_H
