/**
  Company: CUBY,Ltd
    Класс управления процессом отображения данных
    Термодатчика и управления запросами к Контроллеру.
*/

#ifndef THERMOPLOT_H
#define THERMOPLOT_H

//#include <QObject>
#include <QWidget>
#include <QString>
#include <QTimer>

//#include "/home/walery/qtcustomplot/qcustomplot/qcustomplot.h"
#include "qcustomplot.h"
#include "exchange/thermothread.h"
#include "myglobal.h"
#include "ui_thermoplot.h"
#include "eindicate.h"

extern MyGlobal mglobal;



namespace Ui {
class ThermoPlot;
}

class ThermoPlot: public QWidget
{
    Q_OBJECT

public:
    //explicit Base(QObject *parent = 0);
    explicit ThermoPlot(QWidget *parent = nullptr);

    ThermoPlot(QCustomPlot *plot);
/*
    ~ThermoPlot()
    {
        delete (ui);
        logfile.close();
//        thread.terminate();
        thread.exit();
        thread.wait(5000);
    }
*/
signals:
    void sg_statusChanged(const Status_t* status);
    void sg_statusFailed();


private slots:
  void realtimeDataSlot();
  void updateStatus(const Status_t* status);
  void failedStatus();

private:

  Ui::ThermoPlot *ui;

  QObject *parent;

    QCustomPlot* plot;

    QString plotName;

    float oldTemperature;

    QTimer dataTimer;

    ThermoThread thread;

    QString logfileName;

    QFile logfile;

    const Status_t* status;

    void setupPlot(QCustomPlot* customPlot);
#ifdef THERMO_
    void printStatus(const Status_t *c_status);
    void  indicateTemperature(eIndicate ind, QString message);
#endif

    void createLog();

    void writeLog();

    QString datetime();

};

#endif // THERMOPLOT_H
