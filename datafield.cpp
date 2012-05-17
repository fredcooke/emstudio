/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "datafield.h"
#include <QDebug>
DataField::DataField()
{
}
DataField::DataField(QString shortname,QString description,int offset,int size,float div,float min,float max,float addoffset)
{
	m_offset = offset;
	m_size = size;
	m_div = div;
	m_name = shortname;
	m_description = description;
	m_min = min;
	m_max = max;
	m_addoffset = addoffset;
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
		return (val / m_div) + m_addoffset;
	}
	return 0;
}
