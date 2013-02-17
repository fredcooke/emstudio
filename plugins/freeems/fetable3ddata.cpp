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

#include "fetable3ddata.h"
#include <QDebug>
FETable3DData::FETable3DData() : Table3DData()
{
	m_writesEnabled = true;
}
void FETable3DData::setData(unsigned short locationid,bool isflashonly, QByteArray data)
{
	m_isFlashOnly = isflashonly;
	Q_UNUSED(locationid)
	m_xAxis.clear();
	m_yAxis.clear();
	m_values.clear();
	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	qDebug() << "XAxis:" << xaxissize;
	qDebug() << "YAxis:" << yaxissize;


	m_maxXAxis = calcAxis(65535,m_metaData.xAxisCalc);
	m_maxYAxis = calcAxis(65535,m_metaData.yAxisCalc);
	m_maxZAxis = calcAxis(65535,m_metaData.zAxisCalc);

	m_minXAxis = calcAxis(0,m_metaData.xAxisCalc);
	m_minYAxis = calcAxis(0,m_metaData.yAxisCalc);
	m_minZAxis = calcAxis(0,m_metaData.zAxisCalc);

	for (int i=0;i<xaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i]) << 8) + (unsigned char)data[5+i];
		m_xAxis.append(calcAxis(val,m_metaData.xAxisCalc));
	}
	for (int i=0;i<yaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[58+i]) << 8) + (unsigned char)data[59+i];
		m_yAxis.append(calcAxis(val,m_metaData.yAxisCalc));
	}
	for (int y=0;y<yaxissize*2;y+=2)
	{
		QList<double> currrow;
		for (int x=0;x<xaxissize*2;x+=2)
		{
			unsigned short val = (((unsigned char)data[100 + x + (y * xaxissize)]) << 8) + (unsigned char)data[101 + x + (y * xaxissize)];
			currrow.append(calcAxis(val,m_metaData.zAxisCalc));
		}
		m_values.append(currrow);
	}
}

void FETable3DData::setData(unsigned short locationid,bool isflashonly,QByteArray data,Table3DMetaData metadata)
{
	m_locationId = locationid;
	m_metaData = metadata;
	m_isFlashOnly = isflashonly;
	setData(locationid,isflashonly,data);
}
double FETable3DData::maxXAxis()
{
	return m_maxXAxis;
}

double FETable3DData::maxYAxis()
{
	return m_maxYAxis;
}

double FETable3DData::maxZAxis()
{
	return m_maxZAxis;
}
double FETable3DData::minXAxis()
{
	return m_minXAxis;
}

double FETable3DData::minYAxis()
{
	return m_minYAxis;
}

double FETable3DData::minZAxis()
{
	return m_minZAxis;
}
void FETable3DData::setXAxis(int index,double val)
{
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.xAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleData(m_locationId,data,4+(index*2),2);
		}
		m_xAxis[index] = val;
	}
}
void FETable3DData::writeWholeLocation()
{
	emit saveSingleData(m_locationId,data(),0,data().size());
}

void FETable3DData::setYAxis(int index,double val)
{
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.yAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleData(m_locationId,data,58+(index*2),2);
		}
		m_yAxis[index] = val;
	}
}

void FETable3DData::setCell(int yIndex, int xIndex,double val)
{
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.zAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	//qDebug() << "Attempting to save data at:" << yIndex << xIndex << val;
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleData(m_locationId,data,100+(xIndex*2)+(yIndex * (m_xAxis.size()*2)),2);
		}
		m_values[yIndex][xIndex] = val;
	}
}
void FETable3DData::setWritesEnabled(bool enabled)
{
	m_writesEnabled = enabled;
}

QByteArray FETable3DData::data()
{
	QByteArray data;

	unsigned short xaxissize = m_xAxis.size();
	data.append((char)((xaxissize>> 8) & 0xFF));
	data.append((char)(xaxissize & 0xFF));

	unsigned short yaxissize = m_yAxis.size();
	data.append((char)((yaxissize>> 8) & 0xFF));
	data.append((char)(yaxissize & 0xFF));
	for (int i=0;i<m_xAxis.size();i++)
	{
		unsigned short val = backConvertAxis(m_xAxis[i],m_metaData.xAxisCalc);
		data.append((char)((val>> 8) & 0xFF));
		data.append((char)(val & 0xFF));
	}
	for (int i=data.size();i<58;i++)
	{
		data.append((char)0x00);
	}
	for (int i=0;i<m_yAxis.size();i++)
	{
		unsigned short val = backConvertAxis(m_yAxis[i],m_metaData.yAxisCalc);
		data.append((char)((val>> 8) & 0xFF));
		data.append((char)(val & 0xFF));
	}
	for (int i=data.size();i<100;i++)
	{
		data.append((char)0x00);
	}
	for (int i=0;i<m_values.size();i++)
	{
		for (int j=0;j<m_values[i].size();j++)
		{
			unsigned short val = backConvertAxis(m_values[i][j],m_metaData.zAxisCalc);
			data.append((char)((val>> 8) & 0xFF));
			data.append((char)(val & 0xFF));
		}
	}
	for (int i=data.size();i<1024;i++)
	{
		data.append((char)0x00);
	}
	return data;
}

QList<double> FETable3DData::yAxis()
{
	return m_yAxis;
}

QList<double> FETable3DData::xAxis()
{
	return m_xAxis;
}

QList<QList<double> > FETable3DData::values()
{
	return m_values;
}

int FETable3DData::columns()
{
	return m_xAxis.size();
}

int FETable3DData::rows()
{
	return m_yAxis.size();
}
