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

#include "tableview3d.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <qjson/serializer.h>
TableView3D::TableView3D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	tableData=0;
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
	connect(ui.exportPushButton,SIGNAL(clicked()),this,SLOT(exportClicked()));
	metaDataValid = true;
}
void TableView3D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	Q_UNUSED(prevrow)
	Q_UNUSED(prevcolumn)
	if (currentrow == -1 || currentcolumn == -1 || !ui.tableWidget->item(currentrow,currentcolumn))
	{
		return;
	}
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toDouble();
}
void TableView3D::exportClicked()
{
	QVariantMap topmap;
	topmap["3DTable"] = "";
	topmap["title"] = m_metaData.tableTitle;
	topmap["description"] = m_metaData.tableTitle;
	QVariantMap x;
	x["unit"] = m_metaData.xAxisTitle;
	x["label"] = m_metaData.xAxisTitle;
	QVariantList xlist;
	for (int i=1;i<ui.tableWidget->columnCount();i++)
	{
		xlist.append(ui.tableWidget->item(ui.tableWidget->rowCount()-1,i)->text().toDouble());
	}
	x["values"] = xlist;
	QVariantMap y;
	y["unit"] = m_metaData.yAxisTitle;
	y["label"] = m_metaData.yAxisTitle;
	QVariantList ylist;
	for (int i=0;i<ui.tableWidget->rowCount()-1;i++)
	{
		ylist.append(ui.tableWidget->item(i,0)->text().toDouble());
	}
	y["values"] = ylist;
	QVariantMap z;
	z["unit"] = m_metaData.zAxisTitle;
	z["label"] = m_metaData.zAxisTitle;
	QVariantList zlist;
	for (int j=0;j<ui.tableWidget->rowCount()-1;j++)
	{
		QVariantList zrow;
		/*for (int i=1;i<ui.tableWidget->columnCount();i++)
		{
			zrow.append(QString::number(j) + ":ZDATA:" + QString::number(i));
			//zrow.append(ui.tableWidget->item(j,i)->text().toDouble());
		}*/
		zlist.append(QString::number(j) + ":ZROW");
	}
	z["values"] = zlist;
	//topmap["X"];
	topmap["X"] = x;
	topmap["Y"] = y;
	topmap["Z"] = z;

	QJson::Serializer serializer;
	QByteArray serialized = serializer.serialize(topmap);

	//This hack is to fix a QJson issue with lists in lists.
	for (int j=0;j<ui.tableWidget->rowCount()-1;j++)
	{
		QString list = "[";
		for (int i=1;i<ui.tableWidget->columnCount();i++)
		{
			list += ui.tableWidget->item(j,i)->text() + ",";
		}
		list = list.mid(0,list.length()-1);
		list += "]";
		QString before = "\"" + QString::number(j) + ":ZROW\"";
		serialized.replace(before,list.toAscii());
	}
	//End of hack

	QFile file("testoutput.json");
	file.open(QIODevice::ReadWrite | QIODevice::Truncate);
	file.write(serialized);
	file.close();
}

void TableView3D::loadClicked()
{
	emit reloadTableData(m_locationId);
}
void TableView3D::passData(unsigned short locationid,QByteArray data,int physicallocation)
{
	metaDataValid = false;
	passData(locationid,data,physicallocation,Table3DMetaData());
}

