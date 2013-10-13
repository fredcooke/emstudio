#ifndef FERAWDATA_H
#define FERAWDATA_H

#include "rawdata.h"

class FERawData : public RawData
{
	Q_OBJECT
public:
	FERawData();
	void setData(unsigned short locationid,bool isflashonly,QByteArray payload);
	QByteArray data();
	unsigned short locationId();
	bool isFlashOnly();
	void updateValue(QByteArray data);
private:
	QByteArray m_data;
	bool m_isFlashOnly;
	unsigned short m_locationId;
signals:
	void update();
	void saveSingleDataToFlash(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);
	void saveSingleDataToRam(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);
	void requestBlockFromRam(unsigned short locationid,unsigned short offset,unsigned short size);
	void requestBlockFromFlash(unsigned short locationid,unsigned short offset,unsigned short size);
public slots:
	void updateFromFlash(); //Trigger a flash update, if table is in ram, it copies from flash to ram, and sends a request to the ECU
	void updateFromRam(); //Trigger a ram udpate. Requests table from the ECU from ram
	void saveRamToFlash();
};

#endif // FERAWDATA_H
