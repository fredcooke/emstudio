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

#ifndef CONFIGBLOCK_H
#define CONFIGBLOCK_H
#include <QString>
#include <QPair>
#include <QList>
class ConfigBlock
{
public:
	ConfigBlock();
	ConfigBlock(QString name,QString type, unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc);
	void setName(QString name) { m_name = name; }
	void setType(QString type) { m_type = type; }
	void setLocationId(unsigned short locationid) { m_locationId = locationid; }
	void setSize(unsigned short size) { m_size = size; }
	void setElementSize(unsigned short size) { m_elementSize = size; }
	void setOffset(unsigned short offset) { m_offset = offset; }
	void setCalc(QList<QPair<QString,double> > calc) { m_calc = calc; }
	QString name() { return m_name; }
	QString type() { return m_type; }
	unsigned short locationId() { return m_locationId; }
	unsigned short offset() { return m_offset; }
	unsigned short size() { return m_size; }
	unsigned short elementSize() { return m_elementSize; }
	QList<QPair<QString,double> > calc() { return m_calc; }
private:
	QString m_name;
	QString m_type;
	unsigned short m_locationId;
	unsigned short m_size;
	unsigned short m_elementSize;
	unsigned short m_offset;
	QList<QPair<QString,double> > m_calc;


};

#endif // CONFIGBLOCK_H
