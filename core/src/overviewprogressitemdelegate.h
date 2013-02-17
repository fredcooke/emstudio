#ifndef OVERVIEWPROGRESSITEMDELEGATE_H
#define OVERVIEWPROGRESSITEMDELEGATE_H

#include <QItemDelegate>

class OverviewProgressItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit OverviewProgressItemDelegate(QObject *parent = 0);
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
	
public slots:
	
};

#endif // OVERVIEWPROGRESSITEMDELEGATE_H
