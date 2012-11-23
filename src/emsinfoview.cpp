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
#include "emsinfoview.h"
#include <QMdiSubWindow>
#include <QMessageBox>
EmsInfoView::EmsInfoView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	/*ui.locationIdInfoTableWidget->setColumnCount(17);

	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Table Name"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Flags"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("RamPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("FlashPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("RamAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("FlashAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Size"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Has Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Ram"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Flash"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Index"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("Is Read Only"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("Is Verified"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(15,new QTableWidgetItem("For Backup"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(16,new QTableWidgetItem("Table Type"));*/

	connect(ui.locationIdInfoTableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(locationInfoWidgetDoubleClicked(int,int)));
	connect(ui.checkSyncButton,SIGNAL(clicked()),this,SIGNAL(checkSync()));

	//ui.locationIdInfoTableWidget->set

	//ui.locationIdInfoTableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	//QAction *hexAction = new QAction(this);
	//hexAction->setText("Hex View");
	//connect(hexAction,SIGNAL(triggered()),this,SLOT(hexViewClicked()));
	//QAction *normalAction= new QAction(this);
	//normalAction->setText("Normal View");
	//connect(normalAction,SIGNAL(triggered()),this,SLOT(normalViewClicked()));

	//ui.locationIdInfoTableWidget->addAction(normalAction);
	//ui.locationIdInfoTableWidget->addAction(hexAction);

}
void EmsInfoView::normalViewClicked()
{
	if (ui.locationIdInfoTableWidget->selectedItems().size() == 0)
	{
		return;
	}
	int row = ui.locationIdInfoTableWidget->selectedItems()[0]->row();
	QString num = ui.locationIdInfoTableWidget->item(row,0)->text().split("x")[1];
	bool ok = false;
	//int locid = num.toInt(&ok,16);
	if (!ok)
	{
		qDebug() << "Numeric conversion failed for:" << num;
		return;
	}
	//displayLocationId(locid,true,0); //0 for normal view, 1 for hex view
}

void EmsInfoView::hexViewClicked()
{
	if (ui.locationIdInfoTableWidget->selectedItems().size() == 0)
	{
		return;
	}
	int row = ui.locationIdInfoTableWidget->selectedItems()[0]->row();
	QString num = ui.locationIdInfoTableWidget->item(row,0)->text().split("x")[1];
	bool ok = false;
	//int locid = num.toInt(&ok,16);
	if (!ok)
	{
		qDebug() << "Numeric conversion failed for:" << num;
		return;
	}
	//displayLocationId(locid,true,1); //0 for normal view, 1 for hex view
}

void EmsInfoView::setLocalRam(bool dirty)
{
	if (dirty)
	{
		ui.localRamLineEdit->setText("Local changes not saved to RAM");
	}
	else
	{
		ui.localRamLineEdit->setText("Local changes in device RAM");
	}
}
void EmsInfoView::clear()
{
	ui.locationIdInfoTableWidget->clearContents();
	ui.firmwareVersionLineEdit->clear();
	ui.interfaceVersionLineEdit->clear();
	ui.locationIdInfoTableWidget->setRowCount(0);
}

void EmsInfoView::setDeviceFlash(bool dirty)
{
	if (dirty)
	{
		ui.deviceFlashLineEdit->setText("Local changes not saved to FLASH");
	}
	else
	{
		ui.deviceFlashLineEdit->setText("Local changes in FLASH");
	}
}

