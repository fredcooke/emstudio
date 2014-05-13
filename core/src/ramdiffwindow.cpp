#include "ramdiffwindow.h"
#include "ui_ramdiffwindow.h"

RamDiffWindow::RamDiffWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RamDiffWindow)
{
	ui->setupUi(this);
	ui->tableWidget->setColumnCount(1);
	ui->tableWidget->setColumnWidth(0,200);
	connect(ui->pushLocalButton,SIGNAL(clicked()),this,SIGNAL(acceptLocalChanges()));
	connect(ui->pullRemoteButton,SIGNAL(clicked()),this,SIGNAL(rejectLocalChanges()));
}

RamDiffWindow::~RamDiffWindow()
{
	delete ui;
}
void RamDiffWindow::setDirtyLocation(unsigned short locationid)
{
	ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
	ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(locationid)));
}
