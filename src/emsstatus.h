#ifndef EMSSTATUS_H
#define EMSSTATUS_H

#include <QDockWidget>
#include "ui_emsstatus.h"

class EmsStatus : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit EmsStatus(QWidget *parent = 0);
	~EmsStatus();
signals:
	void dockRequested();
private slots:
	void dockButtonClicked();
private:
	Ui::EmsStatus ui;
};

#endif // EMSSTATUS_H
