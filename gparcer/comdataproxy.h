#ifndef COMDATAPROXY_H
#define COMDATAPROXY_H

#include "araytag.h"
#include "coordinatus.h"
//#include "lexer.h"
#include "mitoaction.h"
#include "step_motor/controller.h"

#include <QObject>

class ComdataProxy : public QObject
{
    Q_OBJECT
public:
    explicit ComdataProxy(QObject *parent = nullptr);

    virtual ~ComdataProxy(){}

    void sendG0Line(sG0_t *data);
    void sendG1Line(sG1_t *data);
    void sendG2Line(sG2_t *data);
    void sendG3Line(sG3_t *data);
    void sendG3Tag(sG4_t *data);
    void sendG6Tag(sG6_t *data);
    void sendG10Tag(sG10_t *data);
    void sendG20_21Tag(sG20_21_t *data);
    void sendG28Tag(sG28_t *data);
    void sendG29_1Tag(sG29_1_t *data);
    void sendG29_2Tag(sG29_2_t *data);
    void sendG30Tag(sG30_t * data);
    void sendG90_Tag(sG90_t *data);
    void sendG91_Tag(sG90_t * data);
    void sendM82_Tag(sM82_t * data);
    void sendM84_Tag(sM84_t * data);
    void sendM106_Tag(sM106_t * data);
    void sendM107_Tag(sM106_t * data);
    void sendM109_Tag(sM109_t * data);
    void sendM104Tag(sM104_t *data);

    void sendG92Tag(sG92_t * data);

    uint getLine_counter() const;

signals:

public slots:

private:
    uint line_counter;// lines counter

    Coordinatus *coordinatus;

    Controller* controller;

    bool isPlaneHasSteps();

    ComDataReq_t *buildComdata(uint linenumber);

};

#endif // COMDATAPROXY_H
