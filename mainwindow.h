#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rightarea.h"
#include "statuslabel.h"
#include "opengl/graphicsview.h"
#include "opengl/openglscene.h"
#include "opengl/model.h"
#include <QMainWindow>
#include "ui_mainwindow.h"
#include "command/executeprogramm.h"
#include "messager.h"
#include "command/zeropointcommand.h"
#include "command/pushfilamentdown.h"
#include <QAction>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void s_openFileDo();
    void modelLoaded(Model *model);
    void editProfile();
    void actTermoDo();
    void aboutWindowDo();
    void on_commandExecuteProgram();
    void on_commandOpenFile();
    void on_gprogrammFinish();
    void on_message(QString msg);
    //actionZeroPoint_Z
    void on_actionZeroPoint_Z();
    void commandZeroPointDone();
    void filamentDownPressed();
    void filamentDownReleased();
    void filamentUpPressed();
    void filamentUpReleased();
    void on_runProgramButton();
    void on_stopProgram();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;

    RightArea *rightArea;

    StatusLabel* statusLabel;

    GraphicsView *view;

    OpenGLScene *scene;

    QFile gcodeFile;

    ExecuteProgramm* executeProgramm;

    // -------- Status
    QLabel *fileLabel;

    Messager *messager;

//----------- Menu
    QMenu* menuFile;
    QAction* actionOpenModel;

    QMenu *optionsMenu;

    QAction* optionEditAct; // Profile

    QMenu * menuTools;

    QAction* actThermo;

    QAction* actConfig;

    QAction* pidParams;

    QMenu * menuHelp;

    void pidParamsDo();

    void setupMenu();

    PushFilamentDown* pushdown;

};

#endif // MAINWINDOW_H
