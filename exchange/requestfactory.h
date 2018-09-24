#ifndef REQUESTFACTORY_H
#define REQUESTFACTORY_H

#include "links/ComDataReq_t.h"
#include "gparcer/coordinatus.h"
#include "gparcer/structtag.h"

class RequestFactory
{
public:
    RequestFactory();

    void build(ComDataReq_t *comdata, eOrder order);
    void build(ComDataReq_t *comdata, eOrder order, NumberedTag *tag);

    ComDataReq_t *build(uint linenumber);

private:
    ComDataReq_t *request;




};

#endif // REQUESTFACTORY_H
