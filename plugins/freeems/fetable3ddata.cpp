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

#include "fetable3ddata.h"
#include "QsLog.h"

FETable3DData::FETable3DData() : Table3DData()
{
	m_writesEnabled = true;
	m_acccessMutex = new QMutex();
}
void FETable3DData::setData(unsigned short locationid,bool isflashonly, QByteArray data)
{
	m_acccessMutex->lock();
	Q_UNUSED(locationid)
	m_isFlashOnly = isflashonly;
	m_xAxis.clear();
	m_yAxis.clear();
	m_values.clear();
	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	QLOG_DEBUG() << "XAxis:" << xaxissize;
	QLOG_DEBUG() << "YAxis:" << yaxissize;


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
	m_acccessMutex->unlock();
	emit update();
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
	QMutexLocker locker(m_acccessMutex);
	return m_maxXAxis;
}

double FETable3DData::maxYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxYAxis;
}

double FETable3DData::maxZAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_maxZAxis;
}
double FETable3DData::minXAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minXAxis;
}

double FETable3DData::minYAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minYAxis;
}

double FETable3DData::minZAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_minZAxis;
}
void FETable3DData::setXAxis(int index,double val)
{
	QMutexLocker locker(m_acccessMutex);
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.xAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleDataToRam(m_locationId,4+(index*2),2,data);
		}
		m_xAxis[index] = val;
	}
}
void FETable3DData::writeWholeLocation(bool ram)
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

void FETable3DData::setYAxis(int index,double val)
{
	QMutexLocker locker(m_acccessMutex);
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.yAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleDataToRam(m_locationId,58+(index*2),2,data);
		}
		m_yAxis[index] = val;
	}
}

void FETable3DData::setCell(int yIndex, int xIndex,double val)
{
	QMutexLocker locker(m_acccessMutex);
	QByteArray data;
	unsigned short newval = backConvertAxis(val,m_metaData.zAxisCalc);
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	if (!m_isFlashOnly)
	{
		if (m_writesEnabled)
		{
			emit saveSingleDataToRam(m_locationId,100+(xIndex*2)+(yIndex * (m_xAxis.size()*2)),2,data);
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
	QMutexLocker locker(m_acccessMutex);
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
	QMutexLocker locker(m_acccessMutex);
	return m_yAxis;
}

QList<double> FETable3DData::xAxis()
{
	QMutexLocker locker(m_acccessMutex);
	return m_xAxis;
}

QList<QList<double> > FETable3DData::values()
{
	QMutexLocker locker(m_acccessMutex);
	return m_values;
}

int FETable3DData::columns()
{
	QMutexLocker locker(m_acccessMutex);
	return m_xAxis.size();
}

int FETable3DData::rows()
{
	QMutexLocker locker(m_acccessMutex);
	return m_yAxis.size();
}
double FETable3DData::calcAxis(int val,QList<QPair<QString,double> > metadata)
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
int FETable3DData::backConvertAxis(double val,QList<QPair<QString,double> > metadata)
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
void FETable3DData::updateFromFlash()
{
	//emit requestBlockFromFlash(m_locationId,0,0);
	emit requestRamUpdateFromFlash(m_locationId);
}

void FETable3DData::updateFromRam()
{
	//emit requestRamUpdateFromFlash(m_locationId);
	emit requestBlockFromRam(m_locationId,0,0);
}
void FETable3DData::saveRamToFlash()
{
	emit requestFlashUpdateFromRam(m_locationId);
}
