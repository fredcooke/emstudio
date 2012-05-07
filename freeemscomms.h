#ifndef FREEEMSCOMMS_H
#define FREEEMSCOMMS_H

#include <QThread>
#include <QFile>
#include "serialthread.h"
#include "logloader.h"
class FreeEmsComms : public QThread
{
	Q_OBJECT
public:
	FreeEmsComms(QObject *parent = 0);
	void setPort(QString portname);
	void setBaud(int baudrate);
	void setLogFileName(QString filename);
	void connectSerial();
	void loadLog(QString filename);
	void playLog();
	void populateDataFields();
protected:
	void run();
private:
	SerialThread *serialThread;
	LogLoader *logLoader;
	//void parseBuffer(QByteArray buffer);
signals:
	void payloadReceived(QByteArray header,QByteArray payload);
public slots:
private slots:
	void parseBuffer(QByteArray buffer);

};

#endif // FREEEMSCOMMS_H
