#include "table3ddata.h"
#include <QDebug>
Table3DData::Table3DData(unsigned short locationid, QByteArray data) : QObject()
{
	setData(locationid,data);
}
void Table3DData::setData(unsigned short locationid,QByteArray data)
{
	m_locationId = locationid;

	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	qDebug() << "XAxis:" << xaxissize;
	qDebug() << "YAxis:" << yaxissize;


	for (int i=0;i<xaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i]) << 8) + (unsigned char)data[5+i];
		m_xAxis.append(val);
	}
	for (int i=0;i<yaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[58+i]) << 8) + (unsigned char)data[59+i];
		m_yAxis.append(val);
	}
	for (int y=0;y<yaxissize*2;y+=2)
	{
		QList<unsigned short> currrow;
		for (int x=0;x<xaxissize*2;x+=2)
		{
			unsigned short val = (((unsigned char)data[100 + x + (y * xaxissize)]) << 8) + (unsigned char)data[101 + x + (y * xaxissize)];
			currrow.append(val);
		}
		m_values.append(currrow);
	}
}
void Table3DData::setXAxis(int column,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,4+(((m_xAxis.size()) - column)*2),2);
}

void Table3DData::setYAxis(int row,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,58+((m_xAxis.size() - row)*2),2);
}

void Table3DData::setCell(int row, int column,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	qDebug() << "Attempting to save data at:" << row << column;
	emit saveSingleData(m_locationId,data,100+((column)*2)+(row * (m_xAxis.size()*2)),2);
}
QByteArray Table3DData::data()
{
	return QByteArray();
}

QList<unsigned short> Table3DData::yAxis()
{
	return m_yAxis;
}

QList<unsigned short> Table3DData::xAxis()
{
	return m_xAxis;
}

QList<QList<unsigned short> > Table3DData::values()
{
	return m_values;
}

int Table3DData::columns()
{
	return m_xAxis.size();
}

int Table3DData::rows()
{
	return m_yAxis.size();
}
