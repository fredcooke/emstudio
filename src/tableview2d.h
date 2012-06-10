#ifndef TABLEVIEW2D_H
#define TABLEVIEW2D_H

#include <QWidget>
#include "ui_tableview2d.h"
class TableView2D : public QWidget
{
	Q_OBJECT
	
public:
	explicit TableView2D(QWidget *parent = 0);
	~TableView2D();
	void passData(unsigned short locationid,QByteArray data,int physicallocation);
private:
	Ui::TableView2D ui;
	unsigned short m_locationid;
	int m_physicalid;
private slots:
	void saveClicked();
signals:
	void saveData(unsigned short locationid,QByteArray data,int phyiscallocation);
};

#endif // TABLEVIEW2D_H
