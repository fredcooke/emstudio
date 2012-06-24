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

#include "comsettings.h"


ComSettings::ComSettings(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.cancelPushButton,SIGNAL(clicked()),this,SIGNAL(cancelClicked()));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SIGNAL(saveClicked()));
}

ComSettings::~ComSettings()
{
}
void ComSettings::setInterByteDelay(int delay)
{
	ui.interByteDelaySpinBox->setValue(delay);
}
void ComSettings::setBaud(int baud)
{
	ui.baudRateLineEdit->setText(QString::number(baud));
}
void ComSettings::setComPort(QString port)
{
	ui.portNameLineEdit->setText(port);
}
int ComSettings::getInterByteDelay()
{
	return ui.interByteDelaySpinBox->value();
}
int ComSettings::getBaud()
{
	return ui.baudRateLineEdit->text().toInt();
}
QString ComSettings::getComPort()
{
	return ui.portNameLineEdit->text();
}
