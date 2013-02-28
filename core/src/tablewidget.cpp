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

#include "tablewidget.h"
#include <QDebug>

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{

}
void TableWidget::addHotkey(int key,Qt::KeyboardModifier modifier)
{
	m_hotkeyList.append(QPair<int,Qt::KeyboardModifiers>(key,modifier));
}

void TableWidget::keyPressEvent(QKeyEvent *event)
{
	if (this->selectedItems().size() == 0)
	{
		QTableWidget::keyPressEvent(event);
		return;
	}
	for (int i=0;i<m_hotkeyList.size();i++)
	{
		if (event->modifiers() != Qt::NoModifier)
		{
			if (m_hotkeyList[i].first == event->key() && m_hotkeyList[i].second & event->modifiers())
			{
				qDebug() << "found event";
				emit hotKeyPressed(event->key(),event->modifiers());
				return;
			}
		}
		else
		{
			if (m_hotkeyList[i].first == event->key())
			{
				qDebug() << "found event2";
				emit hotKeyPressed(event->key(),event->modifiers());
				return;
			}
		}
	}
	//qDebug() << "Unhandled event:" << event->key() << event->modifiers();
	event->ignore();
	QTableWidget::keyPressEvent(event);
}
