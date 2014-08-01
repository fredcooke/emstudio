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

#ifndef TABLEVIEW3D_H
#define TABLEVIEW3D_H

#include <QWidget>
#include <QKeyEvent>
#include <QClipboard>
#include "table3ddata.h"
#include "ui_tableview3d.h"
#include "dataview.h"
#include "tablemap3d.h"
#include <QTimer>
class TableView3D : public DataView
{
	Q_OBJECT
	
public:
	explicit TableView3D(QWidget *parent = 0);
	~TableView3D();
	void setMetaData(Table3DMetaData metadata);
	bool setData(unsigned short locationid,DataBlock *data);
	bool setData(QString name,DataBlock *data);
	void passDatalog(QVariantMap data);
private:
	bool m_firstKeypress;
	QTimer *m_inputTimer;
	QList<QPair<int,int> > m_highlightItemList;
	int m_oldXLoc;
	int m_oldYLoc;
	TableMap3D *m_tableMap;
	bool m_isFlashOnly;
	bool metaDataValid;
	Table3DData *tableData;
	unsigned short m_locationId;
	QString m_currentValue;
	Ui::TableView3D ui;
	unsigned short m_xAxisSize;
	unsigned short m_yAxisSize;
	Table3DMetaData m_metaData;
	void setSilentValue(int row,int column,QString value);
	QString formatNumber(double num,int prec=2);
	int m_currRow;
	int m_currCol;
	void setValue(int row, int column,double value,bool ignoreselection = true);
	void setRange(QList<QPair<QPair<int,int>,double> > data);
	bool m_tracingEnabled;
	void writeTable(bool ram);
	QString verifyValue(int row,int column,QString item);
	QList<QPair<QPair<int,int>,double> > m_queuedValList;
protected:
	void contextMenuEvent(QContextMenuEvent *evt);
	void keyPressEvent(QKeyEvent *event);
public slots:
	bool updateTable();
private slots:
	void currentSelectionChanged(QList<QPair<int,int> > selectionList);
	void itemChangeRequest(int row,int column,QString text);
	void tracingCheckBoxStateChanged(int newstate);
	void tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn);
	void saveClicked();
	void loadClicked();
	void loadRamClicked();
	void exportClicked();
	void importClicked();
	void exportJson(QString filename);
	void showMapClicked();
	void hotKeyPressed(int key,Qt::KeyboardModifier modifier);
	void regularKeyPressed(int key);
	void inputTimerTimeout();

signals:
	void show3DTable(unsigned short locationid,Table3DData *data);
	void saveToFlash(unsigned short locationid);
	void reloadTableData(unsigned short locationid,bool ram);
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);

};

#endif // TABLEVIEW3D_H
