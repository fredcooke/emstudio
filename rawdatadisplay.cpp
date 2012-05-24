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
