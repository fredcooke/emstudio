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

#ifndef FETABLE3DDATA_H
#define FETABLE3DDATA_H

#include <QObject>
#include <QMutex>

#include "table3ddata.h"
#include "table3dmetadata.h"

class FETable3DData : public Table3DData
{
	Q_OBJECT
public:
	FETable3DData();
	void setData(unsigned short locationid,bool isflashonly,QByteArray payload,Table3DMetaData metadata);
	void setData(unsigned short locationid,bool isflashonly,QByteArray payload);
	QByteArray data();
	QList<double> xAxis();
	QList<double> yAxis();
	QList<QList<double> > values();
	int columns();
	int rows();
	void setCell(int row, int column,double val);
	void setXAxis(int column,double val);
	void setYAxis(int row,double val);
	double maxActualXAxis();
	double maxActualYAxis();
	double maxActualValue();
	double minActualYAxis();
	double minActualXAxis();
	double minActualValue();
	double maxCalcedXAxis();
	double maxCalcedYAxis();
	double maxCalcedValue();
	double minCalcedXAxis();
	double minCalcedYAxis();
	double minCalcedValue();
	void setWritesEnabled(bool enabled);
	void writeWholeLocation(bool ram);
	bool isRam() { return !m_isFlashOnly; }
	double calcAxis(int val,QList<QPair<QString,double> > metadata);
	int backConvertAxis(double val,QList<QPair<QString,double> > metadata);
private:
	QMutex *m_acccessMutex;
	bool m_writesEnabled;
	bool m_isFlashOnly;
	unsigned short m_locationId;
	QList<double> m_xAxis;
	QList<double> m_yAxis;
	QList<QList<double> > m_values;
	QString xAxisLabel;
	QString yAxisLabel;
	QString valuesLabel;
	double m_maxCalcedXAxis;
	double m_maxCalcedYAxis;
	double m_maxCalcedValue;

	double m_minCalcedXAxis;
	double m_minCalcedYAxis;
	double m_minCalcedValue;

	double m_maxActualXAxis;
	double m_maxActualYAxis;
	double m_maxActualValue;

	double m_minActualXAxis;
	double m_minActualYAxis;
	double m_minActualValue;
	Table3DMetaData m_metaData;
signals:
	void saveSingleDataToFlash(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);
	void saveSingleDataToRam(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);
	void requestBlockFromRam(unsigned short locationid,unsigned short offset,unsigned short size);
	void requestBlockFromFlash(unsigned short locationid,unsigned short offset,unsigned short size);
	void update();
	void requestRamUpdateFromFlash(unsigned short locationid);
	void requestFlashUpdateFromRam(unsigned short locationid);
public slots:
	void updateFromFlash(); //Trigger a flash update, if table is in ram, it copies from flash to ram, and sends a request to the ECU
	void updateFromRam(); //Trigger a ram udpate. Requests table from the ECU from ram
	void saveRamToFlash();
};

#endif // FETABLE3DDATA_H
