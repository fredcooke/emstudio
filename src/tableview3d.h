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

#ifndef TABLEVIEW3D_H
#define TABLEVIEW3D_H

#include <QWidget>
#include "headers.h"
#include "table3ddata.h"
#include "ui_tableview3d.h"

class TableView3D : public QWidget
{
	Q_OBJECT
	
public:
	explicit TableView3D(QWidget *parent = 0);
	~TableView3D();
	void passData(unsigned short locationid,QByteArray data,int physicallocation,Table3DMetaData metadata);
	void passData(unsigned short locationid,QByteArray data,int physicallocation);
private:
	bool metaDataValid;
	Table3DData *tableData;
	unsigned short m_locationId;
	double  currentvalue;
	Ui::TableView3D ui;
	unsigned short m_xAxisSize;
	unsigned short m_yAxisSize;
	Table3DMetaData m_metaData;
	void setSilentValue(int row,int column,QString value);
	QString formatNumber(double num);
private slots:
	void tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn);
	void tableCellChanged(int row,int column);
	void saveClicked();
	void loadClicked();
signals:
	void saveToFlash(unsigned short locationid);
	void reloadTableData(unsigned short locationid);
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);

};

#endif // TABLEVIEW3D_H
