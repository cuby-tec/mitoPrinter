#ifndef EXCHANGE_H
#define EXCHANGE_H

//#include <stdio.h>

//#include from FeeRTOS project.
#include "links/status.h"
#include "links/ComDataReq_t.h"

#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <cstdio>

#include <QMutex>

#include "profiles/profile.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE    -1
#endif

#define EXIT_SUCCESS    0

static uint requestIndex = 0;

class Exchange
{
public:

    static Exchange* instance()
    {
        static Exchange exchange;
        return &exchange;
    }

    void buildComData(struct ComDataReq_t* comdata);

    void buildComData(ComDataReq_t *comdata, eOrder order);

    /**
     * params:
     * 	psc - data to be filled.
     * 	i - offset for line number.
     */
    void build_segment_default(struct sSegment* psc, uint32_t i);

    int sendRequest(ComDataReq_t* request);

    void NoOperation();

    Status_t* getStatus() { return c_status; }

    QMutex mutex;


private:

    Exchange(){}

    ~Exchange(){}

    QFile *fp;

    int sendBuffer(uint8_t* buffer, uint32_t size, QFile* fp);

    int sendBuffer(uint8_t* buffer, uint32_t size, std::FILE* fp);

    void buildProfile(sProfile *sprofile_dst);

    ComDataReq_t comdata;

    Status_t* c_status;


    void print_status(Status_t* c_status);

    void load_defaults(struct sControl* pctl);

    Exchange(Exchange const&) = delete;
    void operator =(Exchange const&) = delete;


};



#endif // EXCHANGE_H
