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

#include "tableview3d.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include "QsLog.h"

//#include <freeems/fetable3ddata.h>
#include "tablewidgetdelegate.h"
TableView3D::TableView3D(QWidget *parent)
{
	Q_UNUSED(parent)
	m_isFlashOnly = false;
	m_tableMap=0;
	ui.setupUi(this);
	tableData=0;
	m_tracingEnabled = false;
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadFlashPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
	connect(ui.loadRamPushButton,SIGNAL(clicked()),this,SLOT(loadRamClicked()));
	connect(ui.exportPushButton,SIGNAL(clicked()),this,SLOT(exportClicked()));
	connect(ui.importPushButton,SIGNAL(clicked()),this,SLOT(importClicked()));
	connect(ui.tableWidget,SIGNAL(hotKeyPressed(int,Qt::KeyboardModifiers)),this,SLOT(hotKeyPressed(int,Qt::KeyboardModifiers)));
	connect(ui.tracingCheckBox,SIGNAL(stateChanged(int)),this,SLOT(tracingCheckBoxStateChanged(int)));
	ui.tableWidget->addHotkey(Qt::Key_Plus,Qt::ShiftModifier);
	ui.tableWidget->addHotkey(Qt::Key_Minus,Qt::NoModifier);
	ui.tableWidget->addHotkey(Qt::Key_Underscore,Qt::ShiftModifier);
	ui.tableWidget->addHotkey(Qt::Key_Equal,Qt::NoModifier);
	ui.tableWidget->setItemDelegate(new TableWidgetDelegate());

	setContextMenuPolicy(Qt::DefaultContextMenu);
	//QAction* fooAction = new QAction("foo",this);
	//QAction* barAction = new QAction("bar",this);
	//connect(fooAction, SIGNAL(triggered()), this, SLOT(doSomethingFoo()));
	//connect(barAction, SIGNAL(triggered()), this, SLOT(doSomethingBar()));
	//addAction(fooAction);
	//addAction(barAction);
	metaDataValid = true;
	/*if (!isram)
	{
		//Is only flash
		ui.loadRamPushButton->setVisible(false);
	}
	else if (!isflash)
	{
		//Is only ram
		ui.savePushButton->setVisible(false);
		ui.loadFlashPushButton->setVisible(false);
	}
	else
	{
		//Is both ram and flash
	}*/
	//ui.importPushButton->setVisible(false);
	connect(ui.showMapPushButton,SIGNAL(clicked()),this,SLOT(showMapClicked()));
}
void TableView3D::tracingCheckBoxStateChanged(int newstate)
{
	if (newstate == Qt::Checked)
	{
		m_tracingEnabled = true;
	}
	else
	{
		m_tracingEnabled = false;
		ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
		ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
		for (int i=0;i<m_highlightItemList.size();i++)
		{
			ui.tableWidget->item(m_highlightItemList[i].first,m_highlightItemList[i].second)->setTextColor(QColor::fromRgb(0,0,0));
			ui.tableWidget->item(m_highlightItemList[i].first,m_highlightItemList[i].second)->setData(Qt::UserRole+1,false);
		}
		m_highlightItemList.clear();
		connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
		connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	}
}

void TableView3D::setValue(int row, int column,double value,bool ignoreselection)
{
	if (row == -1 || column == -1)
	{
		QLOG_ERROR() << "Negative array index! Should be unreachable code! FIXME!";
		return;
	}
	if (row >= ui.tableWidget->rowCount() || column >= ui.tableWidget->columnCount())
	{
		QLOG_ERROR() << "Larger than life, should be unreachable code! FIXME!";
		return;
	}
	// Ignore bottom right corner if the disallow on editing fails
	if (row == ui.tableWidget->rowCount()-1 && column == 0)
	{
		QLOG_ERROR() << "This should not happen! Bottom right corner ignored!";
		return;
	}
	//bool conversionOk = false; // Note, value of this is irrelevant, overwritten during call in either case, but throws a compiler error if not set.
	//double tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);
	double tempValue = value;
	double oldValue = tempValue;
	/*if (!conversionOk)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		setSilentValue(row,column,formatNumber(currentvalue));
		//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}*/

	//ui.tableWidget->item(row,column)->setText(QString::number(tempValue,'f',2));
	//tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);

	//New value has been accepted. Let's write it.

	//If the value is an axis, and there are multiple cells selected, cancel the edit!
	if (ui.tableWidget->selectedItems().size() > 1 && !ignoreselection)
	{
		for (int i=0;i<ui.tableWidget->selectedItems().size();i++)
		{
			if (ui.tableWidget->selectedItems()[i]->row() == ui.tableWidget->rowCount()-1)
			{
				QLOG_ERROR() << "XAxis edit attempted with multiple cells selected. This is not allowed";
				QMessageBox::information(0,"Error","Editing of multiple AXIS cells at once is not allowed. Please select a single axis cell to edit at a time");
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.xDp));
				return;
			}
			if (ui.tableWidget->selectedItems()[i]->column() == 0)
			{
				QLOG_ERROR() << "YAxis edit attempted with multiple cells selected. This is not allowed";
				QMessageBox::information(0,"Error","Editing of multiple AXIS cells at once is not allowed. Please select a single axis cell to edit at a time");
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.xDp));
				return;
			}
		}
		setSilentValue(row,column,formatNumber(currentvalue,m_metaData.zDp)); //Reset the value, setRange will set it properly.
		//currentvalue = oldValue;
		QLOG_DEBUG() << "Setting all cells to" << currentvalue << "for" << row << column;
		QList<QPair<QPair<int,int>,double> > vallist;
		for (int i=0;i<ui.tableWidget->selectedItems().size();i++)
		{
			QPair<QPair<int,int>,double> val;
			val.first = QPair<int,int>(ui.tableWidget->selectedItems()[i]->row(),ui.tableWidget->selectedItems()[i]->column());
			val.second = tempValue;
			vallist.append(val);
		}
		setRange(vallist);
	}
	else
	{
		if (row == ui.tableWidget->rowCount()-1)
		{
			setSilentValue(row,column,formatNumber(tempValue,m_metaData.xDp));
			if (tempValue > tableData->maxXAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minXAxis()) + "-" + QString::number(tableData->maxXAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.xDp));
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				return;
			}
			else if (tempValue < tableData->minXAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minXAxis()) + "-" + QString::number(tableData->maxXAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.xDp));
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				return;
			}

			currentvalue = oldValue;
			tableData->setXAxis(column-1,currentvalue);
		}
		else if (column == 0)
		{
			setSilentValue(row,column,formatNumber(tempValue,m_metaData.yDp));
			if (tempValue > tableData->maxYAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minYAxis()) + "-" + QString::number(tableData->maxYAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.yDp));
				return;
			}
			else if (tempValue < tableData->minYAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minYAxis()) + "-" + QString::number(tableData->maxYAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.yDp));
				return;
			}
			currentvalue = oldValue;
			tableData->setYAxis(ui.tableWidget->rowCount()-(row+2),currentvalue);
		}
		else
		{
			setSilentValue(row,column,formatNumber(tempValue,m_metaData.zDp));
			if (tempValue > tableData->maxZAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minZAxis()) + "-" + QString::number(tableData->maxZAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.zDp));
				return;
			}
			if (tempValue < tableData->minZAxis())
			{
				QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minZAxis()) + "-" + QString::number(tableData->maxZAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
				//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
				setSilentValue(row,column,formatNumber(currentvalue,m_metaData.zDp));
				return;
			}
			currentvalue = oldValue;
			tableData->setCell(ui.tableWidget->rowCount()-(row+2),column-1,currentvalue);
		}
		reColorTable(row,column);
	}
	//ui.tableWidget->resizeColumnsToContents();
	resizeColumnWidths();


}

