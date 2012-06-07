/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
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
	ui.locationIdInfoTableWidget->setColumnCount(16);
	ui.locationIdInfoTableWidget->setColumnWidth(0,80);
	ui.locationIdInfoTableWidget->setColumnWidth(1,80);
	ui.locationIdInfoTableWidget->setColumnWidth(2,80);
	ui.locationIdInfoTableWidget->setColumnWidth(3,80);
	ui.locationIdInfoTableWidget->setColumnWidth(4,80);
	ui.locationIdInfoTableWidget->setColumnWidth(5,100);
	ui.locationIdInfoTableWidget->setColumnWidth(6,100);
	ui.locationIdInfoTableWidget->setColumnWidth(7,80);
	ui.locationIdInfoTableWidget->setColumnWidth(8,80);
	ui.locationIdInfoTableWidget->setColumnWidth(9,80);
	ui.locationIdInfoTableWidget->setColumnWidth(10,80);
	ui.locationIdInfoTableWidget->setColumnWidth(11,80);
	ui.locationIdInfoTableWidget->setColumnWidth(12,120);
	ui.locationIdInfoTableWidget->setColumnWidth(13,120);
	ui.locationIdInfoTableWidget->setColumnWidth(14,120);
	ui.locationIdInfoTableWidget->setColumnWidth(15,120);

	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Flags"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("RamPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("FlashPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("RamAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("FlashAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("Size"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Has Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Is Ram"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Flash"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Index"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Read Only"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("Is Verified"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("For Backup"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(15,new QTableWidgetItem("Table Type"));

	connect(ui.locationIdInfoTableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(locationInfoWidgetDoubleClicked(int,int)));
	connect(ui.checkSyncButton,SIGNAL(clicked()),this,SIGNAL(checkSync()));

}
void EmsInfoView::setLocalFlash(bool dirty)
{
	if (dirty)
	{
		ui.localFlashLineEdit->setText("DIRTY");
	}
	else
	{
		ui.localFlashLineEdit->setText("CLEAN");
	}
}

void EmsInfoView::setLocalRam(bool dirty)
{
	if (dirty)
	{
		ui.localRamLineEdit->setText("DIRTY");
	}
	else
	{
		ui.localRamLineEdit->setText("CLEAN");
	}
}

void EmsInfoView::setDeviceFlash(bool dirty)
{
	if (dirty)
	{
		ui.deviceFlashLineEdit->setText("DIRTY");
	}
	else
	{
		ui.deviceFlashLineEdit->setText("CLEAN");
	}
}

void EmsInfoView::locationInfoWidgetDoubleClicked(int row, int column)
{
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
	if (ui.locationIdInfoTableWidget->item(row,15)->text() == "2d Table")
	{
		type = 1;
	}
	else if (ui.locationIdInfoTableWidget->item(row,15)->text() == "Lookup")
	{
		type = 2;
	}
	else if (ui.locationIdInfoTableWidget->item(row,15)->text() == "Main Table")
	{
		type = 3;
	}
	else if (ui.locationIdInfoTableWidget->item(row,15)->text() == "Config")
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

void EmsInfoView::locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size)
{
	/*ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Parent"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("RamPage"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("FlashPage"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("RamAddress"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("FlashAddress"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("Size"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("Has Parent"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Is Ram"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Is Flash"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Index"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Read Only"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Verified"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("For Backup"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("Table Type"));*/
	qDebug() << "Location ID Info for:" << QString::number(locationid,16);
	bool found = false;
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
	ui.locationIdInfoTableWidget->setRowCount(ui.locationIdInfoTableWidget->rowCount()+1);
	foundi = ui.locationIdInfoTableWidget->rowCount()-1;
	ui.locationIdInfoTableWidget->setItem(foundi,0,new QTableWidgetItem("0x" + QString::number(locationid,16).toUpper()));
	for (int i=1;i<16;i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,i,new QTableWidgetItem(""));
	}
	//}
	ui.locationIdInfoTableWidget->item(foundi,1)->setText(QString::number(rawFlags));

	if (flags.contains(FreeEmsComms::BLOCK_IS_2D_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("2d Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_LOOKUP_DATA))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Lookup");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_MAIN_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Main Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_CONFIGURATION))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Config");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_INDEXABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,11)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("False");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_READ_ONLY))
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_GETS_VERIFIED))
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_FOR_BACKUP_RESTORE))
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,3)->setText("0x" + QString::number(rampage,16).toUpper());
		ui.locationIdInfoTableWidget->item(foundi,5)->setText("0x" + QString::number(ramaddress,16).toUpper());
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,4)->setText("0x" + QString::number(flashpage,16).toUpper());
		ui.locationIdInfoTableWidget->item(foundi,6)->setText("0x" + QString::number(flashaddress,16).toUpper());
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
	{
		ui.locationIdInfoTableWidget->item(foundi,8)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,2)->setText(QString::number(parent));
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,8)->setText("False");
	}
	ui.locationIdInfoTableWidget->item(foundi,7)->setText(QString::number(size));
	//Q_UNUSED(locationid)
	//Q_UNUSED(flags)
	//Q_UNUSED(parent)
	//Q_UNUSED(rampage)
	//Q_UNUSED(flashpage)
	//Q_UNUSED(ramaddress)
	//Q_UNUSED(flashaddress)
	//Q_UNUSED(size)
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
