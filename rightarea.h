#ifndef RIGHTAREA_H
#define RIGHTAREA_H

#include "ui_rightArea.h"

#include "thermoplot.h"

//#include "oneform.h"
//#include "secondform.h"


#include <QWidget>

namespace Ui{
class RightArea;
}

class RightArea : public QWidget
{
    Q_OBJECT
public:
    explicit RightArea(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;

signals:

public slots:
    void hide1(int state);
    void hide2(int state);


private:
    Ui::RightArea *ui;

    QCheckBox * check1;
    QCheckBox * check2;

    ThermoPlot *plotter;

//    OneForm *oneform;

//    SecondForm * secondForm;

};

#endif // RIGHTAREA_H