void TableView3D::hotKeyPressed(int key,Qt::KeyboardModifiers modifier)
{
	if (key == Qt::Key_Plus || key == Qt::Key_Equal)
	{
		if (ui.tableWidget->selectedItems().size() == 0)
		{
			return;
		}
		QList<QPair<QPair<int,int>,double> > vallist;
		for (int i=0;i<ui.tableWidget->selectedItems().size();i++)
		{
			QString text = ui.tableWidget->selectedItems()[i]->text();
			double textd = text.toDouble();
			double maxval = 0;
			if (ui.tableWidget->selectedItems()[i]->row() == ui.tableWidget->rowCount()-1)
			{
				maxval = tableData->maxXAxis();
			}
			else if (ui.tableWidget->selectedItems()[i]->column() == 0)
			{
				maxval = tableData->maxYAxis();
			}
			else
			{
				maxval = tableData->maxZAxis();
			}
			if (modifier & Qt::ShiftModifier)
			{
				textd += maxval / 10;
			}
			else
			{
				textd += maxval / 100;
			}

			if (textd > maxval)
			{
				textd = maxval;
			}
			QPair<QPair<int,int>,double> val;
			val.first = QPair<int,int>(ui.tableWidget->selectedItems()[i]->row(),ui.tableWidget->selectedItems()[i]->column());
			val.second = textd;
			vallist.append(val);
			//else if (column == 0) Y
			//else Z

			//ui.tableWidget->selectedItems()[0]->setText(formatNumber(textd,m_metaData.zDp));
			//setValue(ui.tableWidget->selectedItems()[i]->row(),ui.tableWidget->selectedItems()[i]->column(),textd);

		}
		setRange(vallist);
	}
	else if (key == Qt::Key_Minus || key == Qt::Key_Underscore)
	{
		if (ui.tableWidget->selectedItems().size() == 0)
		{
			return;
		}
		QList<QPair<QPair<int,int>,double> > vallist;
		for (int i=0;i<ui.tableWidget->selectedItems().size();i++)
		{
			QString text = ui.tableWidget->selectedItems()[i]->text();
			double textd = text.toDouble();
			double minval = 0;
			double maxval = 0;
			if (ui.tableWidget->selectedItems()[i]->row() == ui.tableWidget->rowCount()-1)
			{
				maxval = tableData->maxXAxis();
				minval = tableData->minXAxis();
			}
			else if (ui.tableWidget->selectedItems()[i]->column() == 0)
			{
				maxval = tableData->maxYAxis();
				minval = tableData->minYAxis();
			}
			else
			{
				maxval = tableData->maxZAxis();
				minval = tableData->minZAxis();
			}
			if (modifier & Qt::ShiftModifier)
			{
				textd -= maxval / 10;
			}
			else
			{
				textd -= maxval / 100;
			}
			if (textd < minval)
			{
				textd = minval;
			}
			QPair<QPair<int,int>,double> val;
			val.first = QPair<int,int>(ui.tableWidget->selectedItems()[i]->row(),ui.tableWidget->selectedItems()[i]->column());
			val.second = textd;
			vallist.append(val);

			//ui.tableWidget->selectedItems()[0]->setText(formatNumber(textd,m_metaData.zDp));
			//setValue(ui.tableWidget->selectedItems()[0]->row(),ui.tableWidget->selectedItems()[0]->column(),textd);
		}
		setRange(vallist);
	}
}

