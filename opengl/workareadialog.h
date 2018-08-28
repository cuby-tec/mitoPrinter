#ifndef WORKAREADIALOG_H
#define WORKAREADIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class WorkAreaDialog;
}

class WorkAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WorkAreaDialog(QWidget *parent = nullptr);
    ~WorkAreaDialog();

      QLineEdit* x_width;
      QLineEdit* y_depth;
      QLineEdit* z_height;

private:
    Ui::WorkAreaDialog *ui;
};

#endif // WORKAREADIALOG_H
