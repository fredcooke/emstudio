/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

#include "freeemscomms.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include "fetable2ddata.h"
#include "fetable3ddata.h"
#define NAK 0x02
FreeEmsComms::FreeEmsComms(QObject *parent) : EmsComms(parent)
{
	qRegisterMetaType<QList<unsigned short> >("QList<unsigned short>");
	qRegisterMetaType<QList<FreeEmsComms::LocationIdFlags> >("QList<FreeEmsComms::LocationIdFlags>");
	qRegisterMetaType<SerialPortStatus>("SerialPortStatus");
	serialPort = new SerialPort(this);
	connect(serialPort,SIGNAL(dataWritten(QByteArray)),this,SLOT(dataLogWrite(QByteArray)));
	connect(serialPort,SIGNAL(parseBuffer(QByteArray)),this,SLOT(parseBuffer(QByteArray)),Qt::DirectConnection);
	m_isConnected = false;

	dataPacketDecoder = new FEDataPacketDecoder();
	m_metaDataParser = new FEMemoryMetaData();

    m_lastdatalogTimer = new QTimer(this);
    connect(m_lastdatalogTimer,SIGNAL(timeout()),this,SLOT(datalogTimerTimeout()));
    m_lastdatalogTimer->start(500); //Every half second, check to see if we've timed out on datalogs.

	m_waitingForResponse = false;
	m_logsEnabled = false;
    m_lastDatalogUpdateEnabled = false;
	m_logInFile=0;
	m_logOutFile=0;
	m_logInOutFile=0;
	m_debugLogsEnabled = false;
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
	m_blockFlagList.append(BLOCK_IS_2D_SIGNED_TABLE);
	m_blockFlagList.append(BLOCK_IS_2D_TABLE);
	m_blockFlagList.append(BLOCK_IS_MAIN_TABLE);
	m_blockFlagList.append(BLOCK_IS_LOOKUP_DATA);
	m_blockFlagList.append(BLOCK_IS_CONFIGURATION);


	m_blockFlagToNameMap[BLOCK_HAS_PARENT] = "Parent";
	m_blockFlagToNameMap[BLOCK_IS_RAM] = "Is Ram";
	m_blockFlagToNameMap[BLOCK_IS_FLASH] = "Is Flash";
	m_blockFlagToNameMap[BLOCK_IS_INDEXABLE] = "Is Indexable";
	m_blockFlagToNameMap[BLOCK_IS_READ_ONLY] = "Is Read Only";
	m_blockFlagToNameMap[BLOCK_FOR_BACKUP_RESTORE] = "For Backup";
	m_blockFlagToNameMap[BLOCK_GETS_VERIFIED] = "Is Verified";
	m_blockFlagToNameMap[BLOCK_IS_2D_TABLE] = "2D Table";
	m_blockFlagToNameMap[BLOCK_IS_2D_SIGNED_TABLE] = "2D Signed Table";
	m_blockFlagToNameMap[BLOCK_IS_MAIN_TABLE] = "3D Table";
	m_blockFlagToNameMap[BLOCK_IS_LOOKUP_DATA] = "Lookup Table";
	m_blockFlagToNameMap[BLOCK_IS_CONFIGURATION] = "Configuration";

}
MemoryMetaData *FreeEmsComms::getMetaParser()
{
	return m_metaDataParser;
}

DataPacketDecoder *FreeEmsComms::getDecoder()
{
	return dataPacketDecoder;
}
Table3DData *FreeEmsComms::getNew3DTableData()
{
	return new FETable3DData();
}

Table2DData *FreeEmsComms::getNew2DTableData()
{
	return new FETable2DData();
}

FreeEmsComms::~FreeEmsComms()
{
}

