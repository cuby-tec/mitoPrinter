#ifndef AUTOLEVEL_H
#define AUTOLEVEL_H

#include <QWidget>

#include "links/status.h"
#include "ui_autolevel.h"

//Ui_StatusLabel
namespace Ui {
    class AutolevelWidget;
}


class AutolevelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AutolevelWidget(QWidget *parent = nullptr);

signals:

public slots:
    void statusFailed();
    void updateStatus(const Status_t *status);

private:
    Ui::AutolevelWidget *ui;

};

#endif // AUTOLEVEL_H
