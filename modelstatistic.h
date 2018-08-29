#ifndef OPENSTATISTIC_H
#define OPENSTATISTIC_H

#include "ui_modelstatistic.h"
#include "opengl/model.h"

#include <QWidget>
#include <QString>

namespace Ui {
class ModelStatistic;
}


class ModelStatistic : public QWidget
{
    Q_OBJECT

public:
    explicit ModelStatistic(QWidget *parent = nullptr);
    void setModelStatistic(Model *model);

signals:

public slots:

private:
    Ui::ModelStatistic *ui;

};

#endif // OPENSTATISTIC_H
