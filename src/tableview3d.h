#ifndef TABLEVIEW3D_H
#define TABLEVIEW3D_H

#include <QWidget>
#include "ui_tableview3d.h"

class TableView3D : public QWidget
{
	Q_OBJECT
	
public:
	explicit TableView3D(QWidget *parent = 0);
	~TableView3D();
	void passData(unsigned short locationid,QByteArray data,int physicallocation);
private:
	unsigned short m_locationId;
	Ui::TableView3D ui;
};

#endif // TABLEVIEW3D_H
