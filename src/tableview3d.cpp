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
TableView3D::TableView3D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	tableData=0;
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
}
void TableView3D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	if (currentrow == -1 || currentcolumn == -1 || !ui.tableWidget->item(currentrow,currentcolumn))
	{
		return;
	}
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toInt();
}
void TableView3D::loadClicked()
{
	emit reloadTableData(m_locationId);
}
void TableView3D::passData(unsigned short locationid,QByteArray data,int physicallocation,Table3DMetaData metadata)
{
	m_metaData = metadata;
	if (tableData)
	{
		tableData->deleteLater();
	}
	tableData = new Table3DData(locationid,data);
	connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	m_locationId = locationid;

	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
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
		double val = (unsigned short)tableData->yAxis()[i];
		for (int j=0;j<m_metaData.yAxisCalc.size();j++)
		{
			if (m_metaData.yAxisCalc[j].first == "add")
			{
				val += m_metaData.yAxisCalc[j].second;
			}
			else if (m_metaData.yAxisCalc[j].first == "sub")
			{
				val -= m_metaData.yAxisCalc[j].second;
			}
			else if (m_metaData.yAxisCalc[j].first == "mult")
			{
				val *= m_metaData.yAxisCalc[j].second;
			}
			else if (m_metaData.yAxisCalc[j].first == "div")
			{
				val /= m_metaData.yAxisCalc[j].second;
			}
		}
		//ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(QString::number(tableData->yAxis()[i])));
		ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<tableData->columns();i++)
	{
		double val = (unsigned short)tableData->xAxis()[i];
		for (int j=0;j<m_metaData.xAxisCalc.size();j++)
		{
			if (m_metaData.xAxisCalc[j].first == "add")
			{
				val += m_metaData.xAxisCalc[j].second;
			}
			else if (m_metaData.xAxisCalc[j].first == "sub")
			{
				val -= m_metaData.xAxisCalc[j].second;
			}
			else if (m_metaData.xAxisCalc[j].first == "mult")
			{
				val *= m_metaData.xAxisCalc[j].second;
			}
			else if (m_metaData.xAxisCalc[j].first == "div")
			{
				val /= m_metaData.xAxisCalc[j].second;
			}
		}
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(QString::number(val)));
		//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(QString::number(tableData->xAxis()[i])));
	}
	for (int row=0;row<tableData->rows();row++)
	{
		for (int col=0;col<tableData->columns();col++)
		{
			double val = (unsigned short)tableData->values()[row][col];
			unsigned short val2 = (unsigned short)tableData->values()[row][col];
			double max = 65535;
			for (int i=0;i<m_metaData.zAxisCalc.size();i++)
			{
				if (m_metaData.zAxisCalc[i].first == "add")
				{
					val += m_metaData.zAxisCalc[i].second;
					max += m_metaData.zAxisCalc[i].second;
				}
				else if (m_metaData.zAxisCalc[i].first == "sub")
				{
					val -= m_metaData.zAxisCalc[i].second;
					max -= m_metaData.zAxisCalc[i].second;
				}
				else if (m_metaData.zAxisCalc[i].first == "mult")
				{
					val *= m_metaData.zAxisCalc[i].second;
					max *= m_metaData.zAxisCalc[i].second;
				}
				else if (m_metaData.zAxisCalc[i].first == "div")
				{
					val /= m_metaData.zAxisCalc[i].second;
					max /= m_metaData.zAxisCalc[i].second;
				}
			}
			ui.tableWidget->setItem((tableData->rows()-1)-(row),col+1,new QTableWidgetItem(QString::number(val)));
			if (val < max/4)
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(max/4.0))),255));
			}
			else if (val < ((max/4)*2))
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((max/4.0)))/(max/4.0)))));
			}
			else if (val < ((max/4)*3))
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((max/4.0)*2))/(max/4.0))),255,0));
			}
			else
			{
				ui.tableWidget->item((tableData->rows()-1)-((row)),(col)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((max/4.0)*3))/(max/4.0))),0));
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

}
void TableView3D::tableCellChanged(int row,int column)
{
	// Ignore bottom right corner if the disallow on editing fails
	if (row == ui.tableWidget->rowCount()-1 && column == 0)
	{
		qDebug() << "This should not happen! Bottom right corner ignored!";
		return;
	}

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

	bool conversionOk = false; // Note, value of this is irrelevant, overwritten during call in either case.
	double tempValue = ui.tableWidget->item(row,column)->text().toDouble(&conversionOk);
	double oldValue = tempValue;
	unsigned short newTempValue=0;
	//Convert tempValue back to a proper unsigned short
	for (int i=m_metaData.zAxisCalc.size()-1;i>=0;i--)
	{
		if (m_metaData.zAxisCalc[i].first == "add")
		{
			tempValue -= m_metaData.zAxisCalc[i].second;
		}
		else if (m_metaData.zAxisCalc[i].first == "sub")
		{
			tempValue += m_metaData.zAxisCalc[i].second;
		}
		else if (m_metaData.zAxisCalc[i].first == "mult")
		{
			tempValue /= m_metaData.zAxisCalc[i].second;
		}
		else if (m_metaData.zAxisCalc[i].first == "div")
		{
			tempValue *= m_metaData.zAxisCalc[i].second;
		}
	}
	newTempValue = tempValue;


	if (!conversionOk)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}


	if (newTempValue > 65535)
	{
		QMessageBox::information(0,"Error",QString("Value entered too large! Value range 0-65535. Entered value:") + ui.tableWidget->item(row,column)->text());
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}

	currentvalue = oldValue;

	//New value has been accepted. Let's write it.
	if (row == ui.tableWidget->rowCount()-1)
	{
		tableData->setXAxis(column-1,newTempValue);
	}
	else if (column == 0)
	{
		tableData->setYAxis(ui.tableWidget->rowCount()-(row+2),newTempValue);
	}
	else
	{
		tableData->setCell(ui.tableWidget->rowCount()-(row+2),column-1,newTempValue);
	}
	ui.tableWidget->resizeColumnsToContents();
}
void TableView3D::saveClicked()
{
	emit saveToFlash(m_locationId);
}
TableView3D::~TableView3D()
{
}
