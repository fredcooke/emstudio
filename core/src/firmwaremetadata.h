#ifndef FIRMWAREMETADATA_H
#define FIRMWAREMETADATA_H

#include <QWidget>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include "ui_firmwaremetadata.h"


class FirmwareMetaData : public QWidget
{
	Q_OBJECT
	
public:
	explicit FirmwareMetaData(QWidget *parent = 0);
	~FirmwareMetaData();
	void setInterrogationData(QMap<QString,QString> datamap);
protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::FirmwareMetaData ui;
signals:
	void windowHiding(QMdiSubWindow *parent);
};

#endif // FIRMWAREMETADATA_H
