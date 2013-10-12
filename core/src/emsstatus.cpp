/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

#include "emsstatus.h"
#include <QVariant>
#include <QDebug>
EmsStatus::EmsStatus(QWidget *parent) : QDockWidget(parent)
{
	ui.setupUi(this);
	connect(ui.hardResetPushButton,SIGNAL(clicked()),this,SIGNAL(hardResetRequest()));
	connect(ui.softResetPushButton,SIGNAL(clicked()),this,SIGNAL(softResetRequest()));
	setFlag(ui.combustionSyncLineEdit,false);
	setFlag(ui.crankSyncLineEdit,false);
	setFlag(ui.camSyncLineEdit,false);
	setFlag(ui.lastPeriodValidLineEdit,false);
	setFlag(ui.lastTimeValidLineEdit,false);
}
void EmsStatus::closeEvent(QCloseEvent *event)
{
	event->ignore();
	this->hide();
}
EmsStatus::~EmsStatus()
{
}
void EmsStatus::setFlag(QLineEdit *edit,bool green)
{
	if (green)
	{
		if (edit->palette().color(QPalette::Base).red() == 255)
		{
			QPalette pal = edit->palette();
			pal.setColor(QPalette::Base,QColor::fromRgb(100,255,100));
			edit->setPalette(pal);
		}
	}
	else
	{
		if (edit->palette().color(QPalette::Base).green() == 255)
		{
			QPalette pal = edit->palette();
			pal.setColor(QPalette::Base,QColor::fromRgb(255,50,50));
			edit->setPalette(pal);
		}
	}
}

void EmsStatus::passData(QVariantMap data)
{

	unsigned char decoderFlags = data["decoderFlags"].toInt();
	setFlag(ui.combustionSyncLineEdit,decoderFlags & 0x01);
	setFlag(ui.crankSyncLineEdit,decoderFlags & 0x02);
	setFlag(ui.camSyncLineEdit,decoderFlags & 0x04);
	setFlag(ui.lastPeriodValidLineEdit,decoderFlags & 0x08);
	setFlag(ui.lastTimeValidLineEdit,decoderFlags & 0x010);

}