void EmsInfoView::locationInfoWidgetDoubleClicked(int row, int column)
{
	qDebug() << "Double click";
	Q_UNUSED(column)
	if (ui.locationIdInfoTableWidget->rowCount() <= row)
	{
		return;
	}
	QString num = ui.locationIdInfoTableWidget->item(row,0)->text().split("x")[1];
	bool ok = false;
	int locid = num.toInt(&ok,16);
	if (!ok)
	{
		qDebug() << "Numeric conversion failed for:" << num;
		return;
	}
	DataType type = DATA_UNDEFINED;
	if (ui.locationIdInfoTableWidget->item(row,2)->text() == "2D Table")
	{
		type = DATA_TABLE_2D;
	}
	else if (ui.locationIdInfoTableWidget->item(row,2)->text() == "3D Table")
	{
		type = DATA_TABLE_3D;
	}
	else if (ui.locationIdInfoTableWidget->item(row,2)->text() == "Lookup Table")
	{
		type = DATA_TABLE_LOOKUP;
	}
	else if (ui.locationIdInfoTableWidget->item(row,2)->text() == "Configuration")
	{
		type = DATA_CONFIG;
	}


	/*if (ui.locationIdInfoTableWidget->item(row,9)->text().toLower() == "true" && ui.locationIdInfoTableWidget->item(row,10)->text().toLower() == "true")
	{
		if (QMessageBox::question(0,"Ram or flash?","Do you wish to open the RAM? (If not, flash will be opened)",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
		{
			//Is Ram
			emit displayLocationId(locid,true,type);
		}
		else
		{
			//is flash
			emit displayLocationId(locid,false,type);
		}
	}
	else if (ui.locationIdInfoTableWidget->item(row,9)->text().toLower() == "true")
	{
		//Is Ram
		emit displayLocationId(locid,true,type);
	}
	else if (ui.locationIdInfoTableWidget->item(row,10)->text().toLower() == "true")
	{
		//is flash
		emit displayLocationId(locid,false,type);
	}*/
	displayLocationId(locid,true,type);
}

void EmsInfoView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void EmsInfoView::locationIdInfo(unsigned short locationid,QString title,MemoryLocationInfo info)
{
	/*ui.locationIdInfoTableWidget->setColumnCount(17);

	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Table Name"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Flags"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("RamPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("FlashPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("RamAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("FlashAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Size"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Has Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Ram"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Flash"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Index"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("Is Read Only"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("Is Verified"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(15,new QTableWidgetItem("For Backup"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(16,new QTableWidgetItem("Table Type"));
*/
	ui.locationIdInfoTableWidget->setColumnCount(3+info.propertymap.size());
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Table Name"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Type"));
	for (int i=0;i<info.propertymap.size();i++)
	{
		ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3+i,new QTableWidgetItem(info.propertymap[i].first));
	}
	Q_UNUSED(info)
	qDebug() << "Location ID Info for:" << "0x" + QString::number(locationid,16).toUpper();
	int foundi = -1;
	bool ok = false;
	bool found = false;
	for (int i=0;i<ui.locationIdInfoTableWidget->rowCount();i++)
	{
		unsigned short test = ui.locationIdInfoTableWidget->item(i,0)->text().split("x")[1].toInt(&ok,16);
		if (test > locationid)
		{
			foundi = i;
			found = true;
			ui.locationIdInfoTableWidget->insertRow(i);
			break;
		}
	}
	if (!found)
	{
		ui.locationIdInfoTableWidget->setRowCount(ui.locationIdInfoTableWidget->rowCount()+1);
		foundi = ui.locationIdInfoTableWidget->rowCount()-1;
	}
	ui.locationIdInfoTableWidget->setItem(foundi,0,new QTableWidgetItem("0x" + QString::number(locationid,16).toUpper()));
	if (info.type == DATA_TABLE_2D)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2,new QTableWidgetItem("2D Table"));
	}
	else if (info.type == DATA_TABLE_3D)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2,new QTableWidgetItem("3D Table"));
	}
	else if (info.type == DATA_TABLE_LOOKUP)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2,new QTableWidgetItem("Lookup Table"));
	}
	else if (info.type == DATA_CONFIG)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2,new QTableWidgetItem("Configuration"));
	}
	else
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2,new QTableWidgetItem("Undefined"));
	}

	/*for (int i=1;i<17;i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,i,new QTableWidgetItem(""));
	}*/
	//}
	ui.locationIdInfoTableWidget->setItem(foundi,1,new QTableWidgetItem(title));
	//ui.locationIdInfoTableWidget->item(foundi,2)->setText("0x" + QString::number(rawFlags,16).toUpper());
	for (int i=0;i<info.propertymap.size();i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,3+i,new QTableWidgetItem(info.propertymap[i].second));
	}

	ui.locationIdInfoTableWidget->resizeColumnsToContents();
}

void EmsInfoView::setInterfaceVersion(QString version)
{
	ui.interfaceVersionLineEdit->setText(version);
}

void EmsInfoView::setFirmwareVersion(QString firmware)
{
	ui.firmwareVersionLineEdit->setText(firmware);
}

EmsInfoView::~EmsInfoView()
{
    //delete ui;
}
