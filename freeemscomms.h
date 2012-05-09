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
		SERIAL_DISCONNECT,
		UPDATE_BLOCK_IN_RAM
	};
	class RequestClass
	{
	public:
		RequestType type;
		QList<QVariant> args;
		int sequencenumber;
	};

	FreeEmsComms(QObject *parent = 0);
	void setPort(QString portname);
	void setBaud(int baudrate);
	void setLogFileName(QString filename);
	void connectSerial(QString port,int baud);
	void loadLog(QString filename);
	void playLog();
	void populateDataFields();
	int updateBlockInRam(int location,int offset, int size,QByteArray data);
protected:
	void run();
private:
	int m_sequenceNumber;
	QMutex m_reqListMutex;
	QList<RequestClass> m_reqList;
	QList<RequestClass> m_threadReqList;
	SerialThread *serialThread;
	LogLoader *logLoader;
	bool m_waitingForResponse;
	int m_payloadWaitingForResponse;
	RequestClass m_currentWaitingRequest;
	//void parseBuffer(QByteArray buffer);
signals:
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void error(QString msg);
	void commandSuccessfull(int sequencenumber);
	void commandFailed(int sequencenumber,int errornum);
	//void updateBlockInRamFailed(int location,int offset,int size,QByteArray data);
	//void updateBlockInRamSucceeded();
public slots:
private slots:
	QPair<QByteArray,QByteArray> parseBuffer(QByteArray buffer);

};

#endif // FREEEMSCOMMS_H
