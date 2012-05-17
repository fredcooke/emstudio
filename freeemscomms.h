/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#ifndef FREEEMSCOMMS_H
#define FREEEMSCOMMS_H

#include <QThread>
#include <QFile>
#include <QMutex>
#include <QVariant>
#include <QDateTime>
#include "serialthread.h"
#include "logloader.h"
class FreeEmsComms : public QThread
{
	Q_OBJECT
public:
	enum RequestType
	{
		SERIAL_CONNECT=0xFFFF01,
		SERIAL_DISCONNECT=0xFFFF02,
		UPDATE_BLOCK_IN_RAM=0x0100,
		RETRIEVE_BLOCK_IN_RAM=0x0104,
		GET_INTERFACE_VERSION=0x0000,
		GET_FIRMWARE_VERSION=0x0002,
		GET_MAX_PACKET_SIZE=0x0004,
		GET_LOCATION_ID_LIST=0xDA5E,
		GET_LOCATION_ID_INFO=0xF8E0,
		ECHO_PACKET=0x0006,
		SOFT_RESET=0x0008,
		HARD_RESET=0x0010,
		GET_DECODER_NAME=0xEEEE,
		GET_FIRMWARE_BUILD_DATE=0xEEF0,
		GET_COMPILER_VERSION=0xEEF2,
		GET_OPERATING_SYSTEM=0xEEF4
	};
	enum LocationIdFlags
	{
		BLOCK_HAS_PARENT=0x0001,
		BLOCK_IS_RAM=0x0002,
		BLOCK_IS_FLASH=0x0004,
		BLOCK_IS_INDEXABLE=0x0008,
		BLOCK_IS_READ_ONLY=0x0010,
		BLOCK_GETS_VERIFIED=0x0020,
		BLOCK_FOR_BACKUP_RESTORE=0x0040,
		BLOCK_SPARE_7=0x0080,
		BLOCK_SPARE_8=0x0100,
		BLOCK_SPARE_9=0x0200,
		BLOCK_SPARE_10=0x0400,
		BLOCK_SPARE_11=0x0800,
		BLOCK_IS_2D_TABLE=0x1000,
		BLOCK_IS_MAIN_TABLE=0x2000,
		BLOCK_IS_LOOKUP_DATA=0x4000,
		BLOCK_IS_CONFIGURATION=0x8000
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
	int getLocationIdInfo(unsigned short locationid);
	int getInterfaceVersion();
	int getFirmwareVersion();
	int getMaxPacketSize();
	int getDecoderName();
	int getFirmwareBuildDate();
	int getCompilerVersion();
	int getOperatingSystem();
	int echoPacket(QByteArray packet);
	int getLocationIdList(unsigned char listtype, unsigned short listmask);
	int softReset();
	int hardReset();
	bool sendSimplePacket(unsigned short payload);
	void connectSerial(QString port,int baud);
	void loadLog(QString filename);
	void playLog();
	void populateDataFields();
	QByteArray generatePacket(QByteArray header,QByteArray payload);
	int updateBlockInRam(int location,int offset, int size,QByteArray data);
	int retrieveBlockFromRam(int location, int offset, int size);
	void setInterByteSendDelay(int milliseconds);
protected:
	void run();
private:
	qint64 m_timeoutMsecs;
	QList<LocationIdFlags> m_blockFlagList;
	int m_sequenceNumber;
	QMutex m_reqListMutex;
	QList<RequestClass> m_reqList;
	QList<RequestClass> m_threadReqList;
	SerialThread *serialThread;
	LogLoader *logLoader;
	bool m_waitingForResponse;
	unsigned int m_payloadWaitingForResponse;
	RequestClass m_currentWaitingRequest;
	//void parseBuffer(QByteArray buffer);
signals:
	void decoderName(QString name);
	void firmwareBuild(QString date);
	void compilerVersion(QString version);
	void operatingSystem(QString os);
	void connected();
	void locationIdList(QList<unsigned short> idlist);
	void locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size);
	void blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QString version);
	void firmwareVersion(QString version);
	void debugVerbose(QString msg);
	void unknownPacket(QByteArray header,QByteArray payload);
	void debug(QString msg);
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
