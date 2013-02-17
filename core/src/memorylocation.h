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

#ifndef MEMORYLOCATION_H
#define MEMORYLOCATION_H

#include <QObject>

class MemoryLocation
{
public:
	explicit MemoryLocation();
	bool isFlash;
	bool isRam;
	bool hasParent;
	int type;
	unsigned short parent;
	unsigned short ramPage;
	unsigned short ramAddress;
	unsigned short flashPage;
	unsigned short flashAddress;
	unsigned short size;
	unsigned short locationid;
	void addChild(MemoryLocation *child);
	void setParent(MemoryLocation *parent);
	MemoryLocation *getParent() { return m_parent; }
	void setData(QByteArray data);
	bool isEmpty();
	void childChanged(MemoryLocation *child,QByteArray data);
	QByteArray data(MemoryLocation *child);
	QByteArray data();
private:
	QByteArray m_data;
	QList<MemoryLocation*> m_childList;
	MemoryLocation *m_parent;
signals:
	
public slots:
	
};

#endif // MEMORYLOCATION_H
