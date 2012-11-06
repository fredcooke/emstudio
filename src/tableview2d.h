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
#include <QKeyEvent>
#include <QClipboard>
#include <table2ddata.h>
#include "ui_tableview2d.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "headers.h"
#include "dataview.h"
class TableView2D : public DataView
{
	Q_OBJECT
	
public:
	explicit TableView2D(bool isram, bool isflash,QWidget *parent = 0);
	~TableView2D();
	bool setData(unsigned short locationid,QByteArray data);
	bool setData(unsigned short locationid,QByteArray data,Table2DMetaData metadata);
	//void passData(unsigned short locationid,Table2DData data);
	void passDatalog(QVariantMap data);
private:
	bool m_isFlashOnly;
	bool metaDataValid;
	Table2DData *tableData;
	void setSilentValue(int row,int column,QString value);
	Table2DMetaData m_metaData;
	QwtPlotCurve *curve;
	QVector<QPointF> samples;
	Ui::TableView2D ui;
	unsigned short m_locationid;
	int m_physicalid;
	double currentvalue;
	void resizeColumnWidths();
	int m_currRow;
	int m_currCol;
	void exportJson(QString filename);
	void setValue(int row, int column,double value);
protected:
	void resizeEvent(QResizeEvent *evt);
	void keyPressEvent(QKeyEvent *event);
private slots:
	void exportClicked();
	void saveClicked();
	void loadFlashClicked();
	void loadRamClicked();
	void tableCellChanged(int row,int column);
	void tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn);
	void hotKeyPressed(int key,Qt::KeyboardModifiers modifier);
signals:
	void saveToFlash(unsigned short locationid);
	void saveData(unsigned short locationid,QByteArray data,int phyiscallocation);
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	void reloadTableData(unsigned short locationid,bool ram);
};

#endif // TABLEVIEW2D_H
