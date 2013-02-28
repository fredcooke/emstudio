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

#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>
class DataField
{
public:
	DataField();
	DataField(QString shortname,QString description,int offset,int size,double div,double addoffset=0,double min=0,double max=0,bool isFlags=false,int bit=0);
	float getValue(QByteArray *payload);
	QString description() { return m_description; }
	QString name() { return m_name; }
	bool isFlag() { return m_isFlags; }
	bool flagValue(QByteArray *payload);
private:
	QString m_name;
	QString m_description;
	bool m_isFlags;
	int m_bit;
	int m_offset;
	int m_size;
	float m_div;
	float m_addoffset;
	float m_min;
	float m_max;
};

#endif // DATAFIELD_H
