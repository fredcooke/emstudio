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

#ifndef TABLE3DDATA_H
#define TABLE3DDATA_H

#include <QObject>
#include "headers.h"
#include "tabledata.h"
class Table3DData : public TableData
{
	Q_OBJECT
public:
	Table3DData(unsigned short locationid, QByteArray data,Table3DMetaData metadata);
	void setData(unsigned short locationid,QByteArray payload,Table3DMetaData metadata);
	QByteArray data();
	QList<double> xAxis();
	QList<double> yAxis();
	QList<QList<double> > values();
	int columns();
	int rows();
	void setCell(int row, int column,double val);
	void setXAxis(int column,double val);
	void setYAxis(int row,double val);
	double maxXAxis();
	double maxYAxis();
	double maxZAxis();
	double minXAxis();
	double minYAxis();
	double minZAxis();
private:
	unsigned short m_locationId;
	QList<double> m_xAxis;
	QList<double> m_yAxis;
	QList<QList<double> > m_values;
	QString xAxisLabel;
	QString yAxisLabel;
	QString valuesLabel;
	double m_maxXAxis;
	double m_maxYAxis;
	double m_maxZAxis;
	double m_minXAxis;
	double m_minYAxis;
	double m_minZAxis;
	Table3DMetaData m_metaData;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
public slots:
	
};

#endif // TABLE3DDATA_H
