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
void FERawData::updateValue(QByteArray data)
{
	m_data = data;
	emit update();
	if (m_isFlashOnly)
	{
		emit saveSingleDataToFlash(m_locationId,0,m_data.size(),m_data);
	}
	else
	{
		emit saveSingleDataToRam(m_locationId,0,m_data.size(),m_data);

	}
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
