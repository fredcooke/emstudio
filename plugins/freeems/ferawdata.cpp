#include "ferawdata.h"

FERawData::FERawData()
{
}
void FERawData::setData(unsigned short locationid,bool isflashonly,QByteArray payload)
{
	m_isFlashOnly = isflashonly;
	m_data = payload;
	m_locationId = locationid;
	emit update();
}
QByteArray FERawData::data()
{
	return m_data;
}

unsigned short FERawData::locationId()
{
	return m_locationId;
}

bool FERawData::isFlashOnly()
{
	return m_isFlashOnly;
}
