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

#ifndef TABLEVIEW2D_H
#define TABLEVIEW2D_H

#include <QWidget>
#include <QResizeEvent>
#include <table2ddata.h>
#include "ui_tableview2d.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "headers.h"

class TableView2D : public QWidget
{
	Q_OBJECT
	
public:
	explicit TableView2D(QWidget *parent = 0);
	~TableView2D();
	void passData(unsigned short locationid,QByteArray data,int physicallocation,Table2DMetaData metadata);
	//void passData(unsigned short locationid,Table2DData data);
private:
	Table2DData *tableData;
	void setSilentValue(int row,int column,QString value);
	Table2DMetaData m_metaData;
	QwtPlotCurve *curve;
	QVector<QPointF> samples;
	Ui::TableView2D ui;
	unsigned short m_locationid;
	int m_physicalid;
	double currentvalue;
protected:
	void resizeEvent(QResizeEvent *evt);
private slots:
	void saveClicked();
	void loadClicked();
	void tableCellChanged(int row,int column);
	void tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn);
signals:
	void saveToFlash(unsigned short locationid);
	void saveData(unsigned short locationid,QByteArray data,int phyiscallocation);
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	void reloadTableData(unsigned short locationid);
};

#endif // TABLEVIEW2D_H
