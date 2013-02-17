#ifndef EMSSTATUS_H
#define EMSSTATUS_H

#include <QDockWidget>
#include <QCloseEvent>
#include "ui_emsstatus.h"

class EmsStatus : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit EmsStatus(QWidget *parent = 0);
	~EmsStatus();
	void passData(QVariantMap data);
	void setFlag(QLineEdit *edit,bool green);
protected:
	void closeEvent(QCloseEvent *event);
signals:
	void hardResetRequest();
	void softResetRequest();
private:
	Ui::EmsStatus ui;
};

#endif // EMSSTATUS_H
