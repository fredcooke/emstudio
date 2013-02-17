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

#ifndef TABLE2DDATA_H
#define TABLE2DDATA_H
#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include "tabledata.h"
#include "headers.h"
class Table2DData : public TableData
{
	Q_OBJECT
public:
//	virtual Table2DData() = 0;
//	virtual Table2DData(unsigned short locationid,bool isflashonly,QByteArray payload,Table2DMetaData metadata) = 0;
	virtual void setData(unsigned short locationid,bool isflashonly,QByteArray payload,Table2DMetaData metadata) = 0;
	virtual QByteArray data() = 0;
	virtual double maxXAxis() = 0;
	virtual double maxYAxis() = 0;
	virtual double minXAxis() = 0;
	virtual double minYAxis() = 0;
	virtual QList<double> axis() = 0;
	virtual QList<double> values() = 0;
	virtual int columns() = 0;
	virtual int rows() = 0;
	virtual void setCell(int row, int column,double newval) = 0;
	virtual void setWritesEnabled(bool enabled) = 0;
	virtual void writeWholeLocation() = 0;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
};

#endif // TABLE2DDATA_H
