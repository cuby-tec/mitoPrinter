#ifndef COMDATAPROXY_H
#define COMDATAPROXY_H

#include "araytag.h"
#include "coordinatus.h"
//#include "lexer.h"
#include "mitoaction.h"
#include "step_motor/controller.h"
#include "exchange/requestfactory.h"

#include <QObject>

class ComdataProxy : public QObject
{
    Q_OBJECT
public:
    explicit ComdataProxy(QObject *parent = nullptr);

    virtual ~ComdataProxy(){}

    mito::Action_t *sendG0Line(sG0_t *data);
    mito::Action_t* sendG1Line(sG1_t *data);
    mito::Action_t *sendG2Line(sG2_t *data);
    mito::Action_t *sendG3Line(sG3_t *data);
    void sendG4Tag(sG4_t *data);
    void sendG6Tag(sG6_t *data);
    void sendG10Tag(sG10_t *data);
    mito::Action_t *sendG20_21Tag(sG20_21_t *data);
    mito::Action_t *sendG28Tag(sG28_t *data);
    void sendG29_1Tag(sG29_1_t *data);
    void sendG29_2Tag(sG29_2_t *data);
    void sendG30Tag(sG30_t * data);
    mito::Action_t *sendG90_Tag(sG90_t *data);
    mito::Action_t *sendM82_Tag(sM82_t * data);
    mito::Action_t* sendM84_Tag(sM84_t * data);
    mito::Action_t* sendM106_Tag(sM106_t * data);
    mito::Action_t* sendM107_Tag(sM106_t * data);
    mito::Action_t* sendM109_Tag(sM109_t * data);
    mito::Action_t *sendM104Tag(sM104_t *data);
    mito::Action_t* sendM140_Tag(sM140_t* data);
    mito::Action_t* sendM190_Tag(sM190_t* data);

//    mito::Action_t *sendG92Tag(sG92_t * data);
    mito::Action_t *sendG92Tag(sMover * data);

    uint getLine_counter() const;

signals:

public slots:

private:
    uint line_counter;// lines counter

    Coordinatus *coordinatus;

    Controller* controller;

    bool isPlaneHasSteps();

    ComDataReq_t *buildComdata(uint linenumber);

    RequestFactory* factory;

    void loadHotendFromProfile(sHotendControl_t* shc);

    void loadBedFromProfile(sBedControl_t* shc);

    void _setMicrosteps(ComDataReq_t* req);


};

#endif // COMDATAPROXY_H
