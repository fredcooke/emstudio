/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "freeemscomms.h"
#include <QDebug>
#include "freeemspacket.h"
#define NAK 0x02
FreeEmsComms::FreeEmsComms(QObject *parent) : QThread(parent)
{
	qRegisterMetaType<QList<unsigned short> >("QList<unsigned short>");
	qRegisterMetaType<QList<FreeEmsComms::LocationIdFlags> >("QList<FreeEmsComms::LocationIdFlags>");
	serialThread = new SerialThread(this);
	connect(serialThread,SIGNAL(parseBuffer(QByteArray)),this,SLOT(parseBuffer(QByteArray)));
	logLoader = new LogLoader(this);
	connect(logLoader,SIGNAL(parseBuffer(QByteArray)),this,SLOT(parseBuffer(QByteArray)));
	m_waitingForResponse = false;
	m_sequenceNumber = 1;
	m_blockFlagList.append(BLOCK_HAS_PARENT);
	m_blockFlagList.append(BLOCK_IS_RAM);
	m_blockFlagList.append(BLOCK_IS_FLASH);
	m_blockFlagList.append(BLOCK_IS_INDEXABLE);
	m_blockFlagList.append(BLOCK_IS_READ_ONLY);
	m_blockFlagList.append(BLOCK_GETS_VERIFIED);
	m_blockFlagList.append(BLOCK_FOR_BACKUP_RESTORE);
	m_blockFlagList.append(BLOCK_SPARE_7);
	m_blockFlagList.append(BLOCK_SPARE_8);
	m_blockFlagList.append(BLOCK_SPARE_9);
	m_blockFlagList.append(BLOCK_SPARE_10);
	m_blockFlagList.append(BLOCK_SPARE_11);
	m_blockFlagList.append(BLOCK_IS_2D_TABLE);
	m_blockFlagList.append(BLOCK_IS_MAIN_TABLE);
	m_blockFlagList.append(BLOCK_IS_LOOKUP_DATA);
	m_blockFlagList.append(BLOCK_IS_CONFIGURATION);

}
void FreeEmsComms::disconnectSerial()
{
	RequestClass req;
	req.type = SERIAL_DISCONNECT;
	m_reqListMutex.lock();
	m_reqList.append(req);
	m_reqListMutex.unlock();
}

void FreeEmsComms::connectSerial(QString port,int baud)
{
	RequestClass req;
	req.type = SERIAL_CONNECT;
	req.addArg(port);
	req.addArg(baud,sizeof(baud));
	m_reqListMutex.lock();
	m_reqList.append(req);
	m_reqListMutex.unlock();
}

void FreeEmsComms::loadLog(QString filename)
{
	logLoader->loadFile(filename);
}

void FreeEmsComms::playLog()
{
	logLoader->start();
}
void FreeEmsComms::setLogsEnabled(bool enabled)
{
	serialThread->setLogsEnabled(enabled);
}

void FreeEmsComms::setLogDirectory(QString dir)
{
	serialThread->setLogDirectory(dir);
}

void FreeEmsComms::setPort(QString portname)
{
	serialThread->setPort(portname);
}

void FreeEmsComms::setBaud(int baudrate)
{
	serialThread->setBaud(baudrate);
}
int FreeEmsComms::burnBlockFromRamToFlash(unsigned short location,unsigned short offset, unsigned short size)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = BURN_BLOCK_FROM_RAM_TO_FLASH;
	req.addArg(location,sizeof(location));
	req.addArg(offset,sizeof(offset));
	req.addArg(size,sizeof(size));
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}

int FreeEmsComms::updateBlockInRam(unsigned short location,unsigned short offset, unsigned short size,QByteArray data)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = UPDATE_BLOCK_IN_RAM;
	req.addArg(location,sizeof(location));
	req.addArg(offset,sizeof(offset));
	req.addArg(size,sizeof(size));
	req.addArg(data);
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::updateBlockInFlash(unsigned short location,unsigned short offset, unsigned short size,QByteArray data)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = UPDATE_BLOCK_IN_FLASH;
	req.addArg(location,sizeof(location));
	req.addArg(offset,sizeof(offset));
	req.addArg(size,sizeof(size));
	req.addArg(data);
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}

