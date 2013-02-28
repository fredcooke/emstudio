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

#include "tablewidgetdelegate.h"
#include <QDebug>
TableWidgetDelegate::TableWidgetDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void TableWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index)
	if (option.state & QStyle::State_Selected)
	{
		//QBrush brush = index.data(Qt::BackgroundRole).value();
		//painter->setPen(brush.color());
		QBrush brush;
		brush.setColor(QColor::fromRgb(100,100,255));
		brush.setStyle(Qt::SolidPattern);
		painter->setBrush(brush);
		painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);

	}
	else
	{
		if (index.data(Qt::UserRole+1).toBool())
		{
			double val = index.data(Qt::UserRole+2).toDouble();
			//Val will be 0-100%, as to what percentage it needs to be highlighted.

			QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
			painter->setBrush(brush);
			painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
			brush.setStyle(Qt::NoBrush);
			painter->setBrush(brush);
			QPen p = painter->pen();
			p.setWidth(5);
			p.setColor(QColor::fromRgb(255 * val,255 * val,255 * val));
			painter->setPen(p);
			painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
		}
		else
		{
			QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
			painter->setBrush(brush);
			painter->setBackground(brush.color());
			painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
			brush.setStyle(Qt::NoBrush);
			brush.setColor(QColor::fromRgb(0,0,0));
			painter->setBrush(brush);
			painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
		}

	}
	painter->setPen(QColor::fromRgb(0,0,0));
	painter->drawText(option.rect.x() + 2,option.rect.y() + (option.rect.height()/2.0),index.data().toString());
}
