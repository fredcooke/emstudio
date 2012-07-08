/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

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
void Table3DData::setXAxis(int index,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,4+(index*2),2);
}

void Table3DData::setYAxis(int index,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,58+(index*2),2);
}

void Table3DData::setCell(int yIndex, int xIndex,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	qDebug() << "Attempting to save data at:" << yIndex << xIndex;
	emit saveSingleData(m_locationId,data,100+(xIndex*2)+(yIndex * (m_xAxis.size()*2)),2);
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
