#ifndef TABLEVIEWNEW3D_H
#define TABLEVIEWNEW3D_H

#include <QWidget>

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

private:
	void paintEvent (QPaintEvent *evt);
	QList<double> xaxis;
	QList<double> yaxis;
	QList<QList<double> > values;
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
	
public slots:
	
};

#endif // TABLEVIEWNEW3D_
