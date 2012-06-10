#include "tableview2d.h"


TableView2D::TableView2D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setColumnWidth(0,100);
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
}
void TableView2D::passData(unsigned short locationid,QByteArray data,int physicallocation)
{
	m_locationid = locationid;
	m_physicalid = physicallocation;
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
void TableView2D::saveClicked()
{
	QByteArray data;
	bool ok = false;
	for (int i=0;i<ui.tableWidget->rowCount();i++)
	{
		unsigned short loc = ui.tableWidget->verticalHeaderItem(i)->text().toInt(&ok,10);
		data.append((char)((loc << 8) & 0xFF));
		data.append((char)(loc & 0xFF));
	}
	for (int i=0;i<ui.tableWidget->rowCount();i++)
	{
		unsigned short loc = ui.tableWidget->item(i,0)->text().toInt(&ok,10);
		data.append((char)((loc << 8) & 0xFF));
		data.append((char)(loc & 0xFF));
	}
	emit saveData(m_locationid,data,m_physicalid);
}
