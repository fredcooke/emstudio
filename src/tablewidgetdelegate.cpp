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
			QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
			//brush.setColor(QColor::fromRgb(255,255,255));
			//brush.setColor(QColor::fromRgb(qMin(brush.color().red() + 180,255),qMin(brush.color().green() + 180 ,255),qMin(brush.color().blue() + 180,255)));
			painter->setBrush(brush);
			//painter->setBackground(brush.color());
			painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
			brush.setStyle(Qt::NoBrush);
			painter->setBrush(brush);
			QPen p = painter->pen();
			p.setWidth(5);
			p.setColor(QColor::fromRgb(255,255,255));
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
