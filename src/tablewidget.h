#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QKeyEvent>
class TableWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit TableWidget(QWidget *parent = 0);
	void addHotkey(int key,Qt::KeyboardModifier modifier);
private:
	QList<QPair<int,Qt::KeyboardModifiers> > m_hotkeyList;
protected:
	void keyPressEvent(QKeyEvent *event);
signals:
	void hotKeyPressed(int key,Qt::KeyboardModifiers modifier);
public slots:
	
};

#endif // TABLEWIDGET_H