void FreeEmsComms::disconnectSerial()
{
	RequestClass req;
	req.type = SERIAL_DISCONNECT;
	m_reqListMutex.lock();
	m_reqList.append(req);
	m_reqListMutex.unlock();
}
void FreeEmsComms::openLogs()
{
	qDebug() << "Open logs:" << m_logsDirectory + "/" + m_logsFilename + ".bin";
	if (!QDir(m_logsDirectory).exists())
	{
		QDir dir(QCoreApplication::instance()->applicationDirPath());
		if (!dir.mkpath(m_logsDirectory))
		{
			emit error("Unable to create log directory. Data will NOT be logged until this is fixed!");
		}
	}
	m_logInFile = new QFile(m_logsDirectory + "/" + m_logsFilename + ".bin");
	m_logInFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	if (m_debugLogsEnabled)
	{
		m_logInOutFile = new QFile(m_logsDirectory + "/" + m_logsFilename + ".both.bin");
		m_logInOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
		m_logOutFile = new QFile(m_logsDirectory + "/" + m_logsFilename + ".toecu.bin");
		m_logOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	}
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
	Q_UNUSED(filename);
}

void FreeEmsComms::playLog()
{
}
void FreeEmsComms::setLogsEnabled(bool enabled)
{
	if (m_logsEnabled && !enabled)
	{
		m_logInFile->close();
		delete m_logInFile;
		m_logInFile=0;

		if (m_debugLogsEnabled)
		{
			m_logInOutFile->close();
			delete m_logInOutFile;
			m_logInOutFile=0;

			m_logOutFile->close();
			delete m_logOutFile;
			m_logOutFile=0;
		}
	}
	else if (!m_logsEnabled && enabled)
	{
		if (m_isConnected)
		{
			//If we're connected, open logs. Otherwise, don't as they will be open next time we connect.
			openLogs();
		}
	}
	m_logsEnabled = enabled;
}
void FreeEmsComms::setlogsDebugEnabled(bool enabled)
{
	if (m_logsEnabled && enabled && !m_debugLogsEnabled)
	{
		if (!QDir(m_logsDirectory).exists())
		{
			QDir dir(QCoreApplication::instance()->applicationDirPath());
			if (!dir.mkpath(m_logsDirectory))
			{
				emit error("Unable to create log directory. Data will NOT be logged until this is fixed!");
			}
		}
		m_logInOutFile = new QFile(m_logsDirectory + "/" + m_logsFilename + ".both.bin");
		m_logInOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
		m_logOutFile = new QFile(m_logsDirectory + "/" + m_logsFilename + ".toecu.bin");
		m_logOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	}
	else if (m_logsEnabled && !enabled && m_debugLogsEnabled)
	{
		m_logInOutFile->close();
		m_logInOutFile->deleteLater();
		m_logInOutFile=0;
		m_logOutFile->close();
		m_logOutFile->deleteLater();
		m_logOutFile=0;
	}
	m_debugLogsEnabled = enabled;
}

void FreeEmsComms::setLogDirectory(QString dir)
{
	m_logsDirectory = dir;
}

void FreeEmsComms::setPort(QString portname)
{
	serialPort->setPort(portname);
}

void FreeEmsComms::setBaud(int baudrate)
{
	serialPort->setBaud(baudrate);
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
int FreeEmsComms::enableDatalogStream()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = UPDATE_BLOCK_IN_RAM;
	req.addArg(0x9000,2);
	req.addArg(0,2);
	req.addArg(1,2);
	req.addArg(QByteArray().append((char)0x01));
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}

int FreeEmsComms::disableDatalogStream()
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = UPDATE_BLOCK_IN_RAM;
	req.addArg(0x9000,2);
	req.addArg(0,2);
	req.addArg(1,2);
	req.addArg(QByteArray().append((char)0x00));
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
		qDebug() << "sendPacket failed";
		return false;
	}
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
	qDebug() << "About to send packet";
	if (serialPort->writePacket(generatePacket(header,payload)) < 0)
	{
		return false;
	}
	qDebug() << "Sent packet" << "0x" + QString::number(payloadid,16).toUpper();
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
	packet.append((char)0xCC);
	return packet;
}
void FreeEmsComms::setInterByteSendDelay(int milliseconds)
{
	serialPort->setInterByteSendDelay(milliseconds);
}