void TableView3D::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
	{
		//Copy
		if (ui.tableWidget->selectedItems().size() == 0)
		{
			return;
		}
		QByteArray itembytes;
		//int row = ui.tableWidget->selectedItems()[0]->row();
		QMap<int,QMap<int,QString> > tablemap;
		int maxrow = 0;
		int maxcolumn = 0;
		int minrow = 255;
		int mincolumn = 255;
		foreach(QTableWidgetItem* item,ui.tableWidget->selectedItems())
		{
			tablemap[item->row()][item->column()] = item->text();
			if (item->row() > maxrow) maxrow = item->row();
			if (item->column() > maxcolumn) maxcolumn = item->column();
			if (item->row() < minrow) minrow = item->row();
			if (item->column() < mincolumn) mincolumn = item->column();
		}
		for (int i=minrow;i<=maxrow;i++)
		{
			for (int j=mincolumn;j<=maxcolumn;j++)
			{
				if (tablemap.contains(i))
				{
					if (tablemap[i].contains(j))
					{
						itembytes.append(tablemap[i][j]);
						itembytes.append("\t");
					}
					else
					{
						itembytes.append("\t");
					}
				}
				else
				{
					//itembytes.append("\n");
				}
			}
			itembytes = itembytes.mid(0,itembytes.length()-1);
			itembytes.append("\n");
		}

		QMimeData * mime = new QMimeData();
		mime->setData("text/plain",itembytes);
		QApplication::clipboard()->setMimeData(mime);
	}
	else if(event->key() == Qt::Key_V && event->modifiers() & Qt::ControlModifier)
	{
		//Paste
		if (ui.tableWidget->selectedItems().size() == 0)
		{
			//Can't paste when we don't know where to paste!
			return;
		}
		if (ui.tableWidget->selectedItems().size() != 1)
		{
			QMessageBox::information(0,"Error","Pasting to a selection group is not supported yet. Please select the top left cell you wish to paste from");
			return;
		}
		int rowindex = ui.tableWidget->selectedItems()[0]->row();
		int columnindex = ui.tableWidget->selectedItems()[0]->column();
		int newrow = 0;
		int newcolumn = 0;
		QByteArray data = QApplication::clipboard()->mimeData()->data("text/plain");
		QString datastring(data);
		QStringList datastringsplit = datastring.split("\n");

		//Check to make sure we're in-bounds first
		if (datastringsplit.size() + rowindex > ui.tableWidget->rowCount()+1)
		{
			QMessageBox::information(0,"Error","Attempted to paste a block that does not fit!");
			return;
		}
		foreach(QString line,datastringsplit)
		{
			if (line.split("\t").size() + columnindex > ui.tableWidget->columnCount()+1)
			{
				QMessageBox::information(0,"Error","Attempted to paste a block that does not fit!");
				return;
			}
		}

		//Disable signals, so we can write the table all at once
		ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
		ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
		//bool valid = true;
		QMap<int,QMap<int,QString> > tmpvaluemap;
		QStringList invalidreasons;
		QList<QPair<QPair<int,int>,double> > valuelist;
		//setRange
		foreach(QString line,datastringsplit)
		{
			QStringList linesplit = line.split("\t");
			foreach (QString item,linesplit)
			{
				if (item != "")
				{
					if (!tmpvaluemap.contains(rowindex+newrow))
					{
						tmpvaluemap[rowindex+newrow] = QMap<int,QString>();
					}
					//newvallist.append(item.toDouble());
					QPair<QPair<int,int>,double> val;
					val.first = QPair<int,int>(rowindex+newrow,columnindex+newcolumn);
					val.second = item.toDouble();
					valuelist.append(val);

					tmpvaluemap[rowindex+newrow][columnindex+newcolumn] = ui.tableWidget->item(rowindex+newrow,columnindex+newcolumn)->text();
					QString verifystr = verifyValue(rowindex+newrow,columnindex+newcolumn,item);
					if (verifystr != "GOOD")
					{
						invalidreasons.append(verifystr);
						//valid = false;
					}
					//ui.tableWidget->item(rowindex+newrow,columnindex+newcolumn)->setText(item);
				}
				newcolumn++;
			}
			newcolumn=0;
			newrow++;
		}
		setRange(valuelist);
		/*
		//Write the values, and re-enable the signals.
		if (valid)
		{
			writeTable(true);
		}
		else
		{
			for (QMap<int,QMap<int,QString> >::const_iterator i=tmpvaluemap.constBegin();i!=tmpvaluemap.constEnd();i++)
			{
				for (QMap<int,QString>::const_iterator j=i.value().constBegin();j!=i.value().constEnd();j++)
				{
					ui.tableWidget->item(i.key(),j.key())->setText(j.value());
				}
			}
			QString errorstr = "";
			foreach(QString reason,invalidreasons)
			{
				errorstr += reason + ",";
			}

			QMessageBox::information(0,"Error",errorstr);
		}*/
		connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
		connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	}
}
QString TableView3D::verifyValue(int row,int column,QString item)
{
	if (row == ui.tableWidget->rowCount()-1 && column == 0)
	{
		QLOG_ERROR() << "This should not happen! Bottom right corner ignored!";
		return "INVALID INDEX";
	}
	double tempValue = item.toDouble();

	if (row == ui.tableWidget->rowCount()-1)
	{
		if (tempValue > tableData->maxXAxis())
		{
			return "Axis value entered too large!";
		}
		else if (tempValue < tableData->minXAxis())
		{
			return "Axis value entered too small";
		}
	}
	else if (column == 0)
	{
		if (tempValue > tableData->maxYAxis())
		{
			return "Axis value too large";
		}
		else if (tempValue < tableData->minYAxis())
		{
			return  "Axis value too small";
		}
	}
	else
	{
		if (tempValue > tableData->maxZAxis())
		{
			return "Value too large for table";
		}
		if (tempValue < tableData->minZAxis())
		{
			return "Value too small for table";
		}
	}
	return "GOOD";
}
//Data is arranged as a
void TableView3D::setRange(QList<QPair<QPair<int,int>,double> > data)
{
	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
	QMap<int,QMap<int,QString> > tmpvaluemap;
	bool valid = true;
	QList<QString> invalidreasons;
	for (int i=0;i<data.size();i++)
	{
		if (!tmpvaluemap.contains(data[i].first.first))
		{
			tmpvaluemap[data[i].first.first] = QMap<int,QString>();
		}
		tmpvaluemap[data[i].first.first][data[i].first.second] = ui.tableWidget->item(data[i].first.first,data[i].first.second)->text();
		QString formatstr = formatNumber(data[i].second,m_metaData.zDp);
		QString verifystr = verifyValue(data[i].first.first,data[i].first.second,formatstr);
		if (verifystr != "GOOD")
		{
			invalidreasons.append(verifystr);
			valid = false;
		}
		ui.tableWidget->item(data[i].first.first,data[i].first.second)->setText(formatstr);
	}
	//Write the values, and re-enable the signals.
	if (valid)
	{
		writeTable(true);
		if (data.size() == 1)
		{
			reColorTable(data[0].first.first,data[0].first.second);
		}
		else
		{
			reColorTable(-1,-1);
		}
	}
	else
	{
		for (QMap<int,QMap<int,QString> >::const_iterator i=tmpvaluemap.constBegin();i!=tmpvaluemap.constEnd();i++)
		{
			for (QMap<int,QString>::const_iterator j=i.value().constBegin();j!=i.value().constEnd();j++)
			{
				ui.tableWidget->item(i.key(),j.key())->setText(j.value());
			}
		}
		QString errorstr = "";
		foreach(QString reason,invalidreasons)
		{
			errorstr += reason + ",";
		}
		QMessageBox::information(0,"Error",errorstr);
	}
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
}

