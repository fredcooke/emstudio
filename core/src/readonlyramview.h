#ifndef READONLYRAMVIEW_H
#define READONLYRAMVIEW_H

#include <QWidget>
#include <QTimer>
#include "ui_readonlyramview.h"
#include "headers.h"
class ReadOnlyRamView : public QWidget
{
	Q_OBJECT
	
public:
	explicit ReadOnlyRamView(QWidget *parent = 0);
	~ReadOnlyRamView();
	void passData(unsigned short locationid,QByteArray data,QList<ReadOnlyRamData> metadata);
	QTimer *readRamTimer;
	double calcAxis(unsigned short val,QList<QPair<QString,double> > metadata);
private:
	unsigned short m_locationId;
	Ui::ReadOnlyRamView ui;
private slots:
	void readRamTimerTick();
signals:
	void readRamLocation(unsigned short locationid);
};

#endif // READONLYRAMVIEW_H