void FreeEmsComms::run()
{
	rxThread = new SerialRXThread(this);
	connect(rxThread,SIGNAL(incomingPacket(QByteArray)),this,SLOT(parseEverything(QByteArray)),Qt::DirectConnection);
	connect(rxThread,SIGNAL(dataRead(QByteArray)),this,SLOT(dataLogRead(QByteArray)));
	m_terminateLoop = false;
	bool serialconnected = false;

	while (!m_terminateLoop)
	{
		m_reqListMutex.lock();
		m_threadReqList.append(m_reqList);
		m_reqList.clear();
		m_reqListMutex.unlock();
		for (int i=0;i<m_threadReqList.size();i++)
		{
			if (m_threadReqList[i].type == SERIAL_CONNECT)
			{
				SerialPortStatus errortype = serialPort->verifyFreeEMS(m_threadReqList[i].args[0].toString());
				if (errortype != NONE)
				{
					qDebug() << "Unable to verify ECU";
					QString errorstr = "";
					if (errortype == UNABLE_TO_CONNECT)
					{
						errorstr = "Unable to open serial port " + m_threadReqList[i].args[0].toString() + " Please ensure no other application has the port open and that the port exists!";
					}
					else if (errortype == UNABLE_TO_LOCK)
					{
						errorstr = "Unable to open serial port " + m_threadReqList[i].args[0].toString() + " because another compatible application has locked it. Please close all serial port related applications and try again.";
					}
					else if (errortype == UNABLE_TO_WRITE)
					{
						errorstr = "Unable to open serial port " + m_threadReqList[i].args[0].toString() + " Please ensure no other application has the port open and that the port exists!";
					}
					else if (errortype == LOADER_MODE)
					{
						//TODO Fix this when we have the ability to reset SM mode
						errorstr = "Unable to connect to ECU. SerialMonitor mode detected! Please remove SM jumper, reset the ECU and click retry!";
					}
					emit error(errortype,errorstr);
					serialconnected = false;
					serialPort->closePort();
					emit disconnected();
					//On a disconnect, we are going to be deleting this thread, so go ahead and quit out;
					return;
					m_threadReqList.removeAt(i);
					i--;
					continue;
				}
				emit debugVerbose("SERIAL_CONNECT");
				int errornum = 0;
				if ((errornum = serialPort->openPort(m_threadReqList[i].args[0].toString(),m_threadReqList[i].args[1].toInt())))
				{
					if (errornum == -1)
					{
						emit error(UNABLE_TO_CONNECT,"Unable to open serial port " + m_threadReqList[i].args[0].toString() + " Please ensure no other application has the port open and that the port exists!");
						qDebug() << "Unable to connect to COM port";
					}
					else if (errornum == -2)
					{
						emit error(UNABLE_TO_LOCK,"Unable to open serial port " + m_threadReqList[i].args[0].toString() + " due to another freeems application locking the port. Please close all other freeems related applications and try again.");
						qDebug() << "Unable to connect to COM port due to process lock";
					}
					m_threadReqList.removeAt(i);
					i--;
					emit disconnected();
					continue;
				}
				qDebug() << "Serial connected!";
				//Before we finish emitting the fact that we are connected, let's verify this is a freeems system we are talking to.
				if (!sendPacket(GET_FIRMWARE_VERSION))
				{
					qDebug() << "Error writing packet. Quitting thread";
					return;
				}
				int dataattempts = 0;
				int nodataattempts=0;
				bool good = false;
				bool nodata = true;
				while (dataattempts < 10 && !good && nodataattempts < 4)
				{
					QByteArray result = rxThread->readSinglePacket(serialPort);
					if (result.size() > 0)
					{
						dataattempts++;
						nodata = false;
						Packet p = parseBuffer(result);
						if (p.isValid && p.payloadid == GET_FIRMWARE_VERSION+1)
						{
							//We're good!
							good = true;
						}
					}
					else
					{
						nodataattempts++;
					}
				}
				if (!good)
				{
					QString errorstr = "";
					SerialPortStatus errortype = NONE;
					if (nodata)
					{
						errorstr = "Unable to communicate with ECU, Serial port is unresponsive. Please verify your FreeEMS Board is plugged in, powered up, and all serial settings are properly set.";
						errortype = (SerialPortStatus)NO_DATA;
					}
					else
					{
						errorstr = "Unable to communicate with FreeEMS, corrupt data received. Please verify serial settings, in particular double check the baud rate.";
						errortype = (SerialPortStatus)INVALID_DATA;
					}
					emit error(errortype,errorstr);
					serialconnected = false;
					serialPort->closePort();
					emit disconnected();
					//On a disconnect, we are going to be deleting this thread, so go ahead and quit out;
					return;
					m_threadReqList.removeAt(i);
					i--;
					continue;
				}

				serialconnected = true;
				emit debug("Connected to serial port");
				m_isConnected = true;
				openLogs();
				emit connected();
				m_threadReqList.removeAt(i);
				i--;
				rxThread->start(serialPort);

			}
			else if (!serialconnected)
			{
				continue;
			}
			else if (m_threadReqList[i].type == SERIAL_DISCONNECT)
			{
				emit debugVerbose("SERIAL_DISCONNECT");
				rxThread->stop();
				rxThread->wait(500);
				rxThread->terminate();
				rxThread->deleteLater();

				serialPort->closePort();
				serialconnected = false;
				emit disconnected();
			}
			else if (m_threadReqList[i].type == GET_LOCATION_ID_LIST)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_DECODER_NAME)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_FIRMWARE_BUILD_DATE)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_COMPILER_VERSION)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_OPERATING_SYSTEM)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == BURN_BLOCK_FROM_RAM_TO_FLASH)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0x0108;
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_LOCATION_ID_INFO)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xF8E0;
					if (!sendPacket(m_threadReqList[i],false))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
				}
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == UPDATE_BLOCK_IN_RAM)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();

			}
			else if (m_threadReqList[i].type == RETRIEVE_BLOCK_IN_RAM)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == UPDATE_BLOCK_IN_FLASH)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == RETRIEVE_BLOCK_IN_FLASH)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_INTERFACE_VERSION)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_FIRMWARE_VERSION)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == GET_MAX_PACKET_SIZE)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == ECHO_PACKET)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == SOFT_RESET)
			{
				m_waitingInfoMutex.lock();
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
				m_waitingInfoMutex.unlock();
			}
			else if (m_threadReqList[i].type == HARD_RESET)
			{
				m_waitingInfoMutex.lock();
				if (!m_waitingForResponse)
				{
					m_currentWaitingRequest = m_threadReqList[i];
					if (!sendPacket(HARD_RESET))
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
					m_threadReqList.removeAt(i);
					i--;
					emit packetSent(HARD_RESET,QByteArray(),QByteArray());
				}
				m_waitingInfoMutex.unlock();
			}
			else if (false)
			{
				serialPort->writePacket(QByteArray());
				break;
			}
		}

		m_waitingInfoMutex.lock();
		if (QDateTime::currentDateTime().currentMSecsSinceEpoch() - m_timeoutMsecs > 500 && m_waitingForResponse)
		{
			//5 seconds
			qDebug() << "TIMEOUT waiting for response to payload:" << "0x" + QString::number(m_payloadWaitingForResponse,16).toUpper() << "Sequence:" << m_currentWaitingRequest.sequencenumber;
			if (m_currentWaitingRequest.retryCount >= 2)
			{
				qDebug() << "No retries left!";
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
		m_waitingInfoMutex.unlock();
	}
	rxThread->stop();
	rxThread->wait(500);
}
void FreeEmsComms::parsePacket(Packet parsedPacket)
{
	if (parsedPacket.isValid)
	{
		QMutexLocker locker(&m_waitingInfoMutex);
		unsigned short payloadid = parsedPacket.payloadid;
		if (m_isSilent)
		{
			emit emsSilenceBroken();
			m_isSilent = false;
		}
		if (payloadid == 0x0191)
		{	//Datalog packet

			if (parsedPacket.isNAK)
			{
				//NAK
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
					details += "0x";
					details += (((unsigned char)parsedPacket.payload[j] < 0xF) ? "0" : "");
					details += QString::number(parsedPacket.payload[j],16);
					details += ",";
				}
				details += "}";
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
					details += "0x";
					details += (((unsigned char)parsedPacket.payload[j] < 0xF) ? "0" : "");
					details += QString::number(parsedPacket.payload[j],16);
					details += ",";
				}
				details += "}";

				if (m_currentWaitingRequest.args.size() == 0)
				{
					qDebug() << "ERROR! Current waiting packet's arg size is zero1!!";
					qDebug() << "0x" + QString::number(m_currentWaitingRequest.type,16).toUpper();
					qDebug() << "0x" + QString::number(payloadid,16).toUpper();
				}
				unsigned short locationid = m_currentWaitingRequest.args[0].toInt();
				//TODO double check to make sure that there aren't an odd number of items here...
				QList<LocationIdFlags> flaglist;
				if (parsedPacket.payload.size() >= 2)
				{
					MemoryLocationInfo info;
					unsigned short test = parsedPacket.payload[0] << 8;
					unsigned short parent;
					unsigned char rampage;
					unsigned char flashpage;
					unsigned short ramaddress;
					unsigned short flashaddress;
					unsigned short size;
					test += parsedPacket.payload[1];

					for (int j=0;j<m_blockFlagList.size();j++)
					{
						if (test & m_blockFlagList[j])
						{
							flaglist.append(m_blockFlagList[j]);
							if (m_blockFlagToNameMap.contains(m_blockFlagList[j]))
							{
								info.propertymap.append(QPair<QString,QString>(m_blockFlagToNameMap[m_blockFlagList[j]],"true"));
							}
						}
						else
						{
							if (m_blockFlagToNameMap.contains(m_blockFlagList[j]))
							{
								info.propertymap.append(QPair<QString,QString>(m_blockFlagToNameMap[m_blockFlagList[j]],"false"));
							}
						}
					}
					parent = ((unsigned char)parsedPacket.payload[2]) << 8;
					parent += (unsigned char)parsedPacket.payload[3];
					rampage = (unsigned char)parsedPacket.payload[4];
					flashpage = (unsigned char)parsedPacket.payload[5];
					ramaddress = ((unsigned char)parsedPacket.payload[6]) << 8;
					ramaddress += (unsigned char)parsedPacket.payload[7];
					flashaddress = ((unsigned char)parsedPacket.payload[8]) << 8;
					flashaddress += (unsigned char)parsedPacket.payload[9];
					size = ((unsigned char)parsedPacket.payload[10]) << 8;
					size += (unsigned char)parsedPacket.payload[11];

					info.locationid = locationid;
					info.parent = parent;
					info.ramaddress = ramaddress;
					info.rampage = rampage;
					info.flashaddress = flashaddress;
					info.flashpage = flashpage;
					info.rawflags = test;
					info.size = size;
					if (flaglist.contains(FreeEmsComms::BLOCK_IS_RAM))
					{
						info.isRam = true;
					}
					else
					{
						info.isRam = false;
					}
					if (flaglist.contains(FreeEmsComms::BLOCK_IS_FLASH))
					{
						info.isFlash = true;
					}
					else
					{
						info.isFlash = false;
					}
					if (flaglist.contains(BLOCK_HAS_PARENT))
					{
						info.hasParent = true;
					}
					else
					{
						info.hasParent = false;
					}
					if (flaglist.contains(BLOCK_IS_READ_ONLY))
					{
						info.isReadOnly = true;
					}
					if (flaglist.contains(FreeEmsComms::BLOCK_IS_2D_TABLE))
					{
						info.type = DATA_TABLE_2D;
					}
					else if (flaglist.contains(FreeEmsComms::BLOCK_IS_2D_SIGNED_TABLE))
					{
						info.type = DATA_TABLE_2D_SIGNED;
					}
					else if (flaglist.contains(FreeEmsComms::BLOCK_IS_MAIN_TABLE))
					{
						info.type = DATA_TABLE_3D;
					}
					else if (flaglist.contains(FreeEmsComms::BLOCK_IS_CONFIGURATION))
					{
						info.type = DATA_CONFIG;
					}
					else if (flaglist.contains(FreeEmsComms::BLOCK_IS_LOOKUP_DATA))
					{
						info.type = DATA_TABLE_LOOKUP;
					}
					else
					{
						info.type = DATA_UNDEFINED;
					}
					emit locationIdInfo(locationid,info);

				}


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
		if (payloadid == 0x0191)
		{
			m_lastDatalogUpdateEnabled = true;
			m_lastDatalogTime = QDateTime::currentMSecsSinceEpoch();

			//Need to pull sequence number out of here
		}
		else
		{
			if (m_waitingForResponse)
			{
				if (payloadid == m_payloadWaitingForResponse+1)
				{
					qDebug() << "Recieved Response" << "0x" + QString::number(m_payloadWaitingForResponse+1,16).toUpper() << "For Payload:" << "0x" + QString::number(m_payloadWaitingForResponse+1,16).toUpper()<< "Sequence Number:" << m_currentWaitingRequest.sequencenumber;
					qDebug() << "Currently waiting for:" << QString::number(m_currentWaitingRequest.type,16).toUpper();
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
				else
				{
					qDebug() << "ERROR! Invalid packet:" << "0x" + QString::number(payloadid,16).toUpper();
				}
			}

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
	if (serialPort->writePacket(generatePacket(header,payload)) < 0)
	{
		qDebug() << "Error writing packet. Quitting thread";
		return false;
	}
	return true;
}

void FreeEmsComms::setLogFileName(QString filename)
{
	m_logsFilename = filename;
}
void FreeEmsComms::datalogTimerTimeout()
{
	if (!m_lastDatalogUpdateEnabled)
	{
		return;
	}
	quint64 current = QDateTime::currentMSecsSinceEpoch() - m_lastDatalogTime;
	if (current > 5000)
	{
		//It's been 5 seconds since our last datalog. We've likely either reset, or stopped responding.
		m_isSilent = true;
		m_lastDatalogUpdateEnabled = false;
		emit emsSilenceStarted();
	}
}

void FreeEmsComms::dataLogWrite(QByteArray buffer)
{
	if (m_logsEnabled)
	{
		if (m_debugLogsEnabled)
		{
			m_logOutFile->write(buffer);
			m_logInOutFile->write(buffer);
		}
	}
}


void FreeEmsComms::dataLogRead(QByteArray buffer)
{
	if (m_logsEnabled)
	{
		m_logInFile->write(buffer);
		if (m_debugLogsEnabled)
		{
			m_logInOutFile->write(buffer);
		}
	}
}

void FreeEmsComms::parseEverything(QByteArray buffer)
{
	Packet p = parseBuffer(buffer);
	parsePacket(p);
}

FreeEmsComms::Packet FreeEmsComms::parseBuffer(QByteArray buffer)
{
	if (buffer.size() <= 2)
	{

		qDebug() << "Not long enough to even contain a header!";
		emit decoderFailure(buffer);
		return Packet(false);
	}


	Packet retval;
	QByteArray header;
	//Parse the packet here
	int headersize = 3;
	int iloc = 0;
	bool seq = false;
	bool len = false;
	if (buffer[iloc] & 0x100)
	{
		//Has header
		seq = true;
		headersize += 1;
	}
	if (buffer[iloc] & 0x1)
	{
		//Has length
		len = true;
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
		unsigned int length = buffer[iloc] << 8;
		length += (unsigned char)buffer[iloc+1];
		retval.length = length;
		iloc += 2;
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
		payload.append(buffer.mid(iloc),(buffer.length()-iloc));
	}
	QString output;
	for (int i=0;i<payload.size();i++)
	{
		int num = (unsigned char)payload[i];
		output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
	}
	output.clear();
	for (int i=0;i<header.size();i++)
	{
		int num = (unsigned char)header[i];
		output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
	}
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

Q_EXPORT_PLUGIN2(FreeEmsPlugin, FreeEmsComms)