void TableView3D::writeTable(bool ram)
{
	if (ram)
	{
		tableData->setWritesEnabled(false);
		for (int i=0;i<ui.tableWidget->rowCount();i++)
		{
			for (int j=0;j<ui.tableWidget->columnCount();j++)
			{
				if (i == ui.tableWidget->rowCount()-1)
				{
					if (j != 0)
					{
						//X Axis
						tableData->setXAxis(j-1,ui.tableWidget->item((ui.tableWidget->rowCount()-1),j)->text().toDouble());
					}
				}
				else if (j == 0)
				{
					//Y axis
					tableData->setYAxis(i,ui.tableWidget->item((ui.tableWidget->rowCount()-2)-i,j)->text().toDouble());
				}
				else
				{
					tableData->setCell(i,j-1,ui.tableWidget->item((ui.tableWidget->rowCount()-2)-i,/*(ui.tableWidget->columnCount())-*/j)->text().toDouble());
				}
			}
		}
		tableData->writeWholeLocation(ram);
		tableData->setWritesEnabled(true);
	}
}

void TableView3D::showMapClicked()
{
	emit show3DTable(m_locationId,tableData);
	if (m_tableMap)
	{
//		m_tableMap->show();
	}
	else
	{
		/*m_tableMap = new TableMap3D();
		m_tableMap->setGeometry(100,100,800,600);
		m_tableMap->show();
		m_tableMap->passData(tableData);
		emit show3DTable(m_locationId,m_tableData);*/
	}
}

void TableView3D::contextMenuEvent(QContextMenuEvent *evt)
{
	QMenu menu(this);
	menu.addAction("Test");
	menu.addAction("Second");
	menu.setGeometry(evt->x(),evt->y(),menu.width(),menu.height());
	menu.show();
}

