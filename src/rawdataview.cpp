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

#include "rawdataview.h"


RawDataView::RawDataView(bool isram, bool isflash,QWidget *parent)
{
	Q_UNUSED(parent)
	ui.setupUi(this);
	connect(ui.saveFlashPushButton,SIGNAL(clicked()),this,SLOT(saveFlashButtonClicked()));
	connect(ui.saveRamPushButton,SIGNAL(clicked()),this,SLOT(saveRamButtonClicked()));
	connect(ui.loadFlashPushButton,SIGNAL(clicked()),this,SLOT(loadFlashButtonClicked()));
	connect(ui.loadRamPushButton,SIGNAL(clicked()),this,SLOT(loadRamButtonClicked()));
	if (!isram)
	{
		//Is only flash
		ui.saveRamPushButton->setEnabled(false);
		ui.saveRamPushButton->setVisible(false);
		ui.loadRamPushButton->setEnabled(false);
		ui.loadRamPushButton->setVisible(false);
	}
	else if (!isflash)
	{
		//Is only ram
		ui.saveFlashPushButton->setEnabled(false);
		ui.saveFlashPushButton->setVisible(false);
		ui.loadFlashPushButton->setEnabled(false);
		ui.loadFlashPushButton->setVisible(false);
	}
	else
	{
		//Is both ram and flash, leave both sets of buttons enabled.
	}
}
bool RawDataView::setData(unsigned short locationid,QByteArray data)
{
	m_locationId = locationid;
	ui.hexEditor->setData(data);
	ui.locationIdLabel->setText("0x" + QString::number(locationid,16).toUpper());
	return true;
}
void RawDataView::loadRamButtonClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the block from ram, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Ok";
		emit reloadData(m_locationId,true);
	}
	else
	{
		qDebug() << "Not ok";
	}

}

void RawDataView::loadFlashButtonClicked()
{
	if (QMessageBox::information(0,"Warning","Doing this will reload the block from flash, and wipe out any changes you may have made. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Ok";
		emit reloadData(m_locationId,false);
	}
	else
	{
		qDebug() << "Not ok";
	}
}

RawDataView::~RawDataView()
{
}
void RawDataView::saveFlashButtonClicked()
{
	emit saveData(m_locationId,ui.hexEditor->data(),1); //0 for RAM, 1 for flash.
}

void RawDataView::saveRamButtonClicked()
{
	emit saveData(m_locationId,ui.hexEditor->data(),0); //0 for RAM, 1 for flash.
}
