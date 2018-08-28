#ifndef OPENSTATISTIC_H
#define OPENSTATISTIC_H

#include "ui_modelstatistic.h"

#include <QWidget>

namespace Ui {
class ModelStatistic;
}


class ModelStatistic : public QWidget
{
    Q_OBJECT

public:
    explicit ModelStatistic(QWidget *parent = nullptr);

signals:

public slots:

private:
    Ui::ModelStatistic *ui;

};

#endif // OPENSTATISTIC_H
