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
void TableView3D::passData(unsigned short locationid,QByteArray data,int physicallocation)
{
	if (tableData)
	{
		tableData->deleteLater();
	}
	tableData = new Table3DData(locationid,data);
	connect(tableData,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
	m_locationId = locationid;

	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->clear();
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	ui.tableWidget->setRowCount(tableData->rows()+1);
	ui.tableWidget->setColumnCount(tableData->columns()+1);

	for (int i=0;i<tableData->rows();i++)
	{
		ui.tableWidget->setItem((tableData->rows()-1)-(i),0,new QTableWidgetItem(QString::number(tableData->yAxis()[i])));
	}
	for (int i=0;i<tableData->columns();i++)
	{
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i+1),new QTableWidgetItem(QString::number(tableData->xAxis()[i])));
	}
	for (int i=0;i<tableData->rows();i++)
	{
		for (int j=0;j<tableData->columns();j++)
		{
			unsigned short val = tableData->values()[i][j];
			ui.tableWidget->setItem((tableData->rows()-1)-(i),j+1,new QTableWidgetItem(QString::number(val)));
			if (val < 65535/4)
			{
				ui.tableWidget->item((tableData->rows()-1)-((i)),(j)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(65535.0/4.0))),255));
			}
			else if (val < ((65535/4)*2))
			{
				ui.tableWidget->item((tableData->rows()-1)-((i)),(j)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((65535/4.0)))/(65535.0/4.0)))));
			}
			else if (val < ((65535/4)*3))
			{
				ui.tableWidget->item((tableData->rows()-1)-((i)),(j)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((65535/4.0)*2))/(65535.0/4.0))),255,0));
			}
			else
			{
				ui.tableWidget->item((tableData->rows()-1)-((i)),(j)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((65535/4.0)*3))/(65535.0/4.0))),0));
			}
		}
	}
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem());
	ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->setFlags(ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->flags() & ~Qt::ItemIsEditable);
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));

}
void TableView3D::tableCellChanged(int row,int column)
{
	if (row == ui.tableWidget->rowCount()-1 && column == 0)
	{
		//ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
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
	/*if (samples.size() <= column)
	{
		return;
	}*/
	unsigned short newval = ui.tableWidget->item(row,column)->text().toInt();
	currentvalue = newval;
	if (row == 0)
	{
		//samples.replace(column,QPointF(ui.tableWidget->item(row,column)->text().toInt(),samples.at(column).y()));
		//curve->setSamples(samples);
		//ui.plot->replot();
	}
	else if (row == 1)
	{
		//samples.replace(column,QPointF(samples.at(column).x(),ui.tableWidget->item(row,column)->text().toInt()));
		//curve->setSamples(samples);
		//ui.plot->replot();
	}
	//New value has been accepted. Let's write it.
	if (row == ui.tableWidget->rowCount()-1)
	{
		tableData->setXAxis(column-1,newval);
	}
	else if (column == 0)
	{
		tableData->setYAxis(row,newval);
	}
	else
	{
		tableData->setCell(row+1,column-1,newval);
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
