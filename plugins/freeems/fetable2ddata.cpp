/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

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
void FETable2DData::setData(unsigned short locationid, bool isflashonly,QByteArray payload,Table2DMetaData metadata,bool signedData)
{
	m_dataSize = payload.size();
	m_isSignedData = signedData;
	m_isFlashOnly = isflashonly;
	m_metaData = metadata;
	m_maxCalcedXAxis = calcAxis(65535,metadata.xAxisCalc);
	m_maxCalcedYAxis = calcAxis(65535,metadata.yAxisCalc);
	m_minCalcedXAxis = calcAxis(-65535,metadata.xAxisCalc);
	m_minCalcedYAxis = calcAxis(-65535,metadata.yAxisCalc);

	//Reverse the min and max, so we can figure them out based on real data
	m_minActualXAxis = calcAxis(65535,metadata.xAxisCalc);
	m_minActualYAxis = calcAxis(65535,metadata.yAxisCalc);
	m_maxActualXAxis = calcAxis(-65535,metadata.xAxisCalc);
	m_maxActualYAxis = calcAxis(-65535,metadata.yAxisCalc);
	m_locationId = locationid;
	m_axis.clear();
	m_values.clear();

	for (int i=0;i<payload.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)payload[i]) << 8) + ((unsigned char)payload[i+1]);
		unsigned short y = (((unsigned char)payload[(payload.size()/2)+ i]) << 8) + ((unsigned char)payload[(payload.size()/2) + i+1]);
		double xdouble = 0;
		double ydouble = 0;
		if (signedData)
		{
			xdouble = calcAxis((short)x,metadata.xAxisCalc);
			ydouble = calcAxis((short)y,metadata.yAxisCalc);
		}
		else
		{
			xdouble = calcAxis(x,metadata.xAxisCalc);
			ydouble = calcAxis(y,metadata.yAxisCalc);
		}
		if (xdouble > m_maxActualXAxis)
		{
			m_maxActualXAxis = xdouble;
		}
		if (xdouble < m_minActualXAxis)
		{
			m_minActualXAxis = xdouble;
		}

		if (ydouble > m_maxActualYAxis)
		{
			m_maxActualYAxis = ydouble;
		}
		if (ydouble < m_minActualYAxis)
		{
			m_minActualYAxis = ydouble;
		}

		m_axis.append(xdouble);
		m_values.append(ydouble);
	}
	}
double FETable2DData::maxActualXAxis()
{
	return m_maxActualXAxis;
}

double FETable2DData::maxActualYAxis()
{
	return m_maxActualYAxis;
}

double FETable2DData::minActualXAxis()
{
	return m_minActualXAxis;
}

double FETable2DData::minActualYAxis()
{
	return m_minActualYAxis;
}
double FETable2DData::maxCalcedXAxis()
{
	return m_maxCalcedXAxis;
}

double FETable2DData::maxCalcedYAxis()
{
	return m_maxCalcedYAxis;
}

double FETable2DData::minCalcedXAxis()
{
	return m_minCalcedXAxis;
}

double FETable2DData::minCalcedYAxis()
{
	return m_minCalcedYAxis;
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
	short val = 0;
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
	/*		if (signedData)
		{
			xdouble = calcAxis((short)x,metadata.xAxisCalc);
			ydouble = calcAxis((short)y,metadata.yAxisCalc);
		}
		else
		{
			xdouble = calcAxis(x,metadata.xAxisCalc);
			ydouble = calcAxis(y,metadata.yAxisCalc);
		}
*/
	QByteArray data;
	if (m_isSignedData)
	{
		data.append((char)((val >> 8) & 0xFF));
		data.append((char)(val & 0xFF));
	}
	else
	{
		data.append((char)((((unsigned short)val) >> 8) & 0xFF));
		data.append((char)(((unsigned short)val) & 0xFF));
	}
	//qDebug() << "Attempting to save data at:" << yIndex << xIndex;
	//emit saveSingleData(m_locationId,data,100+(xIndex*2)+(yIndex * (m_xAxis.size()*2)),2);

	//QByteArray data;
	//data.append((char)((newval >> 8) & 0xFF));
	//data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			if (m_metaData.valid)
			{
				emit saveSingleData(m_locationId,data,(column*2)+(row * (m_metaData.size / 2.0)),2);
			}
			else
			{
				emit saveSingleData(m_locationId,data,(column*2)+(row * (m_dataSize / 2.0)),2);
			}
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
