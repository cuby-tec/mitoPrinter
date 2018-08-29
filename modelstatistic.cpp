#include "modelstatistic.h"

ModelStatistic::ModelStatistic(QWidget *parent) : QWidget(parent)
  , ui(new Ui::ModelStatistic)
{
    ui->setupUi(parent);
}

void ModelStatistic::setModelStatistic(Model *model)
{
    ui->label_pointsValue->setText(QString("%1").arg(model->points()));
    ui->label_edgesValue->setText(QString("%1").arg(model->edges()));
    ui->label_facesValue->setText(QString("%1").arg(model->faces()));
    ui->label_modelValue->setText(QString("%1").arg(model->fileName()));

    qDebug()<<__FILE__<<__LINE__<<model->fileName();
}
