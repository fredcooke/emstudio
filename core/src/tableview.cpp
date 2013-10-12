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

#include "tableview.h"
#include <QMdiSubWindow>

TableView::TableView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->setColumnCount(3);
	ui.tableWidget->setColumnWidth(0,200);
	ui.tableWidget->setColumnWidth(1,100);
	ui.tableWidget->setColumnWidth(2,300);
	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(250);

}
void TableView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void TableView::passDecoder(DataPacketDecoder *decoder)
{
	dataPacketDecoder = decoder;
	ui.tableWidget->setRowCount(dataPacketDecoder->fieldSize());
	for (int i=0;i<dataPacketDecoder->fieldSize();i++)
	{
        m_nameToIndexMap[dataPacketDecoder->getFieldName(i)] = i;
        ui.tableWidget->setItem(i,0,new QTableWidgetItem(dataPacketDecoder->getFieldName(i)));
		ui.tableWidget->setItem(i,1,new QTableWidgetItem("0"));
        ui.tableWidget->setItem(i,2,new QTableWidgetItem(dataPacketDecoder->getFieldDescription(i)));
	}
}

TableView::~TableView()
{

}
void TableView::passData(QVariantMap data)
{
	m_valueMap = data;
}

void TableView::guiUpdateTimerTick()
{
	QVariantMap::const_iterator i = m_valueMap.constBegin();
	while (i != m_valueMap.constEnd())
	{
		if (i.value().type() == QVariant::Double)
		{
			//widget->propertyMap.setProperty(i.key().toAscii(),QVariant::fromValue(i.value()));
			double val = i.value().toDouble();
			QString valstr = QString::number(val,'f',2);
			if (m_nameToIndexMap.contains(i.key()))
			{
				ui.tableWidget->item(m_nameToIndexMap[i.key()],1)->setText(valstr);
			}
		}
		//QLOG_DEBUG() << i.key() << m_nameToIndexMap[i.key()] << i.value();
		i++;
	}

}
