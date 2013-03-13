/**************************************************************************************
 * Copyright (c) 2013, Michael Carpenter  (malcom2073@gmail.com)                      *
 * All rights reserved.                                                               *
 *                                                                                    *
 * Redistribution and use in source and binary forms, with or without                 *
 * modification, are permitted provided that the following conditions are met:        *
 *     * Redistributions of source code must retain the above copyright               *
 *       notice, this list of conditions and the following disclaimer.                *
 *     * Redistributions in binary form must reproduce the above copyright            *
 *       notice, this list of conditions and the following disclaimer in the          *
 *       documentation and/or other materials provided with the distribution.         *
 *     * Neither the name EMStudio nor the                                            *
 *       names of its contributors may be used to endorse or promote products         *
 *       derived from this software without specific prior written permission.        *
 *                                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND    *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE             *
 * DISCLAIMED. IN NO EVENT SHALL MICHAEL CARPENTER BE LIABLE FOR ANY                  *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;       *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND        *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                       *
 **************************************************************************************/

#ifndef O5ECOMMS_H
#define O5ECOMMS_H

#include <QThread>
#include <QFile>
#include <QMutex>
#include <QVariant>
#include <QDateTime>
#include <QtPlugin>
#include "table3ddata.h"
#include "table2ddata.h"
#include "datapacketdecoder.h"
#include "o5edatapacketdecoder.h"
#include "o5ememorymetadata.h"
#include "headers.h"
#include "serialport.h"
#include "emscomms.h"
#include "o5eheaders.h"
#include <QTimer>


class O5EComms : public EmsComms
{
	Q_OBJECT
	Q_INTERFACES(EmsComms)
public:
	O5EComms(QObject *parent=0);
	void stop();
	void setLogsEnabled(bool enabled);
	DataPacketDecoder *getDecoder();
	MemoryMetaData *getMetaParser();
	Table3DData *getNew3DTableData();
	Table2DData *getNew2DTableData();
	void setLogDirectory(QString dir);
	void setPort(QString portname);
	void setBaud(int baudrate);
	void setLogFileName(QString filename);
	bool sendPacket(unsigned short payloadid,QList<QVariant> arglist=QList<QVariant>(),QList<int> argsizelist=QList<int>(),bool haslength=false);
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
	void disconnectSerial();
	void loadLog(QString filename);
	void playLog();
	QByteArray generatePacket(QByteArray header,QByteArray payload);
	int updateBlockInRam(unsigned short location,unsigned short offset, unsigned short size,QByteArray data);
	int updateBlockInFlash(unsigned short location,unsigned short offset, unsigned short size,QByteArray data);
	int retrieveBlockFromRam(unsigned short location, unsigned short offset, unsigned short size);
	int retrieveBlockFromFlash(unsigned short location, unsigned short offset, unsigned short size);
	int burnBlockFromRamToFlash(unsigned short location,unsigned short offset, unsigned short size);
	void setInterByteSendDelay(int milliseconds);
	void setlogsDebugEnabled(bool enabled);
private:
	enum RequestType
	{
		GET_INTERFACE_VERSION=0x0000,
		GET_FIRMWARE_VERSION=0x0002,
		GET_MAX_PACKET_SIZE=0x0004,
		ECHO_PACKET=0x0006,
		SOFT_RESET=0x0008,
		HARD_RESET=0x000A,
		UPDATE_BLOCK_IN_RAM=0x0100,
		UPDATE_BLOCK_IN_FLASH=0x0102,
		RETRIEVE_BLOCK_IN_RAM=0x0104,
		RETRIEVE_BLOCK_IN_FLASH=0x0106,
		BURN_BLOCK_FROM_RAM_TO_FLASH=0x0108,
		GET_LOCATION_ID_LIST=0xDA5E,
		GET_DECODER_NAME=0xEEEE,
		GET_FIRMWARE_BUILD_DATE=0xEEF0,
		GET_COMPILER_VERSION=0xEEF2,
		GET_OPERATING_SYSTEM=0xEEF4,
		GET_LOCATION_ID_INFO=0xF8E0,
		SERIAL_CONNECT=0xFFFF01,
		SERIAL_DISCONNECT=0xFFFF02
	};
	class RequestClass
	{
	public:
		RequestClass()
		{
			retryCount=0;
		}

		unsigned char retryCount;
		RequestType type;
		QList<QVariant> args;
		QList<int> argsize;
		int sequencenumber;
		void addArg(QVariant arg,int size=0) { args.append(arg); argsize.append(size);}
	};
	unsigned long currentPacketNum;
	unsigned int currentPacketCount;
	QByteArray readPacket(SerialPort *port);
	QMap<QString,QMap<QString,scalarclass> > pageMap;
	QMutex reqListMutex;
	QList<RequestClass> m_reqList;
	QList<RequestClass> m_privReqList;
	O5EDataPacketDecoder *m_dataDecoder;
	QString m_portName;
	SerialPort m_serialPort;
	O5EMemoryMetaData *m_memoryMetaData;
signals:
	void packetSent(unsigned short locationid,QByteArray header,QByteArray payload);
	void packetAcked(unsigned short locationid,QByteArray header,QByteArray payload);
	void packetNaked(unsigned short locationid,QByteArray header,QByteArray payload,unsigned short errornum);
	void decoderFailure(QByteArray packet);
	void decoderName(QString name);
	void firmwareBuild(QString date);
	void compilerVersion(QString version);
	void operatingSystem(QString os);
	void connected();
	void disconnected();
	void locationIdList(QList<unsigned short> idlist);
	void locationIdInfo(unsigned short locationid,MemoryLocationInfo info);
	void ramBlockRetrieved(unsigned short locationid, QByteArray header,QByteArray payload);
	void flashBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QString version);
	void firmwareVersion(QString version);
	void debugVerbose(QString msg);
	void unknownPacket(QByteArray header,QByteArray payload);
	void debug(QString msg);
	//void error(QString msg);
	//void error(QString msg);
	void error(SerialPortStatus error,QString msg = QString());
	void commandSuccessful(int sequencenumber);
	void commandFailed(int sequencenumber,unsigned short errornum);
	void commandTimedOut(int sequencenumber);
private slots:
	void packetCounter();
protected:
	void run();
};
#endif // O5ECOMMS_H
