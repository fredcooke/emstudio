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

#ifndef FETABLE2DDATA_H
#define FETABLE2DDATA_H
#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include "table2ddata.h"
#include "headers.h"
class FETable2DData : public Table2DData
{
	Q_OBJECT
public:
	FETable2DData();
	//FETable2DData(unsigned short locationid,bool isflashonly,QByteArray payload,Table2DMetaData metadata);
	void setData(unsigned short locationid,bool isflashonly, QByteArray payload,Table2DMetaData metadata,bool signedData);
	QByteArray data();
	double maxActualXAxis();
	double maxActualYAxis();
	double minActualXAxis();
	double minActualYAxis();
	double maxCalcedXAxis();
	double maxCalcedYAxis();
	double minCalcedXAxis();
	double minCalcedYAxis();

	QList<double> axis();
	QList<double> values();
	int columns();
	int rows();
	void setCell(int row, int column,double newval);
	void setWritesEnabled(bool enabled);
	void writeWholeLocation();
private:
	unsigned short m_dataSize;
	bool m_isSignedData;
	bool m_isFlashOnly;
	bool m_writesEnabled;
	double m_maxActualXAxis;
	double m_maxActualYAxis;
	double m_minActualXAxis;
	double m_minActualYAxis;
	double m_maxCalcedXAxis;
	double m_maxCalcedYAxis;
	double m_minCalcedXAxis;
	double m_minCalcedYAxis;

	Table2DMetaData m_metaData;
	unsigned short m_locationId;
	QList<double> m_axis;
	QList<double> m_values;
	QString axisLabel;
	QString valuesLabel;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
};

#endif // FETABLE2DDATA_H
