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

#include "flagview.h"
#include <QMdiSubWindow>
#include "QsLog.h"

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
		}
		else
		{
			QLOG_TRACE() << "unknown value type:" << i.value().typeName();
		}
		i++;
	}
}

void FlagView::passDecoder(DataPacketDecoder *decoder)
{
	Q_UNUSED(decoder)
}
