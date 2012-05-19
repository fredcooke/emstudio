#include "rawdatadisplay.h"


RawDataDisplay::RawDataDisplay(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.sendCommandTableWidget->setColumnCount(4);
	ui.sendCommandTableWidget->setColumnWidth(0,50);
	ui.sendCommandTableWidget->setColumnWidth(1,100);
	ui.sendCommandTableWidget->setColumnWidth(2,100);
	ui.sendCommandTableWidget->setColumnWidth(3,500);

}

RawDataDisplay::~RawDataDisplay()
{
}
