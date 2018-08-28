#include "workareadialog.h"
#include "ui_workareadialog.h"

WorkAreaDialog::WorkAreaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkAreaDialog)
{
    ui->setupUi(this);

    x_width = ui->lineEdit_xwidth;
    y_depth = ui->lineEdit_ydepth;
    z_height = ui->lineEdit_zheight;
}

WorkAreaDialog::~WorkAreaDialog()
{
    delete ui;
}