int FreeEmsComms::getDecoderName()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_DECODER_NAME;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getFirmwareBuildDate()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_FIRMWARE_BUILD_DATE;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getCompilerVersion()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_COMPILER_VERSION;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getOperatingSystem()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_OPERATING_SYSTEM;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::retrieveBlockFromFlash(unsigned short location, unsigned short offset, unsigned short size)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = RETRIEVE_BLOCK_IN_FLASH;
	req.addArg(location,sizeof(location));
	req.addArg(offset,sizeof(offset));
	req.addArg(size,sizeof(size));
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::retrieveBlockFromRam(unsigned short location, unsigned short offset, unsigned short size)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = RETRIEVE_BLOCK_IN_RAM;
	req.addArg(location,sizeof(location));
	req.addArg(offset,sizeof(offset));
	req.addArg(size,sizeof(size));
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getInterfaceVersion()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_INTERFACE_VERSION;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getFirmwareVersion()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_FIRMWARE_VERSION;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getMaxPacketSize()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_MAX_PACKET_SIZE;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::echoPacket(QByteArray packet)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = ECHO_PACKET;
	req.sequencenumber = m_sequenceNumber;
	req.addArg(packet);
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::getLocationIdInfo(unsigned short locationid)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_LOCATION_ID_INFO;
	req.sequencenumber = m_sequenceNumber;
	req.addArg(locationid,sizeof(locationid));
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}

int FreeEmsComms::getLocationIdList(unsigned char listtype, unsigned short listmask)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = GET_LOCATION_ID_LIST;
	req.sequencenumber = m_sequenceNumber;
	req.addArg(listtype,sizeof(listtype));
	req.addArg(listmask,sizeof(listmask));
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}

int FreeEmsComms::softReset()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = SOFT_RESET;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::hardReset()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = HARD_RESET;
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
bool FreeEmsComms::sendPacket(RequestClass request,bool haslength)
{
	if (!sendPacket(request.type,request.args,request.argsize,haslength))
	{
		return false;
	}
	qDebug() << "Sent packet" << "0x" + QString::number(request.type,16).toUpper() << "Sequence Number:" << request.sequencenumber;
	return true;
}

bool FreeEmsComms::sendPacket(unsigned short payloadid,QList<QVariant> arglist,QList<int> argsizelist,bool haslength)
{
	if (arglist.size() != argsizelist.size())
	{
		return false;
	}
	QByteArray header;
	QByteArray payload;
	for (int i=0;i<arglist.size();i++)
	{
		if (arglist[i].type() == QVariant::Int)
		{
			if (argsizelist[i] == 1)
			{
				unsigned char arg = arglist[i].toInt();
				payload.append((unsigned char)((arg) & 0xFF));
			}
			else if (argsizelist[i] == 2)
			{
				unsigned short arg = arglist[i].toInt();
				payload.append((unsigned char)((arg >> 8) & 0xFF));
				payload.append((unsigned char)((arg) & 0xFF));
			}
			else if (argsizelist[i] == 4)
			{
				unsigned int arg = arglist[i].toInt();
				payload.append((unsigned char)((arg >> 24) & 0xFF));
				payload.append((unsigned char)((arg >> 16) & 0xFF));
				payload.append((unsigned char)((arg >> 8) & 0xFF));
				payload.append((unsigned char)((arg) & 0xFF));
			}
		}
		else if (arglist[i].type() == QVariant::ByteArray)
		{
			//Data packet
			QByteArray arg = arglist[i].toByteArray();
			payload.append(arg);
		}
		else if (arglist[i].type() == QVariant::String)
		{
			QByteArray arg = arglist[i].toString().toAscii();
			payload.append(arg);
		}
	}
	if (haslength)
	{
		header.append((unsigned char)0x01); //Length, no seq no nak
		header.append((unsigned char)((payloadid >> 8) & 0xFF));
		header.append((unsigned char)((payloadid) & 0xFF));
		header.append((char)(payload.length() >> 8) & 0xFF);
		header.append((char)(payload.length()) & 0xFF);
	}
	else
	{
		header.append((char)0x00); //No Length, no seq no nak
		header.append((char)((payloadid >> 8) & 0xFF));
		header.append((char)((payloadid) & 0xFF));
	}
	if (serialThread->writePacket(generatePacket(header,payload)) < 0)
	{
		return false;
	}
	emit packetSent(payloadid,header,payload);
	return true;
}