void TableView3D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	Q_UNUSED(prevrow)
	Q_UNUSED(prevcolumn)
	if (currentrow == -1 || currentcolumn == -1 || !ui.tableWidget->item(currentrow,currentcolumn))
	{
		return;
	}
	m_currRow = currentrow;
	m_currCol = currentcolumn;
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toDouble();
}
void TableView3D::exportJson(QString filename)
{
	//Create a JSON file similar to MTX's yaml format.
	QVariantMap topmap;
	QVariantMap x;
	QVariantMap y;
	QVariantMap z;
	QVariantList xlist;
	QVariantList ylist;
	QVariantList zlist;

	topmap["3DTable"] = "";
	topmap["type"] = "3D";
	topmap["title"] = m_metaData.tableTitle;
	topmap["description"] = m_metaData.tableTitle;
	x["unit"] = m_metaData.xAxisTitle;
	x["label"] = m_metaData.xAxisTitle;
	y["unit"] = m_metaData.yAxisTitle;
	y["label"] = m_metaData.yAxisTitle;
	z["unit"] = m_metaData.zAxisTitle;
	z["label"] = m_metaData.zAxisTitle;

	for (int i=1;i<ui.tableWidget->columnCount();i++)
	{
		//Reformat the number to be XXXX.XX to make Fred happy.
		double val = ui.tableWidget->item(ui.tableWidget->rowCount()-1,i)->text().toDouble();
		xlist.append(QString::number(val,'f',2));
	}
	for (int i=0;i<ui.tableWidget->rowCount()-1;i++)
	{
		//Reformat the number to be XXXX.XX to make Fred happy.
		double val = ui.tableWidget->item(i,0)->text().toDouble();
		ylist.append(QString::number(val,'f',2));
	}
	for (int j=0;j<ui.tableWidget->rowCount()-1;j++)
	{
		QVariantList zrow;
		for (int i=1;i<ui.tableWidget->columnCount();i++)
		{
			zrow.append(ui.tableWidget->item(j,i)->text());
		}
		zlist.append((QVariant)zrow);
	}

	y["values"] = ylist;
	x["values"] = xlist;
	z["values"] = zlist;
	topmap["X"] = x;
	topmap["Y"] = y;
	topmap["Z"] = z;

	QJson::Serializer serializer;
	QByteArray serialized = serializer.serialize(topmap);

	//TODO: Open a message box and allow the user to select where they want to save the file.
	QFile file(filename);
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		QLOG_ERROR() << "Unable to open file to output JSON!";
		return;
	}
	file.write(serialized);
	file.close();
}
void TableView3D::importClicked()
{
	QString filename = QFileDialog::getOpenFileName(this,"Load Json File",".","Json Files (*.json)");
	if (filename == "")
	{
		return;
	}
	//Create a JSON file similar to MTX's yaml format.

	//QByteArray serialized = serializer.serialize(topmap);

	//TODO: Open a message box and allow the user to select where they want to save the file.
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		QLOG_ERROR() << "Unable to open file to output JSON!";
		QMessageBox::information(0,"Error","Unable to open JSON file:" + file.errorString());
		return;
	}
	QByteArray toparsebytes = file.readAll();
	file.close();
	QJson::Parser parser;
	bool ok = false;
	QVariant topvar = parser.parse(toparsebytes,&ok);
	if (!ok)
	{
		QLOG_ERROR() << "Unable to parse import json";
		QMessageBox::information(0,"Error","Unable to parse JSON. Error: " +  parser.errorString());
		return;
	}

	QVariantMap topmap = topvar.toMap();
	QVariantMap x = topmap["X"].toMap();
	QVariantMap y = topmap["Y"].toMap();
	QVariantMap z = topmap["Z"].toMap();
	QVariantList xlist = x["values"].toList();
	QVariantList ylist = y["values"].toList();
	QVariantList zlist = z["values"].toList();
	QString type = topmap["type"].toString();
	QString title = topmap["title"].toString();
	QString description = topmap["description"].toString();

	/*m_metaData.tableTitle = title;
	m_metaData.tableTitle = description;
	m_metaData.xAxisTitle = x["unit"].toString();
	m_metaData.xAxisTitle = x["label"].toString();
	m_metaData.yAxisTitle = y["unit"].toString();
	m_metaData.yAxisTitle = y["label"].toString();
	m_metaData.zAxisTitle = z["unit"].toString();
	m_metaData.zAxisTitle = z["label"].toString();*/

	if (xlist.size() != ui.tableWidget->columnCount()-1)
	{
		//Error here, wrong number of columns
		QMessageBox::information(0,"Error","Unable to load JSON file. File had " + QString::number(xlist.size()) + " columns of axis data, but table has " + QString::number(ui.tableWidget->columnCount()-1) + " columns of axis data");
		return;
	}
	if (ylist.size() != ui.tableWidget->rowCount()-1)
	{
		//Error here, wrong number of rows!
		QMessageBox::information(0,"Error","Unable to load JSON file. File had " + QString::number(ylist.size()) + " rows of axis data, but table has " + QString::number(ui.tableWidget->rowCount()-1) + " rows of axis data");
		return;
	}
	if (zlist.size() != ui.tableWidget->rowCount()-1)
	{
		QMessageBox::information(0,"Error","Unable to load JSON file. File had " + QString::number(zlist.size()) + " rows of data, but table has " + QString::number(ui.tableWidget->rowCount()-1) + " rows of data");
		return;
	}
	for (int i=0;i<zlist.size();i++)
	{
		if (zlist[i].toList().size() != ui.tableWidget->columnCount()-1)
		{
			QMessageBox::information(0,"Error","Unable to load JSON file. File had " + QString::number(zlist[i].toList().size()) + " columns of data, but table has " + QString::number(ui.tableWidget->columnCount()-1) + " columns of data");
			return;
		}

	}

	for (int i=1;i<ui.tableWidget->columnCount();i++)
	{
		setSilentValue(ui.tableWidget->rowCount()-1,i,xlist[i-1].toString());
	}
	for (int i=0;i<ui.tableWidget->rowCount()-1;i++)
	{
		setSilentValue(i,0,ylist[i].toString());
	}
	for (int j=0;j<ui.tableWidget->rowCount()-1;j++)
	{
		QVariantList zrow = zlist[j].toList();
		for (int i=1;i<ui.tableWidget->columnCount();i++)
		{
			//zrow.append(ui.tableWidget->item(j,i)->text());
			setSilentValue(j,i,zrow[i-1].toString());
		}
	}
	writeTable(true);
}

void TableView3D::exportClicked()
{
	QString filename = QFileDialog::getSaveFileName(this,"Save Json File",".","Json Files (*.json)");
	if (filename == "")
	{
		return;
	}
	if (!filename.toLower().endsWith(".json"))
	{
		filename = filename + ".json";
	}
	exportJson(filename);
}
void TableView3D::loadRamClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the table from ram, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		QLOG_DEBUG() << "Ok";
		//emit reloadTableData(m_locationId,true);
		tableData->updateFromRam();
	}
	else
	{
		QLOG_DEBUG() << "Not ok";
	}
}

