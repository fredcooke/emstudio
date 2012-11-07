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
	int locid = num.toInt(&ok,16);
	if (!ok)
	{
		qDebug() << "Numeric conversion failed for:" << num;
		return;
	}
	displayLocationId(locid,true,0); //0 for normal view, 1 for hex view
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
	int locid = num.toInt(&ok,16);
	if (!ok)
	{
		qDebug() << "Numeric conversion failed for:" << num;
		return;
	}
	displayLocationId(locid,true,1); //0 for normal view, 1 for hex view
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
	int type = 0;
	if (ui.locationIdInfoTableWidget->item(row,16)->text() == "2d Table")
	{
		type = 1;
	}
	else if (ui.locationIdInfoTableWidget->item(row,16)->text() == "Lookup")
	{
		type = 2;
	}
	else if (ui.locationIdInfoTableWidget->item(row,16)->text() == "Main Table")
	{
		type = 3;
	}
	else if (ui.locationIdInfoTableWidget->item(row,16)->text() == "Config")
	{
		type = 4;
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
	ui.locationIdInfoTableWidget->setColumnCount(2+info.propertymap.size());
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Table Name"));
	for (int i=0;i<info.propertymap.size();i++)
	{
		ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2+i,new QTableWidgetItem(info.propertymap[i].first));
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
	/*for (int i=1;i<17;i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,i,new QTableWidgetItem(""));
	}*/
	//}
	ui.locationIdInfoTableWidget->setItem(foundi,1,new QTableWidgetItem(title));
	//ui.locationIdInfoTableWidget->item(foundi,2)->setText("0x" + QString::number(rawFlags,16).toUpper());
	for (int i=0;i<info.propertymap.size();i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,2+i,new QTableWidgetItem(info.propertymap[i].second));
	}

	ui.locationIdInfoTableWidget->resizeColumnsToContents();
}

void EmsInfoView::locationIdInfo(unsigned short locationid,QString title,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size)
{
	qDebug() << "Location ID Info for:" << "0x" + QString::number(locationid,16).toUpper();
	//bool found = false;
	int foundi = -1;

	/*for (int i=0;i<ui.locationIdInfoTableWidget->rowCount();i++)
	{
		if (ui.locationIdInfoTableWidget->item(i,0)->text().toInt() == locationid)
		{
			foundi = i;
			found = true;
		}
	}
	if (!found)
	{*/
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
	for (int i=1;i<17;i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,i,new QTableWidgetItem(""));
	}
	//}
	ui.locationIdInfoTableWidget->item(foundi,1)->setText(title);
	ui.locationIdInfoTableWidget->item(foundi,2)->setText("0x" + QString::number(rawFlags,16).toUpper());

	if (flags.contains(FreeEmsComms::BLOCK_IS_2D_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,16)->setText("2d Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_LOOKUP_DATA))
	{
		ui.locationIdInfoTableWidget->item(foundi,16)->setText("Lookup");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_MAIN_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,16)->setText("Main Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_CONFIGURATION))
	{
		ui.locationIdInfoTableWidget->item(foundi,16)->setText("Config");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_INDEXABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("False");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_READ_ONLY))
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_GETS_VERIFIED))
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_FOR_BACKUP_RESTORE))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,4)->setText("0x" + QString::number(rampage,16).toUpper());
		ui.locationIdInfoTableWidget->item(foundi,6)->setText("0x" + QString::number(ramaddress,16).toUpper());
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		ui.locationIdInfoTableWidget->item(foundi,11)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,5)->setText("0x" + QString::number(flashpage,16).toUpper());
		ui.locationIdInfoTableWidget->item(foundi,7)->setText("0x" + QString::number(flashaddress,16).toUpper());
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,11)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,3)->setText("0x" + QString::number(parent,16).toUpper());
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("False");
	}
	ui.locationIdInfoTableWidget->item(foundi,8)->setText(QString::number(size));
	//Q_UNUSED(locationid)
	//Q_UNUSED(flags)
	//Q_UNUSED(parent)
	//Q_UNUSED(rampage)
	//Q_UNUSED(flashpage)
	//Q_UNUSED(ramaddress)
	//Q_UNUSED(flashaddress)
	//Q_UNUSED(size)
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
