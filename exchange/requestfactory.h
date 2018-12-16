#ifndef REQUESTFACTORY_H
#define REQUESTFACTORY_H

#include "links/ComDataReq_t.h"
#include "gparcer/coordinatus.h"
#include "gparcer/structtag.h"

class RequestFactory
{
public:
    RequestFactory();

    void build(ComDataReq_t *comdata, eOrder order, void *data=nullptr);
    void build(ComDataReq_t *comdata, eOrder order, sHotendControl_t *hend_src);

    ComDataReq_t *build(uint linenumber);
    void buildTag92(struct ComDataReq_t* request, sMover* data);

private:
    ComDataReq_t *request;



};

#endif // REQUESTFACTORY_H
