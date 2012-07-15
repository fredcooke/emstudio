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
	Table2DData();
	Table2DData(unsigned short locationid,QByteArray payload,Table2DMetaData metadata);
	void setData(unsigned short locationid,QByteArray payload,Table2DMetaData metadata);
	//QByteArray data();
	double maxXAxis();
	double maxYAxis();
	double minXAxis();
	double minYAxis();
	QList<double> axis();
	QList<double> values();
	int columns();
	int rows();
	void setCell(int row, int column,double newval);
private:
	double m_maxXAxis;
	double m_maxYAxis;
	double m_minXAxis;
	double m_minYAxis;

	Table2DMetaData m_metaData;
	unsigned short m_locationId;
	QList<double> m_axis;
	QList<double> m_values;
	QString axisLabel;
	QString valuesLabel;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
};

#endif // TABLE2DDATA_H
