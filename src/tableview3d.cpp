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
#include <QFileDialog>
#include <qjson/serializer.h>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
TableView3D::TableView3D(bool isram,bool isflash,QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	tableData=0;
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadFlashPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
	connect(ui.loadRamPushButton,SIGNAL(clicked()),this,SLOT(loadRamClicked()));
	connect(ui.exportPushButton,SIGNAL(clicked()),this,SLOT(exportClicked()));
	setContextMenuPolicy(Qt::DefaultContextMenu);
	//QAction* fooAction = new QAction("foo",this);
	//QAction* barAction = new QAction("bar",this);
	//connect(fooAction, SIGNAL(triggered()), this, SLOT(doSomethingFoo()));
	//connect(barAction, SIGNAL(triggered()), this, SLOT(doSomethingBar()));
	//addAction(fooAction);
	//addAction(barAction);
	metaDataValid = true;
	if (!isram)
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
		qDebug() << "Unable to open file to output JSON!";
		return;
	}
	file.write(serialized);
	file.close();
}

void TableView3D::exportClicked()
{
	QString filename = QFileDialog::getSaveFileName(this,"Save Json File",".","Json Files (*.json)");
	if (filename == "")
	{
		return;
	}
	exportJson(filename);
}
void TableView3D::loadRamClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the table from ram, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Ok";
		emit reloadTableData(m_locationId,true);
	}
	else
	{
		qDebug() << "Not ok";
	}
}

void TableView3D::loadClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the table from flash, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Ok";
		emit reloadTableData(m_locationId,false);
	}
	else
	{
		qDebug() << "Not ok";
	}

}
bool TableView3D::setData(unsigned short locationid,QByteArray data)
{

	if (tableData)
	{
		tableData->deleteLater();
	}
	tableData = new Table3DData(locationid,data,m_metaData);
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
	for (int i=0;i<selectedlist.size();i++)
	{
		ui.tableWidget->item(selectedlist[i].first,selectedlist[i].second)->setSelected(true);
	}
	ui.tableWidget->setCurrentCell(m_currRow,m_currCol);
	selectedlist.clear();
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	return true;
	//return passData(locationid,data,physicallocation,Table3DMetaData());
}

bool TableView3D::setData(unsigned short locationid,QByteArray data,Table3DMetaData metadata)
{
	m_metaData = metadata;
	metaDataValid = true;
	return setData(locationid,data);

}
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
	//ui.tableWidget->resizeColumnsToContents();
	resizeColumnWidths();
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
