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
		UPDATE_BLOCK_IN_RAM,
		RETRIEVE_BLOCK_IN_RAM,
		GET_INTERFACE_VERSION,
		GET_FIRMWARE_VERSION,
		GET_MAX_PACKET_SIZE,
		GET_LOCATION_ID_LIST,
		ECHO_PACKET,
		SOFT_RESET,
		HARD_RESET

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
	int getInterfaceVersion();
	int getFirmwareVersion();
	int getMaxPacketSize();
	int echoPacket(QByteArray packet);
	int getLocationIdList(unsigned char listtype, unsigned short listmask);
	int softReset();
	int hardReset();
	void connectSerial(QString port,int baud);
	void loadLog(QString filename);
	void playLog();
	void populateDataFields();
	QByteArray generatePacket(QByteArray header,QByteArray payload);
	int updateBlockInRam(int location,int offset, int size,QByteArray data);
	int retrieveBlockFromRam(int location, int offset, int size);
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
	void locationIdList(QList<unsigned short> idlist);
	void blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QByteArray version);
	void firmwareVersion(QString version);
	void error(QString msg);
	void commandSuccessful(int sequencenumber);
	void commandFailed(int sequencenumber,unsigned short errornum);
	//void updateBlockInRamFailed(int location,int offset,int size,QByteArray data);
	//void updateBlockInRamSucceeded();
public slots:
private slots:
	QPair<QByteArray,QByteArray> parseBuffer(QByteArray buffer);

};

#endif // FREEEMSCOMMS_H
