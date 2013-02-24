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
#include "headers.h"
#include "serialport.h"
#include "emscomms.h"
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
	unsigned int currentPacketCount;
	QByteArray readPacket(SerialPort *port);
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
