#ifndef FREEEMSCOMMS_H
#define FREEEMSCOMMS_H

#include <QThread>
#include <QFile>
#include <QMutex>
#include <QVariant>
#include "serialthread.h"
#include "logloader.h"
class FreeEmsComms : public QThread
{
	Q_OBJECT
public:
	enum RequestType
	{
		SERIAL_CONNECT,
		SERIAL_DISCONNECT
	};
	class RequestClass
	{
	public:
		RequestType type;
		QList<QVariant> args;
	};

	FreeEmsComms(QObject *parent = 0);
	void setPort(QString portname);
	void setBaud(int baudrate);
	void setLogFileName(QString filename);
	void connectSerial(QString port,int baud);
	void loadLog(QString filename);
	void playLog();
	void populateDataFields();
protected:
	void run();
private:
	QMutex m_reqListMutex;
	QList<RequestClass> m_reqList;
	QList<RequestClass> m_threadReqList;
	SerialThread *serialThread;
	LogLoader *logLoader;
	//void parseBuffer(QByteArray buffer);
signals:
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void error(QString msg);
public slots:
private slots:
	QPair<QByteArray,QByteArray> parseBuffer(QByteArray buffer);

};

#endif // FREEEMSCOMMS_H
