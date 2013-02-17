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
