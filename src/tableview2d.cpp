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

#include "tableview2d.h"
#include <QMessageBox>
#include <QDebug>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
TableView2D::TableView2D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	tableData=0;
	//ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setRowCount(2);
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	//ui.tableWidget->setColumnWidth(0,100);
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));

	QPalette pal = ui.plot->palette();
	pal.setColor(QPalette::Background,QColor::fromRgb(0,0,0));
	ui.plot->setPalette(pal);
	curve = new QwtPlotCurve("Test");
	curve->attach(ui.plot);
	curve->setPen(QPen(QColor::fromRgb(255,0,0),3));
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->setPen(QPen(QColor::fromRgb(100,100,100)));
	grid->attach(ui.plot);

	//curve->setData()
	//QwtSeriesData<QwtIntervalSample> series;


}
void TableView2D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	if (currentrow == -1 || currentcolumn == -1)
	{
		return;
	}
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toInt();
}

void TableView2D::loadClicked()
{
	emit reloadTableData(m_locationid);
}

void TableView2D::tableCellChanged(int row,int column)
{
	if (row == -1 || column == -1)
	{
		return;
	}
	if (row >= ui.tableWidget->rowCount() || column >= ui.tableWidget->columnCount())
	{
		return;
	}
	bool ok = false;
	if (ui.tableWidget->item(row,column)->text().toInt(&ok) == currentvalue)
	{
		return;
	}
	if (ui.tableWidget->item(row,column)->text().toInt(&ok) > 65535)
	{
		QMessageBox::information(0,"Error",QString("Value entered too large! Value range 0-65535. Entered value:") + ui.tableWidget->item(row,column)->text());
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	else if (ok == false)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	if (samples.size() <= column)
	{
		return;
	}
	unsigned short newval = ui.tableWidget->item(row,column)->text().toInt();
	currentvalue = newval;
	if (row == 0)
	{
		samples.replace(column,QPointF(ui.tableWidget->item(row,column)->text().toInt(),samples.at(column).y()));
		curve->setSamples(samples);
		ui.plot->replot();
	}
	else if (row == 1)
	{
		samples.replace(column,QPointF(samples.at(column).x(),ui.tableWidget->item(row,column)->text().toInt()));
		curve->setSamples(samples);
		ui.plot->replot();
	}
	//New value has been accepted. Let's write it.
	tableData->setCell(row,column,newval); //This will emit saveSingleData
	ui.tableWidget->resizeColumnsToContents();
}
void TableView2D::resizeEvent(QResizeEvent *evt)
{
	/*for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		ui.tableWidget->setColumnWidth(i,(ui.tableWidget->width() / ui.tableWidget->columnCount())-1);
	}*/
}

void TableView2D::passData(unsigned short locationid,QByteArray rawdata,int physicallocation)
{
	if (tableData)
	{
		tableData->deleteLater();
	}
	tableData = new Table2DData(locationid,rawdata);
	connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	qDebug() << "TableView2D::passData" << "0x" + QString::number(locationid,16).toUpper();
	samples.clear();
	m_locationid = locationid;
	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->clear();
	ui.tableWidget->setColumnCount(0);
	ui.tableWidget->setRowCount(2);
	for (int i=0;i<tableData->columns();i++)
	{
		ui.tableWidget->setColumnCount(ui.tableWidget->columnCount()+1);
		ui.tableWidget->setItem(0,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(tableData->axis()[i])));
		ui.tableWidget->setItem(1,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(tableData->values()[i])));
		if (tableData->values()[i] < 65535/4)
		{
			ui.tableWidget->item(1,ui.tableWidget->columnCount()-1)->setBackgroundColor(QColor::fromRgb(0,(255*((tableData->values()[i])/(65535.0/4.0))),255));
		}
		else if (tableData->values()[i] < ((65535/4)*2))
		{
			ui.tableWidget->item(1,ui.tableWidget->columnCount()-1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((tableData->values()[i]-((65535/4.0)))/(65535.0/4.0)))));
		}
		else if (tableData->values()[i] < ((65535/4)*3))
		{
			ui.tableWidget->item(1,ui.tableWidget->columnCount()-1)->setBackgroundColor(QColor::fromRgb((255*((tableData->values()[i]-((65535/4.0)*2))/(65535.0/4.0))),255,0));
		}
		else
		{
			ui.tableWidget->item(1,ui.tableWidget->columnCount()-1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((tableData->values()[i]-((65535/4.0)*3))/(65535.0/4.0))),0));
		}
		samples.append(QPointF(tableData->axis()[i],tableData->values()[i]));
	}
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	curve->setSamples(samples);
	ui.plot->replot();
	ui.tableWidget->resizeColumnsToContents();
}

TableView2D::~TableView2D()
{
}
void TableView2D::saveClicked()
{
	emit saveToFlash(m_locationid);
}
