#ifndef GCONSOLE_H
#define GCONSOLE_H

#include <QObject>
#include <QScopedPointer>

//#include <ui_mainwindow.h>
//#include "gparcer/sgcode.h"
#include <sgcode.h>
#include "exchange/threadexchange.h"
#include "gparcer/comdata.h"
#include "profiles/profile.h"
#include "ui_gconsole.h"


//#include "coordinatuswindow.h"

namespace Ui {
class GConsole;
}

extern QMutex thermo_gmutex;

class GConsole : public QWidget
{
    Q_OBJECT

public:
    explicit GConsole(QWidget *parent = nullptr);

//    GConsole(Ui::MainWindow *&ui);
#ifdef THERMO_
    double_t getPath_mm(uint8_t axis, int32_t steps) { return (req_builder->getPath_mm(axis, steps))  ; }
#endif

//    void setCoordinatusWindow(CoordinatusWindow* cwd){ this->coordinatuswindow = cwd; }

    ComData* getComData(){ return this->req_builder; }

private slots:
    void updateStatus(const Status_t* status);
    void failedStatus();


public slots:
    void on_pushButton_linestep_clicked();
    void on_textEdit_command_cursorPositionChanged();
    void on_checkBox_immediately_stateChanged(int arg1);



private:
    Ui::GConsole* ui;

    int oldBlockNumber;

//    ComData _comdata;
     ComData* req_builder;

//     CoordinatusWindow* coordinatuswindow;

//    Profile* profile;
    Coordinatus* coord;

     // type of execution commands Qt::Checked|Qt::Unchecked
     bool checkBox_immediately;
//    ThreadExchange thread;

    /**
     * Разбор одной строки.
     * @brief GcodeWorker::parceString
     * @param src
     * @return
     */
    int parceString(char *src, sGcode *dst);

//bool    GConsole::buildComData(sGcode* sgcode)
    bool buildComData(sGcode* sgcode);

    void setupGconsole();

    void setupThread();


    void setDisabledCursor();

    void setEnabledCursor();

};

#endif // GCONSOLE_H