void TableView3D::loadClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the table from flash, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		QLOG_DEBUG() << "Ok";
		tableData->updateFromFlash();
		//emit reloadTableData(m_locationId,false);
	}
	else
	{
		QLOG_DEBUG() << "Not ok";
	}

}
bool TableView3D::updateTable()
{
	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
	//connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	QList<QPair<int,int> > selectedlist;
	if (ui.tableWidget->selectedItems().size() > 0)
	{
		for (int i=0;i<ui.tableWidget->selectedItems().size();i++)
		{
			selectedlist.append(QPair<int,int>(ui.tableWidget->selectedItems()[i]->row(),ui.tableWidget->selectedItems()[i]->column()));
		}
	}
	QLOG_DEBUG() << "updateTable(): Clearing table";
	ui.tableWidget->clear();
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	ui.tableWidget->setRowCount(tableData->rows()+1);
	ui.tableWidget->setColumnCount(tableData->columns()+1);
	if (tableData->yAxis().size() == 0 || tableData->xAxis().size() == 0)
	{
		//Invalid/empty data
		QLOG_ERROR() << "3D Table axis had zero values. This is INVALID and should be fixed.";
		return false;
	}
	double first = tableData->yAxis()[0];
	int order = 0;
	for (int i=0;i<tableData->rows();i++)
	{
		if (i == 1)
		{
			if (tableData->yAxis()[i] < first)
			{
				order = 1;
			}
			else
			{
				order = 2;
			}
		}
		if (order == 1)
		{
			if (tableData->yAxis()[i] > first)
			{
				//Out of order table axis.
				return false;
			}
		}
		else if (order == 2)
		{
			if (tableData->yAxis()[i] < first)
			{
				//Out of order table axis.
				return false;
			}
		}
		first = tableData->yAxis()[i];

		if (tableData->yAxis()[i] < first)
		{
			//Out of order axis;
			return false;
		}
		first = tableData->yAxis()[i];
		ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(formatNumber(tableData->yAxis()[i],m_metaData.yDp)));
		//ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(QString::number(val)));
	}
	first = tableData->xAxis()[0];
	for (int i=0;i<tableData->columns();i++)
	{
		if (i == 1)
		{
			if (tableData->xAxis()[i] < first)
			{
				order = 1;
			}
			else
			{
				order = 2;
			}
		}
		if (order == 1)
		{
			if (tableData->xAxis()[i] > first)
			{
				//Out of order table axis.
				return false;
			}
		}
		else if (order == 2)
		{
			if (tableData->xAxis()[i] < first)
			{
				//Out of order table axis.
				return false;
			}
		}
		first = tableData->xAxis()[i];
		if (tableData->xAxis()[i] < first)
		{
			//Out of order axis;
			return false;
		}
		first = tableData->xAxis()[i];
		//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(QString::number(val)));
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(formatNumber(tableData->xAxis()[i],m_metaData.xDp)));
	}
	for (int row=0;row<tableData->rows();row++)
	{
		for (int col=0;col<tableData->columns();col++)
		{
			double val = tableData->values()[row][col];
			ui.tableWidget->setItem((tableData->rows()-1)-(row),col+1,new QTableWidgetItem(formatNumber(val,m_metaData.zDp)));
			if (val < tableData->maxZAxis()/4)
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(tableData->maxZAxis()/4.0))),255));
			}
			else if (val < ((tableData->maxZAxis()/4)*2))
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((tableData->maxZAxis()/4.0)))/(tableData->maxZAxis()/4.0)))));
			}
			else if (val < ((tableData->maxZAxis()/4)*3))
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((tableData->maxZAxis()/4.0)*2))/(tableData->maxZAxis()/4.0))),255,0));
			}
			else
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((tableData->maxZAxis()/4.0)*3))/(tableData->maxZAxis()/4.0))),0));
			}
		}
	}
	//ui.tableWidget->resizeColumnsToContents();
	resizeColumnWidths();
	ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem());
	ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->setFlags(ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->flags() & ~Qt::ItemIsEditable);
	ui.tableWidget->setCurrentCell(m_currRow,m_currCol);
	for (int i=0;i<selectedlist.size();i++)
	{
		ui.tableWidget->item(selectedlist[i].first,selectedlist[i].second)->setSelected(true);
	}

	selectedlist.clear();
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	QLOG_DEBUG() << "updateTable(): Done with table";
	return true;
	//return passData(locationid,data,physicallocation,Table3DMetaData());
}

void TableView3D::setMetaData(Table3DMetaData metadata)
{
	m_metaData = metadata;
	metaDataValid = true;
}
bool TableView3D::setData(QString name,DataBlock *data)
{
	Q_UNUSED(name)
	if (tableData == 0)
	{
		tableData = dynamic_cast<Table3DData*>(data);
		connect(tableData,SIGNAL(update()),this,SLOT(updateTable()));
		//connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	}
	else
	{
	}
	//m_locationId = locationid;
	return updateTable();
}

bool TableView3D::setData(unsigned short locationid,DataBlock *data)
{

	if (tableData == 0)
	{
		tableData = dynamic_cast<Table3DData*>(data);
		connect(tableData,SIGNAL(update()),this,SLOT(updateTable()));
		//connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	}
	else
	{
	}
	m_locationId = locationid;
	return updateTable();
}
void TableView3D::reColorTable(int rownum,int colnum)
{
	//QLOG_DEBUG() << "Recoloring" << rownum << colnum;
	if (rownum == ui.tableWidget->rowCount()-1 || colnum == 0)
	{
		return;
	}
	if (rownum == -1 && colnum == -1)
	{
		//Recolor the whole table
		ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
		ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
		QLOG_DEBUG() << "reColorTable(): Starting";
		for (int row=0;row<tableData->rows();row++)
		{
			for (int col=0;col<tableData->columns();col++)
			{
				double val = tableData->values()[row][col];
				//ui.tableWidget->setItem((tableData->rows()-1)-(row),col+1,new QTableWidgetItem(formatNumber(val,m_metaData.zDp)));
				if (val < tableData->maxZAxis()/4)
				{
					ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(tableData->maxZAxis()/4.0))),255));
				}
				else if (val < ((tableData->maxZAxis()/4)*2))
				{
					ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((tableData->maxZAxis()/4.0)))/(tableData->maxZAxis()/4.0)))));
				}
				else if (val < ((tableData->maxZAxis()/4)*3))
				{
					ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((tableData->maxZAxis()/4.0)*2))/(tableData->maxZAxis()/4.0))),255,0));
				}
				else
				{
					ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((tableData->maxZAxis()/4.0)*3))/(tableData->maxZAxis()/4.0))),0));
				}
			}
		}
		QLOG_DEBUG() << "reColorTable(): Finished";
		connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
		connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	}
	else
	{

		/*

		(9) = 5+4;
		10-1-4=5
		10-1-3=6

		10-1-0=9
		10-1-9=0

		rownum == 0
		tabledata->values()[(tableData->rows()-1)-rownum][colnum-1]

*/

		//QLOG_DEBUG() << "Loc:" << (tableData->rows()-1)-(rownum) << colnum - 1;
		ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
		ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
		double val = tableData->values()[(tableData->rows()-1)-(rownum)][colnum-1];
		//QLOG_DEBUG() << "Value:" << val;

		//ui.tableWidget->setItem((tableData->rows()-1)-(row),col+1,new QTableWidgetItem(formatNumber(val,m_metaData.zDp)));
		if (val < tableData->maxZAxis()/4)
		{
			ui.tableWidget->item(rownum,colnum)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(tableData->maxZAxis()/4.0))),255));
		}
		else if (val < ((tableData->maxZAxis()/4)*2))
		{
			ui.tableWidget->item(rownum,colnum)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((tableData->maxZAxis()/4.0)))/(tableData->maxZAxis()/4.0)))));
		}
		else if (val < ((tableData->maxZAxis()/4)*3))
		{
			ui.tableWidget->item(rownum,colnum)->setBackgroundColor(QColor::fromRgb((255*((val-((tableData->maxZAxis()/4.0)*2))/(tableData->maxZAxis()/4.0))),255,0));
		}
		else
		{
			ui.tableWidget->item(rownum,colnum)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((tableData->maxZAxis()/4.0)*3))/(tableData->maxZAxis()/4.0))),0));
		}
		connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
		connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));

	}
}

