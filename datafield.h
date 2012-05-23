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

#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>
class DataField
{
public:
	DataField();
	DataField(QString shortname,QString description,int offset,int size,float div,float min=0,float max=0,float addoffset=0,bool isFlags=false,int bit=0);
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
