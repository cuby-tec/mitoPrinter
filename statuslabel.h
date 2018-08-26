#ifndef STATUSLABEL_H
#define STATUSLABEL_H

#include "links/status.h"
#include "eindicate.h"
#include "ui_statuslabel.h"

#include <QObject>
#include <QLabel>

//Ui_StatusLabel
namespace Ui {
    class StatusLabel;
}

class StatusLabel : public QWidget
{
    Q_OBJECT

public:
    explicit StatusLabel(QWidget *parent = nullptr);

signals:

public slots:
    void statusFailed();
    void updateStatus(const Status_t *status);

private:
    Ui::StatusLabel *ui;

//    QWidget * parent;

    void indicateTemperature(eIndicate ind, QString message);

};

#endif // STATUSLABEL_H
