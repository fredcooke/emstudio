#ifndef RAWDATA_H
#define RAWDATA_H
#include "tabledata.h"
#include <QObject>

class RawData : public DataBlock
{
	Q_OBJECT
public:
	virtual void setData(unsigned short locationid,bool isflashonly,QByteArray payload) = 0;
	virtual QByteArray data() = 0;
	virtual unsigned short locationId()=0;
	virtual bool isFlashOnly()=0;
};
#endif // RAWDATA_H
