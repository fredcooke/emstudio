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

#include "fetable2ddata.h"
#include <QDebug>
FETable2DData::FETable2DData() : Table2DData()
{
	m_writesEnabled = true;
}
void FETable2DData::writeWholeLocation()
{
	emit saveSingleData(m_locationId,data(),0,data().size());
}
void FETable2DData::setWritesEnabled(bool enabled)
{
	m_writesEnabled = enabled;
}
void FETable2DData::setData(unsigned short locationid, bool isflashonly,QByteArray payload,Table2DMetaData metadata)
{
	m_isFlashOnly = isflashonly;
	m_metaData = metadata;
	m_maxXAxis = calcAxis(65535,metadata.xAxisCalc);
	m_maxYAxis = calcAxis(65535,metadata.yAxisCalc);
	m_minXAxis = calcAxis(0,metadata.xAxisCalc);
	m_minYAxis = calcAxis(0,metadata.yAxisCalc);
	m_locationId = locationid;

	for (int i=0;i<payload.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)payload[i]) << 8) + ((unsigned char)payload[i+1]);
		unsigned short y = (((unsigned char)payload[(payload.size()/2)+ i]) << 8) + ((unsigned char)payload[(payload.size()/2) + i+1]);
		m_axis.append(calcAxis(x,metadata.xAxisCalc));
		m_values.append(calcAxis(y,metadata.yAxisCalc));
	}
}
double FETable2DData::maxXAxis()
{
	return m_maxXAxis;
}

double FETable2DData::maxYAxis()
{
	return m_maxYAxis;
}

double FETable2DData::minXAxis()
{
	return m_minXAxis;
}

double FETable2DData::minYAxis()
{
	return m_minYAxis;
}

QList<double> FETable2DData::axis()
{
	return m_axis;
}

QList<double> FETable2DData::values()
{
	return m_values;
}
int FETable2DData::columns()
{
	return m_axis.size();
}

int FETable2DData::rows()
{
	return 2;
}

void FETable2DData::setCell(int row, int column,double newval)
{
	//New value has been accepted. Let's write it.
	//void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	//Data is 64
	//offset = column + (row * 32), size == 2
	qDebug() << "Update:" << row << column << newval;
	unsigned short val = 0;
	if (row == 0)
	{
		val = backConvertAxis(newval,m_metaData.xAxisCalc);
		m_axis.replace(column,newval);
	}
	else if (row == 1)
	{

		val = backConvertAxis(newval,m_metaData.yAxisCalc);
		m_values.replace(column,newval);
	}
	QByteArray data;
	data.append((char)((val >> 8) & 0xFF));
	data.append((char)(val & 0xFF));
	//qDebug() << "Attempting to save data at:" << yIndex << xIndex;
	//emit saveSingleData(m_locationId,data,100+(xIndex*2)+(yIndex * (m_xAxis.size()*2)),2);

	//QByteArray data;
	//data.append((char)((newval >> 8) & 0xFF));
	//data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleData(m_locationId,data,(column*2)+(row * (m_metaData.size / 2.0)),2);
		}
	}
}

QByteArray FETable2DData::data()
{
	QByteArray data;
	for (int i=0;i<m_axis.size();i++)
	{
		unsigned short val = backConvertAxis(m_axis[i],m_metaData.xAxisCalc);
		data.append((char)((val >> 8) & 0xFF));
		data.append((char)(val & 0xFF));
	}
	for (int i=0;i<m_values.size();i++)
	{
		unsigned short val = backConvertAxis(m_values[i],m_metaData.yAxisCalc);
		data.append((char)((val >> 8) & 0xFF));
		data.append((char)(val & 0xFF));
	}
	return data;
}
