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
	QString getComPort() { return m_comPort; }
	void setComPort(QString port) { m_comPort = port; }
	int getBaud() { return m_baud; }
	void setBaud(int baud) { m_baud = baud; }
	int getInterByteDelay() { return m_interByteDelay; }
	void setInterByteDelay(int delay) { m_interByteDelay = delay; }
private:
	int m_interByteDelay;
	int m_baud;
	QString m_comPort;
	Ui::ComSettings ui;
};

#endif // COMSETTINGS_H
