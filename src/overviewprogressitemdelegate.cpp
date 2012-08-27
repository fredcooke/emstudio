#include "overviewprogressitemdelegate.h"
#include <QPainter>
#include <QDebug>
#include <QPen>
#include <QBrush>
OverviewProgressItemDelegate::OverviewProgressItemDelegate(QObject *parent) : QItemDelegate(parent)
{
}
void OverviewProgressItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 1)
	{
		QStringList valuelist = index.data().toStringList();
		int ivalue = valuelist[0].toInt();
		int imax = valuelist[1].toInt();
		QPen pen = painter->pen();
		QBrush brush =  painter->brush();
		painter->setPen(QColor::fromRgb(0,0,255));
		//painter->setBrush(Qt::SolidPattern);
		//qDebug() << "IValue:" << ivalue << "IMax:" << imax;
		if (imax ==0 && ivalue == 0)
		{
		}
		else
		{
			painter->drawRoundedRect(option.rect.x(),option.rect.y() + ((option.rect.height() * 0.4) / 2.0),(((double)ivalue / (double)imax) * option.rect.width())-1,(option.rect.height() * 0.6),10,10);
		}
		painter->setPen(pen);
		painter->setBrush(brush);
		//painter->restore();
	}
	else
	{
		QItemDelegate::paint(painter,option,index);
	}
}
