#include "datafield.h"
#include <QDebug>
DataField::DataField()
{
}
DataField::DataField(QString shortname,QString description,int offset,int size,float div,float min,float max)
{
	m_offset = offset;
	m_size = size;
	m_div = div;
	m_name = shortname;
	m_description = description;
	m_min = min;
	m_max = max;
}
float DataField::getValue(QByteArray *payload)
{
	if (payload->size() > m_offset+m_size)
	{
		float val = 0;
		for (int i=0;i<m_size;i++)
		{
			val += ((unsigned char)payload->at(m_offset+i)) << (8*(m_size-(i+1)));
		}
		return val / m_div;
	}
	return 0;
}
