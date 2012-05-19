#ifndef COMSETTINGS_H
#define COMSETTINGS_H

#include <QWidget>
#include "ui_comsettings.h"

class ComSettings : public QWidget
{
	Q_OBJECT

public:
	Q_PROPERTY(QString comport READ getComPort WRITE setComPort)
	explicit ComSettings(QWidget *parent = 0);
	~ComSettings();
	QString getComPort();
	void setComPort(QString port);
	int getBaud();
	void setBaud(int baud);
	int getInterByteDelay();
	void setInterByteDelay(int delay);
private:
	Ui::ComSettings ui;
signals:
	void saveClicked();
	void cancelClicked();
};

#endif // COMSETTINGS_H
