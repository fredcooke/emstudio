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

#include "memorylocation.h"
#include <QDebug>
MemoryLocation::MemoryLocation()
{
	m_parent = 0;
	parent=0;
	hasParent = false;
	isEmpty = true;
}
void MemoryLocation::addChild(MemoryLocation *child)
{
	m_childList.append(child);
}
void MemoryLocation::setParent(MemoryLocation *parent)
{
	m_parent = parent;
}
void MemoryLocation::childChanged(MemoryLocation *child,QByteArray data)
{
	unsigned short childinparent=0;
	if (isRam)
	{
		childinparent = child->ramAddress - this->ramAddress;
	}
	else
	{
		childinparent = child->flashAddress - this->flashAddress;
	}
	if (data.size() != child->size)
	{
		qDebug() << "Error. Child tried to replace memory location not equal to its own size! Data size: " << data.size() << "Child size:" << child->size;
	}
	m_data.replace(childinparent,data.length(),data);
}
QByteArray MemoryLocation::data(MemoryLocation *child)
{
	unsigned short childinparent=0;
	if (isRam)
	{
		childinparent = child->ramAddress - this->ramAddress;
	}
	else
	{
		childinparent = child->flashAddress - this->flashAddress;
	}
	return m_data.mid(childinparent,child->size);
}

QByteArray MemoryLocation::data()
{
	if (m_parent)
	{
		return m_parent->data(this);
	}
	else
	{
		return m_data;
	}
}

void MemoryLocation::setData(QByteArray data)
{
	if (m_parent)
	{
		m_parent->childChanged(this,data);
	}
	else
	{
		m_data = data;
	}
	isEmpty=false;
}
