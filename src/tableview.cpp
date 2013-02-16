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
		m_nameToIndexMap[dataPacketDecoder->getField(i).name()] = i;
		ui.tableWidget->setItem(i,0,new QTableWidgetItem(dataPacketDecoder->getField(i).name()));
		ui.tableWidget->setItem(i,1,new QTableWidgetItem("0"));
		ui.tableWidget->setItem(i,2,new QTableWidgetItem(dataPacketDecoder->getField(i).description()));
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
			ui.tableWidget->item(m_nameToIndexMap[i.key()],1)->setText(valstr);
		}
		//qDebug() << i.key() << m_nameToIndexMap[i.key()] << i.value();
		i++;
	}

}
