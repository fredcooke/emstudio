#include "tableview2d.h"


TableView2D::TableView2D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setColumnWidth(0,100);
}
void TableView2D::passData(QByteArray data)
{
	for (int i=0;i<data.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)data[i]) << 8) + ((unsigned char)data[i+1]);
		unsigned short y = (((unsigned char)data[(data.size()/2)+ i]) << 8) + ((unsigned char)data[(data.size()/2) + i+1]);
		ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
		ui.tableWidget->setVerticalHeaderItem(ui.tableWidget->rowCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(y)));
	}
}

TableView2D::~TableView2D()
{
}
