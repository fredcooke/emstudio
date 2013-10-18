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
	m_acccessMutex = new QMutex();
}
void FETable2DData::writeWholeLocation(bool ram)
{
	if (ram)
	{
		emit saveSingleDataToRam(m_locationId,0,data().size(),data());
	}
	else
	{
		emit saveSingleDataToFlash(m_locationId,0,data().size(),data());
	}
}
void FETable2DData::setWritesEnabled(bool enabled)
{
	m_writesEnabled = enabled;
}
void FETable2DData::reCalcAxisData()
{
	//QMutexLocker locker(m_acccessMutex);
/*		m_axis.append(xdouble);
		m_values.append(ydouble);*/
	m_minActualXAxis = calcAxis(65535,m_metaData.xAxisCalc);
	m_minActualYAxis = calcAxis(65535,m_metaData.yAxisCalc);
	m_maxActualXAxis = calcAxis(-65535,m_metaData.xAxisCalc);
	m_maxActualYAxis = calcAxis(-65535,m_metaData.yAxisCalc);
	for (int i=0;i<m_axis.size();i++)
	{
		if (m_axis[i] > m_maxActualXAxis)
		{
			m_maxActualXAxis = m_axis[i];
		}
		if (m_axis[i] < m_minActualXAxis)
		{
			m_minActualXAxis = m_axis[i];
		}

	}
	for (int i=0;i<m_values.size();i++)
	{

		if (m_values[i] > m_maxActualYAxis)
		{
			m_maxActualYAxis = m_values[i];
		}
		if (m_values[i] < m_minActualYAxis)
		{
			m_minActualYAxis = m_values[i];
		}
	}
}

void FETable2DData::setData(unsigned short locationid, bool isflashonly,QByteArray payload,Table2DMetaData metadata,bool signedData)
{
	m_acccessMutex->lock();
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
	m_acccessMutex->unlock();
	emit update();
}
double FETable2DData::maxActualXAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxActualXAxis;
}

double FETable2DData::maxActualYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxActualYAxis;
}

double FETable2DData::minActualXAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minActualXAxis;
}

double FETable2DData::minActualYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minActualYAxis;
}
double FETable2DData::maxCalcedXAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxCalcedXAxis;
}

double FETable2DData::maxCalcedYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxCalcedYAxis;
}

double FETable2DData::minCalcedXAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minCalcedXAxis;
}

double FETable2DData::minCalcedYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minCalcedYAxis;
}

QList<double> FETable2DData::axis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_axis;
}

QList<double> FETable2DData::values()
{
	QMutexLocker locker(m_acccessMutex);
	return m_values;
}
int FETable2DData::columns()
{
	QMutexLocker locker(m_acccessMutex);
	return m_axis.size();
}

int FETable2DData::rows()
{
	QMutexLocker locker(m_acccessMutex);
	return 2;
}

void FETable2DData::setCell(int row, int column,double newval)
{
	QMutexLocker locker(m_acccessMutex);
	//New value has been accepted. Let's write it.
	//offset = column + (row * 32), size == 2
	//QLOG_DEBUG() << "Update:" << row << column << newval;
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
	reCalcAxisData();
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			if (m_metaData.valid)
			{
				emit saveSingleDataToRam(m_locationId,(column*2)+(row * (m_metaData.size / 2.0)),2,data);
			}
			else
			{
				emit saveSingleDataToRam(m_locationId,(column*2)+(row * (m_dataSize / 2.0)),2,data);
			}
		}
	}
}

QByteArray FETable2DData::data()
{
	QMutexLocker locker(m_acccessMutex);
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
double FETable2DData::calcAxis(int val,QList<QPair<QString,double> > metadata)
{
    if (metadata.size() == 0)
    {
        return val;
    }
    double newval = val;
    for (int j=0;j<metadata.size();j++)
    {
        if (metadata[j].first == "add")
        {
            newval += metadata[j].second;
        }
        else if (metadata[j].first == "sub")
        {
            newval -= metadata[j].second;
        }
        else if (metadata[j].first == "mult")
        {
            newval *= metadata[j].second;
        }
        else if (metadata[j].first == "div")
        {
            newval /= metadata[j].second;
        }
    }
    return newval;
}
int FETable2DData::backConvertAxis(double val,QList<QPair<QString,double> > metadata)
{
    if (metadata.size() == 0)
    {
        return val;
    }
    double newval = val;
    for (int j=metadata.size()-1;j>=0;j--)
    {
        if (metadata[j].first == "add")
        {
            newval -= metadata[j].second;
        }
        else if (metadata[j].first == "sub")
        {
            newval += metadata[j].second;
        }
        else if (metadata[j].first == "mult")
        {
            newval /= metadata[j].second;
        }
        else if (metadata[j].first == "div")
        {
            newval *= metadata[j].second;
        }
    }
    return (unsigned short)newval;
}
void FETable2DData::updateFromFlash()
{
	//emit requestBlockFromFlash(m_locationId,0,0);
	emit requestRamUpdateFromFlash(m_locationId);
}

void FETable2DData::updateFromRam()
{
	//emit requestRamUpdateFromFlash(m_locationId);
	emit requestBlockFromRam(m_locationId,0,0);
}
void FETable2DData::saveRamToFlash()
{
	emit requestFlashUpdateFromRam(m_locationId);
}
