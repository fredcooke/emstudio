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
	
private:
	Ui::TableView2D ui;
};

#endif // TABLEVIEW2D_H
