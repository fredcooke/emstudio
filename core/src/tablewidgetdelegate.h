#ifndef TABLEWIDGETDELEGATE_H
#define TABLEWIDGETDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

class TableWidgetDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit TableWidgetDelegate(QObject *parent = 0);
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
	
public slots:
	
};

#endif // TABLEWIDGETDELEGATE_H