QByteArray FreeEmsComms::generatePacket(QByteArray header,QByteArray payload)
{
	QByteArray packet;
	packet.append((char)0xAA);
	unsigned char checksum = 0;
	for (int i=0;i<header.size();i++)
	{
		checksum += header[i];
	}
	for (int i=0;i<payload.size();i++)
	{
		checksum += payload[i];
	}
	payload.append(checksum);
	for (int j=0;j<header.size();j++)
	{
		if (header[j] == (char)0xAA)
		{
			packet.append((char)0xBB);
			packet.append((char)0x55);
		}
		else if (header[j] == (char)0xBB)
		{
			packet.append((char)0xBB);
			packet.append((char)0x44);
		}
		else if (header[j] == (char)0xCC)
		{
			packet.append((char)0xBB);
			packet.append((char)0x33);
		}
		else
		{
			packet.append(header[j]);
		}
	}
	for (int j=0;j<payload.size();j++)
	{
		if (payload[j] == (char)0xAA)
		{
			packet.append((char)0xBB);
			packet.append((char)0x55);
		}
		else if (payload[j] == (char)0xBB)
		{
			packet.append((char)0xBB);
			packet.append((char)0x44);
		}
		else if (payload[j] == (char)0xCC)
		{
			packet.append((char)0xBB);
			packet.append((char)0x33);
		}
		else
		{
			packet.append(payload[j]);
		}
	}
	//packet.append(header);
	//packet.append(payload);
	packet.append((char)0xCC);
	return packet;
}
void FreeEmsComms::setInterByteSendDelay(int milliseconds)
{
	serialThread->setInterByteSendDelay(milliseconds);
}

