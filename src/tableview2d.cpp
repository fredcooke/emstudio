#include "tableview2d.h"
#include <QMessageBox>

TableView2D::TableView2D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	//ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setRowCount(2);
	//ui.tableWidget->setColumnWidth(0,100);
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
}
void TableView2D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toInt();
}

void TableView2D::tableCellChanged(int row,int column)
{
	bool ok = false;
	if (ui.tableWidget->item(row,column)->text().toInt(&ok) > 65535)
	{
		QMessageBox::information(0,"Error",QString("Value entered too large! Value range 0-65535. Entered value:") + ui.tableWidget->item(row,column)->text());
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
	}
	else if (ok == false)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
	}
}

void TableView2D::passData(unsigned short locationid,QByteArray data,int physicallocation)
{
	m_locationid = locationid;
	m_physicalid = physicallocation;

	for (int i=0;i<data.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)data[i]) << 8) + ((unsigned char)data[i+1]);
		unsigned short y = (((unsigned char)data[(data.size()/2)+ i]) << 8) + ((unsigned char)data[(data.size()/2) + i+1]);
		/*ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
		ui.tableWidget->setVerticalHeaderItem(ui.tableWidget->rowCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(y)));*/
		ui.tableWidget->setColumnCount(ui.tableWidget->columnCount()+1);
		ui.tableWidget->setColumnWidth(ui.tableWidget->columnCount()-1,50);
		ui.tableWidget->setItem(0,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(1,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(y)));
	}
}

TableView2D::~TableView2D()
{
}
void TableView2D::saveClicked()
{
	QByteArray data;
	bool ok = false;
	QByteArray first;
	QByteArray second;
	for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		unsigned short loc = ui.tableWidget->item(0,i)->text().toInt(&ok,10);
		first.append((char)((loc >> 8) & 0xFF));
		first.append((char)(loc & 0xFF));
		unsigned short loc2 = ui.tableWidget->item(1,i)->text().toInt(&ok,10);
		second.append((char)((loc2 >> 8) & 0xFF));
		second.append((char)(loc2 & 0xFF));
	}
	data.append(first);
	data.append(second);
	emit saveData(m_locationid,data,m_physicalid);
}
