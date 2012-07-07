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

#include "table2ddata.h"

Table2DData::Table2DData() : QObject()
{

}

Table2DData::Table2DData(unsigned short locationid,QByteArray payload) : QObject()
{
	setData(locationid,payload);
}

void Table2DData::setData(unsigned short locationid, QByteArray payload)
{
	m_locationId = locationid;
	for (int i=0;i<payload.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)payload[i]) << 8) + ((unsigned char)payload[i+1]);
		unsigned short y = (((unsigned char)payload[(payload.size()/2)+ i]) << 8) + ((unsigned char)payload[(payload.size()/2) + i+1]);
		m_axis.append(x);
		m_values.append(y);
	}
}
QList<unsigned short> Table2DData::axis()
{
	return m_axis;
}

QList<unsigned short> Table2DData::values()
{
	return m_values;
}
int Table2DData::columns()
{
	return m_axis.size();
}

int Table2DData::rows()
{
	return 2;
}

void Table2DData::setCell(int row, int column,unsigned short newval)
{
	//New value has been accepted. Let's write it.
	//void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	//Data is 64
	//offset = column + (row * 32), size == 2
	if (row == 0)
	{
		m_axis.replace(column,newval);
	}
	else if (row == 1)
	{
		m_values.replace(column,newval);
	}
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,(column*2)+(row * 32),2);
}

QByteArray Table2DData::data()
{
	QByteArray data;
	for (int i=0;i<m_axis.size();i++)
	{
		data.append((char)((m_axis[i] >> 8) & 0xFF));
		data.append((char)(m_axis[i] & 0xFF));
	}
	for (int i=0;i<m_values.size();i++)
	{
		data.append((char)((m_values[i] >> 8) & 0xFF));
		data.append((char)(m_values[i] & 0xFF));
	}
	return data;
}
