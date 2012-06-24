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


RawDataView::RawDataView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.saveChangesButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
}
void RawDataView::setData(unsigned short locationid,QByteArray data,bool isram)
{
	m_isRam = isram;
	m_locationId = locationid;
	ui.hexEditor->setData(data);
	ui.locationIdLabel->setText("0x" + QString::number(locationid,16).toUpper());
}

RawDataView::~RawDataView()
{
}

void RawDataView::saveButtonClicked()
{
	if (m_isRam)
	{
		emit saveData(m_locationId,ui.hexEditor->data(),0); //0 for RAM, 1 for flash.
	}
	else
	{
		emit saveData(m_locationId,ui.hexEditor->data(),1); //0 for RAM, 1 for flash.
	}
}
