#ifndef FREEEMSCOMMS_H
#define FREEEMSCOMMS_H

#include <QThread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <vector>

class FreeEmsComms : public QThread
{
	Q_OBJECT
public:
	FreeEmsComms(QObject *parent = 0);
	void setPort(QString portname);
	void setBaud(int baudrate);
protected:
	void run();
private:
	QString m_portName;
	int m_baud;
	void parseBuffer(QByteArray buffer);
	int m_portHandle;
	int openPort(QString portName,int baudrate);
signals:
	void payloadReceived(QByteArray header,QByteArray payload);
public slots:

};

#endif // FREEEMSCOMMS_H
