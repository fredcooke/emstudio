#include "tableview3d.h"
#include <QDebug>

TableView3D::TableView3D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
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
	//connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	//disconnect(ui.tableWidget,SIGNAL(cellChanged(int,int)));
	ui.tableWidget->clear();

	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	qDebug() << "XAxis:" << xaxissize;
	qDebug() << "YAxis:" << yaxissize;

	ui.tableWidget->setRowCount(xaxissize+1);
	ui.tableWidget->setColumnCount(yaxissize+1);
	for (int i=0;i<xaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i]) << 8) + (unsigned char)data[5+i];
		ui.tableWidget->setItem((i/2)+1,0,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<yaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i+(xaxissize*2)]) << 8) + (unsigned char)data[5+i+(xaxissize*2)];
		ui.tableWidget->setItem(0,(i/2)+1,new QTableWidgetItem(QString::number(val)));
	}
	for (int i=0;i<xaxissize*2;i+=2)
	{
		for (int j=0;j<yaxissize*2;j+=2)
		{
			unsigned short val = (((unsigned char)data[4+((xaxissize+yaxissize)*2) + (i*(xaxissize*2)) + j]) << 8) + (unsigned char)data[5+((yaxissize+xaxissize)*2) + (i*((xaxissize)*2)) + j];
			ui.tableWidget->setItem((j/2)+1,(i/2)+1,new QTableWidgetItem(QString::number(val)));
		}
	}
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

TableView3D::~TableView3D()
{
}