void TableView3D::passData(unsigned short locationid,QByteArray data,int physicallocation,Table3DMetaData metadata)
{
	Q_UNUSED(physicallocation)
	m_metaData = metadata;
	if (tableData)
	{
		tableData->deleteLater();
	}
	tableData = new Table3DData(locationid,data,metadata);
	connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	m_locationId = locationid;

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
	ui.tableWidget->clear();
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	ui.tableWidget->setRowCount(tableData->rows()+1);
	ui.tableWidget->setColumnCount(tableData->columns()+1);
	for (int i=0;i<tableData->rows();i++)
	{
		//double val = tableData->yAxis()[i];
		ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(formatNumber(tableData->yAxis()[i])));
		//ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<tableData->columns();i++)
	{
		//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(QString::number(val)));
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(formatNumber(tableData->xAxis()[i])));
	}
	for (int row=0;row<tableData->rows();row++)
	{
		for (int col=0;col<tableData->columns();col++)
		{
			double val = tableData->values()[row][col];
			ui.tableWidget->setItem((tableData->rows()-1)-(row),col+1,new QTableWidgetItem(formatNumber(val)));
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
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem());
	ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->setFlags(ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->flags() & ~Qt::ItemIsEditable);
	for (int i=0;i<selectedlist.size();i++)
	{
		ui.tableWidget->item(selectedlist[i].first,selectedlist[i].second)->setSelected(true);
	}
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));

}
QString TableView3D::formatNumber(double num)
{
	if (metaDataValid)
	{
		return QString::number(num,'f',2);
	}
	else
	{
		return QString::number(num);
	}
}

void TableView3D::tableCellChanged(int row,int column)
{
	if (row == -1 || column == -1)
	{
		qDebug() << "Negative array index! Should be unreachable code! FIXME!";
		return;
	}
	if (row >= ui.tableWidget->rowCount() || column >= ui.tableWidget->columnCount())
	{
		qDebug() << "Larger than life, should be unreachable code! FIXME!";
		return;
	}
	// Ignore bottom right corner if the disallow on editing fails
	if (row == ui.tableWidget->rowCount()-1 && column == 0)
	{
		qDebug() << "This should not happen! Bottom right corner ignored!";
		return;
	}
	bool conversionOk = false; // Note, value of this is irrelevant, overwritten during call in either case, but throws a compiler error if not set.
	double tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);
	double oldValue = tempValue;
	if (!conversionOk)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		setSilentValue(row,column,formatNumber(currentvalue));
		//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	setSilentValue(row,column,formatNumber(tempValue));
	//ui.tableWidget->item(row,column)->setText(QString::number(tempValue,'f',2));
	tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);

	//New value has been accepted. Let's write it.
	if (row == ui.tableWidget->rowCount()-1)
	{
		if (tempValue > tableData->maxXAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minXAxis()) + "-" + QString::number(tableData->maxXAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			setSilentValue(row,column,formatNumber(currentvalue));
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			return;
		}
		else if (tempValue < tableData->minXAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minXAxis()) + "-" + QString::number(tableData->maxXAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			setSilentValue(row,column,formatNumber(currentvalue));
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			return;
		}

		currentvalue = oldValue;
		tableData->setXAxis(column-1,currentvalue);
	}
	else if (column == 0)
	{
		if (tempValue > tableData->maxYAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minYAxis()) + "-" + QString::number(tableData->maxYAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			setSilentValue(row,column,formatNumber(currentvalue));
			return;
		}
		else if (tempValue < tableData->minYAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minYAxis()) + "-" + QString::number(tableData->maxYAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			setSilentValue(row,column,formatNumber(currentvalue));
			return;
		}
		currentvalue = oldValue;
		tableData->setYAxis(ui.tableWidget->rowCount()-(row+2),currentvalue);
	}
	else
	{
		if (tempValue > tableData->maxZAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too large! Value range " + QString::number(tableData->minZAxis()) + "-" + QString::number(tableData->maxZAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			setSilentValue(row,column,formatNumber(currentvalue));
			return;
		}
		if (tempValue < tableData->minZAxis())
		{
			QMessageBox::information(0,"Error",QString("Value entered too small! Value range " + QString::number(tableData->minZAxis()) + "-" + QString::number(tableData->maxZAxis()) + ". Entered value:") + ui.tableWidget->item(row,column)->text());
			//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
			setSilentValue(row,column,formatNumber(currentvalue));
			return;
		}
		currentvalue = oldValue;
		tableData->setCell(ui.tableWidget->rowCount()-(row+2),column-1,currentvalue);
	}
	ui.tableWidget->resizeColumnsToContents();
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
	emit saveToFlash(m_locationId);
}
TableView3D::~TableView3D()
{
}
