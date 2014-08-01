#ifndef TABLEVIEWNEW3D_H
#define TABLEVIEWNEW3D_H

#include <QWidget>
#include <QTimer>

class TableViewNew3D : public QWidget
{
	Q_OBJECT
public:
	explicit TableViewNew3D(QWidget *parent = 0);
	void clear();
	void setRowCount(int count);
	void setColumnCount(int count);
	void setItem(int row,int column,QString text);
	void rebuildTable();
	int rowCount() { return m_rowCount; }
	int columnCount() { return m_columnCount; }
	QList<QPoint> selectedItems() { QList<QPoint> l; l.append(currentCell); return l; }
	QString item(int row,int column);
	void setXAxis(int index,QString number);
	void setYAxis(int index,QString number);
	QString xAxis(int index);
	QString yAxis(int index);
	void addHotkey(int key,Qt::KeyboardModifier modifier);
	void setTracingValue(double x,double y);
	void setTracingEnabled(bool enabled) { m_traceEnabled = enabled; update(); }
private:
	QTimer *m_updateTimer;
	double m_traceX;
	double m_traceY;
	bool m_traceEnabled;
	QList<QPair<int,Qt::KeyboardModifier> > m_hotkeyMap;
	double m_itemHeight;
	double m_itemWidth;
	void resizeEvent(QResizeEvent *evt);
	void paintEvent (QPaintEvent *evt);
	QList<QString> xaxis;
	QList<QString> yaxis;
	QList<QList<QString> > values;
	void mouseMoveEvent(QMouseEvent *evt);
	void mousePressEvent(QMouseEvent *evt);
	void mouseReleaseEvent(QMouseEvent *evt);
	int m_x;
	int m_y;
	QList<QList<int> > m_highlightList;
	void keyPressEvent(QKeyEvent *evt);
	QPoint currentCell;
	QPoint startSelectCell;
	bool multiSelect;
	int m_rowCount;
	int m_columnCount;
	void rebuildtable();
signals:
	void hotKeyPressed(int key,Qt::KeyboardModifier modifier);
public slots:
	
};

#endif // TABLEVIEWNEW3D_