void TableView3D::passDatalog(QVariantMap data)
{
	if (!m_tracingEnabled)
	{
		return;
	}
	if (data.contains(m_metaData.xHighlight) && data.contains(m_metaData.yHighlight))
	{
		double xval = data[m_metaData.xHighlight].toDouble();
		double yval = data[m_metaData.yHighlight].toDouble();
		int xloc = 1;
		int yloc = ui.tableWidget->rowCount()-2;
		int lowerrow = 0;
		double lowerrowratio = 0;
		double upperrowratio = 0;
		double lowercolumnratio = 0;
		double uppercolumnratio = 0;
		int higherrow = 0;
		int lowercolumn = 0;
		int highercolumn = 1;
		for (int x=1;x<ui.tableWidget->columnCount()-1;x++)
		{
			double testval = ui.tableWidget->item(ui.tableWidget->rowCount()-1,x)->text().toDouble();
			double prevtestval;
			double nexttestval;
			if (x == 1)
			{
				prevtestval = 0;
			}
			else
			{
				prevtestval = ui.tableWidget->item(ui.tableWidget->rowCount()-1,x-1)->text().toDouble();
			}
			if (x == ui.tableWidget->columnCount()-1)
			{
				nexttestval = testval + 1;
			}
			else
			{
				nexttestval = ui.tableWidget->item(ui.tableWidget->rowCount()-1,x+1)->text().toDouble();
			}
			double lowerlimit = testval - ((testval - prevtestval) / 2.0);
			double upperlimit = testval + ((nexttestval - testval) / 2.0);
			if (xval > lowerlimit && xval < upperlimit)
			{
				xloc = x;
				lowercolumn = x-1;
				highercolumn = x+1;
				lowercolumnratio = (xval - lowerlimit) / (upperlimit - lowerlimit);
				uppercolumnratio = (upperlimit - xval) / (upperlimit - lowerlimit);
				if (xval > testval)
				{
					lowercolumn = x+1;
					highercolumn = x+2;
					lowercolumnratio = (xval - testval) / (nexttestval - testval);
					uppercolumnratio = (nexttestval - xval) / (nexttestval - testval);
				}
				else
				{
					lowercolumn = x;
					highercolumn = x+1;
					lowercolumnratio = (xval - prevtestval) / (testval - prevtestval);
					uppercolumnratio = (testval - xval) / (testval - prevtestval);
				}
				//0 500 1000
				//Val is at 750.
				//lowerrowratio should be 50%, and upper should be 50%
				//(xval - lowerlimit) / (upperlimit - lowerlimit) //0-1.0
				//(upperlimit - xval) / (upperlimit - lowerlimit) //0-1.0


				break;
			}
		}
		for (int y=ui.tableWidget->rowCount()-2;y>=1;y--)
		{
			if (ui.tableWidget->item(y,0))
			{
				double testval = ui.tableWidget->item(y,0)->text().toDouble();
				double prevtestval;
				double nexttestval;
				if (y == ui.tableWidget->rowCount()-2)
				{
					prevtestval = 0;
				}
				else
				{
					prevtestval = ui.tableWidget->item(y+1,0)->text().toDouble();
				}
				if (y == 0)
				{
					nexttestval = testval + 1;
				}
				else
				{
					nexttestval = ui.tableWidget->item(y-1,0)->text().toDouble();
				}
				double lowerlimit = testval - ((testval - prevtestval) / 2.0);
				double upperlimit = testval + ((nexttestval - testval) / 2.0);
				if (yval > lowerlimit && yval < upperlimit)
				{
					yloc = y;
					if (yval > testval)
					{
						lowerrow = y-1;
						higherrow = y;
						lowerrowratio = (yval - testval) / (nexttestval - testval);
						upperrowratio = (nexttestval - yval) / (nexttestval - testval);
					}
					else
					{
						lowerrow = y-2;
						higherrow = y-1;
						lowerrowratio = (yval - prevtestval) / (testval - prevtestval);
						upperrowratio = (testval - yval) / (testval - prevtestval);
					}
					//0 500 1000
					//Val is at 750.
					//lowerrowratio should be 50%, and upper should be 50%
					//(xval - lowerlimit) / (upperlimit - lowerlimit) //0-1.0
					//(upperlimit - xval) / (upperlimit - lowerlimit) //0-1.0
					break;
				}
			}
		}
		if (xloc != -1 && yloc != -1)
		{
			if (xloc == m_oldXLoc && yloc == m_oldYLoc)
			{
				//No change, no reason to continue;
				//return;
			}
			ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
			ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
			QList<QPair<int,int> > undonelist;
			for (int i=0;i<m_highlightItemList.size();i++)
			{
				if (!ui.tableWidget->item(m_highlightItemList[i].first,m_highlightItemList[i].second)->isSelected())
				{
					ui.tableWidget->item(m_highlightItemList[i].first,m_highlightItemList[i].second)->setTextColor(QColor::fromRgb(0,0,0));
					ui.tableWidget->item(m_highlightItemList[i].first,m_highlightItemList[i].second)->setData(Qt::UserRole+1,false);
				}
				else
				{
					undonelist.append(QPair<int,int>(m_highlightItemList[i].first,m_highlightItemList[i].second));
				}
			}
			m_highlightItemList.clear();
			m_highlightItemList.append(undonelist);
			m_oldXLoc = xloc;
			m_oldYLoc = yloc;

			if (lowercolumn > 0 )
			{
				if (lowerrow > 0)
				{
					if (!ui.tableWidget->item(lowerrow,lowercolumn)->isSelected())
					{
						m_highlightItemList.append(QPair<int,int>(lowerrow,lowercolumn));
						ui.tableWidget->item(lowerrow,lowercolumn)->setData(Qt::UserRole+1,true);
						ui.tableWidget->item(lowerrow,lowercolumn)->setData(Qt::UserRole+2,lowerrowratio);
					}
				}
				if (higherrow < ui.tableWidget->rowCount()-1)
				{
					if (!ui.tableWidget->item(higherrow,lowercolumn)->isSelected())
					{
						m_highlightItemList.append(QPair<int,int>(higherrow,lowercolumn));
						ui.tableWidget->item(higherrow,lowercolumn)->setData(Qt::UserRole+1,true);
						ui.tableWidget->item(higherrow,lowercolumn)->setData(Qt::UserRole+2,upperrowratio);
					}
				}
			}



			if (lowerrow > 0)
			{
				if (!ui.tableWidget->item(lowerrow,highercolumn)->isSelected())
				{
					if (lowercolumnratio != uppercolumnratio)
					{
					}
					//m_highlightItemList.append(QPair<int,int>(lowerrow,highercolumn));
					//ui.tableWidget->item(lowerrow,highercolumn)->setData(Qt::UserRole+1,true);
					//ui.tableWidget->item(lowerrow,highercolumn)->setData(Qt::UserRole+2,lowercolumnratio);
				}
			}


			if (higherrow < ui.tableWidget->rowCount()-1)
			{
				if (!ui.tableWidget->item(higherrow,highercolumn)->isSelected())
				{
					//m_highlightItemList.append(QPair<int,int>(higherrow,highercolumn));
					//ui.tableWidget->item(higherrow,highercolumn)->setData(Qt::UserRole+1,true);
					//ui.tableWidget->item(higherrow,highercolumn)->setData(Qt::UserRole+2,uppercolumnratio);
				}
			}
			/*if (ui.tableWidget->item(m_oldYLoc,m_oldXLoc))
			{
				ui.tableWidget->item(m_oldYLoc,m_oldXLoc)->setTextColor(QColor::fromRgb(255,255,255));
				ui.tableWidget->item(m_oldYLoc,m_oldXLoc)->setData(Qt::UserRole+1,true);
			}*/
			connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
			connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
		}
		else
		{
			QLOG_ERROR() << "Error parsing datalog, xloc and yloc aren't != -1";
		}
	}
}
/*bool TableView3D::setData(unsigned short locationid,QByteArray data,Table3DMetaData metadata,TableData *newtabledata)
{
	m_metaData = metadata;
	metaDataValid = true;
	return setData(locationid,data,newtabledata);
}
bool TableView3D::setData(unsigned short locationid,Table3DData *data,Table3DMetaData metadata)
{
	m_metaData = metadata;
	metaDataValid = true;
	connect(data,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	setData(locationid,data);
}

bool TableView3D::setData(unsigned short locationid,QByteArray rawdata)
{
	return setData(locationid,rawdata,tableData);
}*/
QString TableView3D::formatNumber(double num,int prec)
{
	if (metaDataValid)
	{
		return QString::number(num,'f',prec);
	}
	else
	{
		return QString::number(num);
	}
}
void TableView3D::resizeColumnWidths()
{
	unsigned int max = 0;
	for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		for (int j=0;j<ui.tableWidget->rowCount();j++)
		{
			if (ui.tableWidget->item(j,i))
			{
				//1.3 is required to make text show correctly
				unsigned int test = ui.tableWidget->fontMetrics().width(ui.tableWidget->item(j,i)->text()) * 1.3;
				if (test > max)
				{
					max = test;
				}
			}
		}
	}
	for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		ui.tableWidget->setColumnWidth(i,max);
	}
}

void TableView3D::tableCellChanged(int row,int column)
{
	bool conversionOk = false; // Note, value of this is irrelevant, overwritten during call in either case, but throws a compiler error if not set.
	double tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);
	Q_UNUSED(tempValue)
	//TODO Fix that ^^
	//double oldValue = tempValue;
	if (!conversionOk)
	{
		QMessageBox::information(0,"Error","Value entered is not a number: " + ui.tableWidget->item(row,column)->text());
		setSilentValue(row,column,formatNumber(currentvalue));
		//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	setValue(row,column,tempValue,false); //Don't ignore selection, so ALL highlighted cells get changed
}
void TableView3D::setSilentValue(int row,int column,QString value)
{
	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
	ui.tableWidget->item(row,column)->setText(value);
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
}


void TableView3D::saveClicked()
{
	//emit saveToFlash(m_locationId);
	tableData->saveRamToFlash();
}
TableView3D::~TableView3D()
{
}
