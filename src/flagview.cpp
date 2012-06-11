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

#include "flagview.h"
#include <QMdiSubWindow>
#include <QDebug>
FlagView::FlagView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    /*m_dataFieldList.append(DataField("coreStatusA","Duplicate",60,1,1.0));
 m_dataFieldList.append(DataField("decoderFlags","Various decoder state flags",61,1,1.0));
 m_dataFieldList.append(DataField("flaggableFlags","Flags to go with flaggables",62,2,1.0));*/
	ui.flagsTableWidget->setColumnCount(1);
	ui.flagsTableWidget->setColumnWidth(0,310);
	//ui.flagsTableWidget->setColumnWidth(1,50);
}

FlagView::~FlagView()
{
}
void FlagView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void FlagView::passData(QVariantMap data)
{
	QVariantMap::const_iterator i = data.constBegin();
	while (i != data.constEnd())
	{
		if (i.value().type() == QVariant::Bool)
		{
			//qDebug() << "Flag value:" << i.key() << i.value();
			//Flag
			bool found = false;
			for (int j=0;j<ui.flagsTableWidget->rowCount();j++)
			{
				if (ui.flagsTableWidget->item(j,0)->text() == i.key())
				{
					found = true;
					if (i.value().toBool())
					{
						ui.flagsTableWidget->item(j,0)->setBackgroundColor(QColor::fromRgb(255,100,100));
					}
					else
					{
						ui.flagsTableWidget->item(j,0)->setBackgroundColor(QColor::fromRgb(255,255,255));
					}
					//ui.flagsTableWidget->item(j,1)->setText(i.value().toBool() ? "1" : "0");
				}
			}
			if (!found)
			{
				ui.flagsTableWidget->setRowCount(ui.flagsTableWidget->rowCount()+1);
				ui.flagsTableWidget->setItem(ui.flagsTableWidget->rowCount()-1,0,new QTableWidgetItem(i.key()));
				if (i.value().toBool())
				{
					ui.flagsTableWidget->item(ui.flagsTableWidget->rowCount()-1,0)->setBackgroundColor(QColor::fromRgb(255,100,100));
				}
				else
				{
					ui.flagsTableWidget->item(ui.flagsTableWidget->rowCount()-1,0)->setBackgroundColor(QColor::fromRgb(255,255,255));
				}
				//ui.flagsTableWidget->setItem(ui.flagsTableWidget->rowCount()-1,1,new QTableWidgetItem(i.value().toBool() ? "1" : "0"));
			}

		}
		else if (i.value().type() == QVariant::Double)
		{
			//Value
			//qDebug() << "Double value:" << i.key() << i.value();
		}
		else
		{
			qDebug() << "unknown value type:" << i.value().typeName();
		}
		i++;
	}
}

void FlagView::passDecoder(DataPacketDecoder *decoder)
{
	Q_UNUSED(decoder)
}
