#include "tableview3d.h"
#include <QDebug>
#include <QMessageBox>
TableView3D::TableView3D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
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
	/*if (data.size() != 64)
	{
		qDebug() << "Passed a data pack to a 2d table that was of size" << data.size() << "should be 64!!!";
		return;
	}*/
	//qDebug() << "TableView2D::passData" << "0x" + QString::number(locationid,16).toUpper();
	m_locationId = locationid;
	//m_physicalid = physicallocation;

	ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	ui.tableWidget->clear();
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	qDebug() << "XAxis:" << xaxissize;
	qDebug() << "YAxis:" << yaxissize;

	m_xAxisSize = xaxissize;
	m_yAxisSize = yaxissize;

	ui.tableWidget->setRowCount(yaxissize+1);
	ui.tableWidget->setColumnCount(xaxissize+1);

	for (int i=0;i<xaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i]) << 8) + (unsigned char)data[5+i];
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i/2)+1,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<yaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[58+i]) << 8) + (unsigned char)data[59+i];
		ui.tableWidget->setItem((yaxissize-1) - (i/2),0,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<xaxissize*2;i+=2)
	{
		for (int j=0;j<yaxissize*2;j+=2)
		{
			//unsigned short val = (((unsigned char)data[4+((xaxissize+yaxissize)*2) + (i*(xaxissize*2)) + j]) << 8) + (unsigned char)data[5+((yaxissize+xaxissize)*2) + (i*((xaxissize)*2)) + j];
			unsigned short val = (((unsigned char)data[100 + j + (i * xaxissize)]) << 8) + (unsigned char)data[101 + j + (i * xaxissize)];
			ui.tableWidget->setItem((xaxissize-1)-((i/2)),(j/2)+1,new QTableWidgetItem(QString::number(val)));
			if (val < 65535/4)
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(65535.0/4.0))),255));
			}
			else if (val < ((65535/4)*2))
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((65535/4.0)))/(65535.0/4.0)))));
			}
			else if (val < ((65535/4)*3))
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((65535/4.0)*2))/(65535.0/4.0))),255,0));
			}
			else
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((65535/4.0)*3))/(65535.0/4.0))),0));
			}

		}
	}
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem());
	ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->setFlags(ui.tableWidget->item(ui.tableWidget->rowCount()-1,0)->flags() & ~Qt::ItemIsEditable);
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	//ui.tableWidget->setRowCount(2);
	/*for (int i=0;i<data.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)data[i]) << 8) + ((unsigned char)data[i+1]);
		unsigned short y = (((unsigned char)data[(data.size()/2)+ i]) << 8) + ((unsigned char)data[(data.size()/2) + i+1]);
		ui.tableWidget->setColumnCount(ui.tableWidget->columnCount()+1);
		ui.tableWidget->setColumnWidth(ui.tableWidget->columnCount()-1,ui.tableWidget->width() / (data.size()/4));
		ui.tableWidget->setItem(0,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(1,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(y)));

	}*/
	//series->setSamples(vector);
	//connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));

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
	//void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	//Data is 64
	//offset = column + (row * 32), size == 2
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	qDebug() << "Attempting to save data at:" << row << column;
	if (column == 0)
	{
		emit saveSingleData(m_locationId,data,58+(((m_xAxisSize-1) - row)*2),2);
	}
	else if (row == ui.tableWidget->rowCount()-1)
	{
		emit saveSingleData(m_locationId,data,4+((column-1)*2),2);
	}
	else
	{
		emit saveSingleData(m_locationId,data,100+((column-1)*2)+(((m_xAxisSize-1) - row) * 32),2);
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