void FreeEmsComms::run()
{
	bool serialconnected = false;
	//bool waitingforresponse=false;
	//int waitingpayloadid=0;

	while (true)
	{
		m_reqListMutex.lock();
		m_threadReqList.append(m_reqList);
		m_reqList.clear();
		m_reqListMutex.unlock();
		for (int i=0;i<m_threadReqList.size();i++)
		{
			if (m_threadReqList[i].type == SERIAL_CONNECT)
			{
				//qDebug() << "SERIAL_CONNECT";
				emit debugVerbose("SERIAL_CONNECT");
				int errornum = 0;
				if ((errornum = serialThread->openPort(m_threadReqList[i].args[0].toString(),m_threadReqList[i].args[1].toInt())))
				{
					if (errornum == -1)
					{
						qDebug() << "Unable to connect to COM port";
						emit error("Unable to connect to com port " + m_threadReqList[i].args[0].toString() + " at baud " + QString::number(m_threadReqList[i].args[1].toInt()));
					}
					else if (errornum == -2)
					{
						qDebug() << "Unable to connect to COM port due to process lock";
						emit error("Unable to connect to com port " + m_threadReqList[i].args[0].toString() + " at baud " + QString::number(m_threadReqList[i].args[1].toInt()) + " due to another process holding lock on the port");
					}
					//return;
					m_threadReqList.removeAt(i);
					i--;
					emit disconnected();
					continue;
				}
				//qDebug() << "Serial connected!";
				serialconnected = true;
				emit debug("Connected to serial port");
				emit connected();
				m_threadReqList.removeAt(i);
				i--;
			}
			else if (!serialconnected)
			{
				continue;
			}
			else if (m_threadReqList[i].type == SERIAL_DISCONNECT)
			{
				emit debugVerbose("SERIAL_DISCONNECT");
				serialThread->closePort();
				serialconnected = false;
				emit disconnected();
			}
			else if (m_threadReqList[i].type == GET_LOCATION_ID_LIST)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					qDebug() << "GET_LOCATION_ID_LIST";
					emit debugVerbose("GET_LOCATION_ID_LIST");
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xDA5E;
					if (!sendPacket(m_threadReqList[i],true))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_DECODER_NAME)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xEEEE;
					if (!sendPacket(GET_DECODER_NAME))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_FIRMWARE_BUILD_DATE)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xEEF0;
					if (!sendPacket(GET_FIRMWARE_BUILD_DATE))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_COMPILER_VERSION)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xEEF2;
					if (!sendPacket(GET_COMPILER_VERSION))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_OPERATING_SYSTEM)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xEEF4;
					if (!sendPacket(GET_OPERATING_SYSTEM))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == BURN_BLOCK_FROM_RAM_TO_FLASH)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0108;
					//qDebug() << "Requesting location ID Info for:" << QString::number(locationid,16);
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_LOCATION_ID_INFO)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xF8E0;
					//qDebug() << "Requesting location ID Info for:" << QString::number(locationid,16);
					//if (!sendPacket(GET_LOCATION_ID_INFO,m_threadReqList[i].args,m_threadReqList[i].argsize,false))
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == UPDATE_BLOCK_IN_RAM)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0100;
					if (!sendPacket(m_threadReqList[i],true))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}

			}
			else if (m_threadReqList[i].type == RETRIEVE_BLOCK_IN_RAM)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0104;
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == UPDATE_BLOCK_IN_FLASH)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0102;
					if (!sendPacket(m_threadReqList[i],true))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == RETRIEVE_BLOCK_IN_FLASH)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0106;
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_INTERFACE_VERSION)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					qDebug() << "GET_INTERFACE_VERSION";
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0000;
					if (!sendPacket(GET_INTERFACE_VERSION))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_FIRMWARE_VERSION)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					qDebug() << "GET_FIRMWARE_VERSION";
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0002;
					if (!sendPacket(GET_FIRMWARE_VERSION))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == GET_MAX_PACKET_SIZE)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0004;
					if (!sendPacket(GET_MAX_PACKET_SIZE))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == ECHO_PACKET)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0006;
					if (!sendPacket(m_threadReqList[i],true))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (m_threadReqList[i].type == SOFT_RESET)
			{
				if (!m_waitingForResponse)
				{
					m_currentWaitingRequest = m_threadReqList[i];
					if (!sendPacket(SOFT_RESET))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
					emit packetSent(SOFT_RESET,QByteArray(),QByteArray());
				}
			}
			else if (m_threadReqList[i].type == HARD_RESET)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0010;
					if (!sendPacket(HARD_RESET))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
			}
			else if (false)
			{
				serialThread->writePacket(QByteArray());
				//waitingforresponse = true;
				//waitingpayloadid = 0x0101;
				break;
			}
		}

		//General packet reading
		if (serialconnected)
		{
			//qDebug() << "Attempting to read:";
			if (serialThread->readSerial(20) > 20)
			{
				//1000 errors with no good packets.
				qDebug() << "Packet read error!";
				emit debugVerbose("SERIAL_DISCONNECT");
				serialThread->closePort();
				serialconnected = false;
				emit disconnected();
			}
			//qDebug() << "finished attempting to read";
		}
		else
		{
			msleep(20);
		}
		if (QDateTime::currentDateTime().currentMSecsSinceEpoch() - m_timeoutMsecs > 500 && m_waitingForResponse)
		{
			//5 seconds
			qDebug() << "TIMEOUT waiting for response to payload:" << "0x" + QString::number(m_payloadWaitingForResponse,16).toUpper() << "Sequence:" << m_currentWaitingRequest.sequencenumber;
			if (m_currentWaitingRequest.retryCount >= 2)
			{
				qDebug() << "No retries left!";
				//emit commandFailed(m_currentWaitingRequest.sequencenumber,0);
				emit commandTimedOut(m_currentWaitingRequest.sequencenumber);
				m_waitingForResponse = false;
			}
			else
			{
				qDebug() << "Retrying";
				m_waitingForResponse = false;
				m_currentWaitingRequest.retryCount++;
				m_threadReqList.insert(0,m_currentWaitingRequest);
			}
			//TODO: Requeue the command for retry.
		}
		while (serialThread->bufferSize() != 0)
		{
			QByteArray packet = serialThread->readPacket();
			Packet parsedPacket = parseBuffer(packet);
			parsePacket(parsedPacket);
		}
	}
}
void FreeEmsComms::parsePacket(Packet parsedPacket)
{
	if (parsedPacket.isValid)
	{
		/*unsigned short payloadid = (unsigned short)packetpair.first[1] << 8;
		payloadid += (unsigned char)packetpair.first[2];*/
		unsigned short payloadid = parsedPacket.payloadid;
		//qDebug() << "Incoming packet. Payload:" << payloadid;

		if (payloadid != 0x0191)
		{
			//qDebug() << "Incoming packet:" << "0x" + QString::number(payloadid,16).toUpper();

		}
		if (m_waitingForResponse)
		{

			if (payloadid == m_payloadWaitingForResponse+1)
			{
				//qDebug() << "Recieved Response" << "0x" + QString::number(m_payloadWaitingForResponse+1,16).toUpper() << "For Payload:" << "0x" + QString::number(m_payloadWaitingForResponse+1,16).toUpper()<< "Sequence Number:" << m_currentWaitingRequest.sequencenumber;
				if (parsedPacket.isNAK)
				{
					//NAK to our packet
					unsigned short errornum = parsedPacket.payload[0] << 8;
					errornum += parsedPacket.payload[1];
					emit commandFailed(m_currentWaitingRequest.sequencenumber,errornum);
					emit packetNaked(m_currentWaitingRequest.type,parsedPacket.header,parsedPacket.payload,errornum);
				}
				else
				{
					//Packet is good.
					emit commandSuccessful(m_currentWaitingRequest.sequencenumber);
					emit packetAcked(m_currentWaitingRequest.type,parsedPacket.header,parsedPacket.payload);
				}
				m_waitingForResponse = false;
			}
		}

		if (payloadid == 0x0191)
		{	//Datalog packet

			if (parsedPacket.isNAK)
			{
				//NAK

				//emit commandFailed(int sequencenumber,int errornum);
			}
			else
			{
				emit dataLogPayloadReceived(parsedPacket.header,parsedPacket.payload);
			}
		}
		else if (payloadid == 0xEEEF)
		{
			//Decoder
			if (!(parsedPacket.isNAK))
			{
				emit decoderName(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0xEEF1)
		{
			//Firmware build date
			if (!(parsedPacket.isNAK))
			{
				emit firmwareBuild(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0xEEF3)
		{
			//Compiler Version
			if (!(parsedPacket.isNAK))
			{
				emit compilerVersion(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0xEEF5)
		{
			//Operating System
			if (!(parsedPacket.isNAK))
			{
				emit operatingSystem(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0xDA5F)
		{
			//Location ID List
			if (parsedPacket.isNAK)
			{
			}
			else
			{
				//TODO double check to make sure that there aren't an odd number of items here...
				qDebug() << "Location ID List";
				QString details = "Details: {";
				for (int j=0;j<parsedPacket.payload.size();j++)
				{
					//details += ((packetpair.second[j] == 0) ? "0x0" : "0x");
					details += "0x";
					details += (((unsigned char)parsedPacket.payload[j] < 0xF) ? "0" : "");
					details += QString::number(parsedPacket.payload[j],16);
					details += ",";
				}
				details += "}";
				//qDebug() << details;
				QList<unsigned short> idlist;
				for (int j=0;j<parsedPacket.payload.size();j+=2)
				{
					unsigned short tmp = 0;
					tmp += parsedPacket.payload[j] << 8;
					tmp += parsedPacket.payload[j+1];
					idlist.append(tmp);
				}
				emit locationIdList(idlist);
			}
		}
		else if (payloadid == 0xF8E1) //Location ID Info
		{
			if (parsedPacket.isNAK)
			{
			}
			else
			{
				QString details = "Details: {";
				for (int j=0;j<parsedPacket.payload.size();j++)
				{
					//details += ((packetpair.second[j] == 0) ? "0x0" : "0x");
					details += "0x";
					details += (((unsigned char)parsedPacket.payload[j] < 0xF) ? "0" : "");
					details += QString::number(parsedPacket.payload[j],16);
					details += ",";
				}
				details += "}";

				unsigned short locationid = m_currentWaitingRequest.args[0].toInt();
				//qDebug() << "Payload:" << QString::number(locationid,16);
				//qDebug() << details;
				//TODO double check to make sure that there aren't an odd number of items here...
				//QList<unsigned short> idlist;
				QList<LocationIdFlags> flaglist;
				if (parsedPacket.payload.size() >= 2)
				{
					unsigned short test = parsedPacket.payload[0] << 8;
					unsigned short parent;
					unsigned char rampage;
					unsigned char flashpage;
					unsigned short ramaddress;
					unsigned short flashaddress;
					unsigned short size;
					test += parsedPacket.payload[1];
					//qDebug() << "Location ID Info for location:" << QString::number(locationid,16) << "Flags:" << QString::number(test,16);
					for (int j=0;j<m_blockFlagList.size();j++)
					{
						if (test & m_blockFlagList[j])
						{
							flaglist.append(m_blockFlagList[j]);
						}
					}
					parent = parsedPacket.payload[2] << 8;
					parent += parsedPacket.payload[3];
					rampage = parsedPacket.payload[4];
					flashpage = parsedPacket.payload[5];
					ramaddress = ((unsigned char)parsedPacket.payload[6]) << 8;
					ramaddress += (unsigned char)parsedPacket.payload[7];
					flashaddress = ((unsigned char)parsedPacket.payload[8]) << 8;
					flashaddress += (unsigned char)parsedPacket.payload[9];
					size = parsedPacket.payload[10] << 8;
					size += parsedPacket.payload[11];
					emit locationIdInfo(locationid,test,flaglist,parent,rampage,flashpage,ramaddress,flashaddress,size);
				}


				//emit locationIdList(idlist);
			}
		}
		else if (payloadid == 0x0001) //Interface version response
		{
			//Handle interface version
			if (parsedPacket.isNAK)
			{
				//NAK
				qDebug() << "IFACE VERSION NAK";
			}
			else
			{
				emit interfaceVersion(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0x0003) //Firmware version response
		{
			if (parsedPacket.isNAK)
			{
				//NAK
				qDebug() << "FIRMWARE VERSION NAK";
			}
			else
			{
				emit firmwareVersion(QString(parsedPacket.payload));
			}
		}
		else if (payloadid == 0x0107)
		{
			if (parsedPacket.isNAK)
			{
			}
			else
			{
				unsigned short locid = m_currentWaitingRequest.args[0].toUInt();
				emit flashBlockRetrieved(locid,parsedPacket.header,parsedPacket.payload);
			}
		}
		else if (payloadid == 0x0105)
		{
			if (parsedPacket.isNAK)
			{
			}
			else
			{
				//Block from ram is here.
				unsigned short locid = m_currentWaitingRequest.args[0].toUInt();
				emit ramBlockRetrieved(locid,parsedPacket.header,parsedPacket.payload);
			}
		}
		else
		{
			emit unknownPacket(parsedPacket.header,parsedPacket.payload);
		}
	}
	else
	{
		qDebug() << "Header size is only" << parsedPacket.header.length() << "! THIS SHOULD NOT HAPPEN!";
		QString headerstring = "";
		QString packetstring = "";
		for (int i=0;i<parsedPacket.header.size();i++)
		{
			headerstring += QString::number((unsigned char)parsedPacket.header[i],16);
		}
		for (int i=0;i<parsedPacket.payload.size();i++)
		{
			packetstring += QString::number((unsigned char)parsedPacket.payload[i],16);
		}
		qDebug() << "Header:" << headerstring;
		qDebug() << "Packet:" << packetstring;
	}
}

bool FreeEmsComms::sendSimplePacket(unsigned short payloadid)
{
	m_waitingForResponse = true;
	m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
	QByteArray header;
	QByteArray payload;
	header.append((char)0x00);
	header.append((char)((payloadid << 8) & 0xFF));
	header.append((char)(payloadid & 0xFF));
	m_payloadWaitingForResponse = payloadid;
	if (serialThread->writePacket(generatePacket(header,payload)) < 0)
	{
		qDebug() << "Error writing packet. Quitting thread";
		return false;
	}
	return true;
}

void FreeEmsComms::setLogFileName(QString filename)
{
	serialThread->setLogFileName(filename);
}

FreeEmsComms::Packet FreeEmsComms::parseBuffer(QByteArray buffer)
{
	if (buffer.size() <= 2)
	{

		qDebug() << "Not long enough to even contain a header!";
		emit decoderFailure(buffer);
		return Packet(false);
	}


	//qDebug() << "Packet:" << QString::number(buffer[1],16) << QString::number(buffer[buffer.length()-2],16);
	Packet retval;
	QByteArray header;
	//currPacket.clear();
	//Parse the packet here
	int headersize = 3;
	int iloc = 0;
	bool seq = false;
	bool len = false;
	if (buffer[iloc] & 0x100)
	{
		//Has header
		seq = true;
		//qDebug() << "Has seq";
		headersize += 1;
	}
	if (buffer[iloc] & 0x1)
	{
		//Has length
		len = true;
		//qDebug() << "Has length";
		headersize += 2;
	}
	header = buffer.mid(0,headersize);
	iloc++;
	unsigned int payloadid = (unsigned int)buffer[iloc] << 8;

	payloadid += (unsigned char)buffer[iloc+1];
	retval.payloadid = payloadid;
	iloc += 2;
	if (seq)
	{
		//qDebug() << "Sequence number" << QString::number(currPacket[iloc]);
		iloc += 1;
		retval.hasseq = true;
	}
	else
	{
		retval.hasseq = false;
	}
	QByteArray payload;
	if (len)
	{
		retval.haslength = true;
		//qDebug() << "Length found, buffer size:" << buffer.length() << "iloc:" << QString::number(iloc);
		unsigned int length = buffer[iloc] << 8;
		length += (unsigned char)buffer[iloc+1];
		retval.length = length;
		//qDebug() << "Length:" << length;
		iloc += 2;
		//curr += length;
		if ((unsigned int)buffer.length() > (unsigned int)(length + iloc))
		{
			qDebug() << "Packet length should be:" << length + iloc << "But it is" << buffer.length();
			emit decoderFailure(buffer);
			return Packet(false);
		}
		payload.append(buffer.mid(iloc,length));
	}
	else
	{
		retval.haslength = false;
		//qDebug() << "Buffer length:" << buffer.length();
		//qDebug() << "Attempted cut:" << buffer.length() - iloc;
		payload.append(buffer.mid(iloc),(buffer.length()-iloc));
	}
	//qDebug() << "Payload";
	QString output;
	for (int i=0;i<payload.size();i++)
	{
		int num = (unsigned char)payload[i];
		output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
	}
	//qDebug() << output;
	output.clear();
	//qDebug() << "Header";
	for (int i=0;i<header.size();i++)
	{
		int num = (unsigned char)header[i];
		output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
	}
	//qDebug() << output;
	//Last byte of currPacket should be out checksum.
	retval.header = header;
	retval.payload = payload;
	if (header[0] & NAK)
	{
		retval.isNAK = true;
	}
	else
	{
		retval.isNAK = false;
	}
	if (retval.header.size() >= 3)
	{
		return retval;
	}
	else
	{
		return Packet(false);
	}
}


