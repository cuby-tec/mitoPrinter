/**
  Только обмен даными с Контроллером(Сервером).
 */


#ifndef UUSBEXCHANGE_H
#define UUSBEXCHANGE_H

#include <QObject>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <cstdio>

#include <QDebug>
#include <QMessageBox>

#include <QMutex>

#include "links/status.h"
#include "links/ComDataReq_t.h"
#include "profiles/profile.h"

#include <myglobal.h>

#define VERSIONUSB 2

extern uint rIndex;

class UsbExchange : public QObject
{
    Q_OBJECT

public:
    //explicit Base(QObject *parent = 0);
     explicit UsbExchange(QObject * parent = nullptr);

    ~UsbExchange(){}

#if VERSIONUSB==1
    void buildComData(struct ComDataReq_t* comdata);
     void buildComData(ComDataReq_t *comdata, eOrder order);
     /**
      * params:
      * 	psc - data to be filled.
      * 	i - offset for line number.
      */
     void build_segment_default(struct sSegment* psc, uint32_t i);
#endif

     int sendRequest(ComDataReq_t* request);

     void NoOperation();

     Status_t* getStatus() { return c_status; }
#if  VERSIONUSB==1
     void print_status(Status_t* c_status);
#endif

signals:
     void sg_statusReceived(struct Status_t* status);


private:


     QFile *fp;

      int sendBuffer(uint8_t* buffer, uint32_t size, QFile* fp);

      int sendBuffer(uint8_t* buffer, size_t size, std::FILE* fp);

      void buildProfile(sProfile *sprofile_dst);

      ComDataReq_t comdata;

      Status_t* c_status;

      char buffer2[64];

#if VERSIONUSB==1
      void load_defaults(struct sControl* pctl);
#endif

};

#endif // UUSBEXCHANGE_H
