#ifndef TABLEVIEW2D_H
#define TABLEVIEW2D_H

#include <QWidget>
#include <QResizeEvent>
#include "ui_tableview2d.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
class TableView2D : public QWidget
{
	Q_OBJECT
	
public:
	explicit TableView2D(QWidget *parent = 0);
	~TableView2D();
	void passData(unsigned short locationid,QByteArray data,int physicallocation);
private:
	QwtPlotCurve *curve;
	QVector<QPointF> samples;
	Ui::TableView2D ui;
	unsigned short m_locationid;
	int m_physicalid;
	unsigned short currentvalue;
protected:
	void resizeEvent(QResizeEvent *evt);
private slots:
	void saveClicked();
	void loadClicked();
	void tableCellChanged(int row,int column);
	void tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn);
signals:
	void saveToFlash(unsigned short locationid);
	void saveData(unsigned short locationid,QByteArray data,int phyiscallocation);
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	void reloadTableData(unsigned short locationid);
};

#endif // TABLEVIEW2D_H
