#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rightarea.h"

#include <QMainWindow>



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

};

#endif // MAINWINDOW_H
