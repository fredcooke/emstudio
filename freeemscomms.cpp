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

#include "freeemscomms.h"
#include <QDebug>
FreeEmsComms::FreeEmsComms(QObject *parent) : QThread(parent)
{

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
void FreeEmsComms::connectSerial(QString port,int baud)
{
	RequestClass req;
	req.type = SERIAL_CONNECT;
	req.args.append(QVariant(port));
	req.args.append(baud);
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

void FreeEmsComms::setPort(QString portname)
{
	serialThread->setPort(portname);
}

void FreeEmsComms::setBaud(int baudrate)
{
	serialThread->setBaud(baudrate);
}
int FreeEmsComms::updateBlockInRam(int location,int offset, int size,QByteArray data)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = UPDATE_BLOCK_IN_RAM;
	req.args.append(location);
	req.args.append(offset);
	req.args.append(size);
	req.args.append(data);
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
int FreeEmsComms::retrieveBlockFromRam(int location, int offset, int size)
{
	m_reqListMutex.lock();
	RequestClass req;
	req.type = RETRIEVE_BLOCK_IN_RAM;
	req.args.append(location);
	req.args.append(offset);
	req.args.append(size);
	req.sequencenumber = m_sequenceNumber;
	m_sequenceNumber++;
	m_reqList.append(req);
	m_reqListMutex.unlock();
	return m_sequenceNumber-1;
}
/*GET_INTERFACE_VERSION,
GET_FIRMWARE_VERSION,
GET_MAX_PACKET_SIZE,
ECHO_PACKET,
SOFT_RESET,
HARD_RESET*/
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
	req.args.append(packet);
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
	req.args.append(locationid);
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
	req.args.append(listtype);
	req.args.append(listmask);
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

QByteArray FreeEmsComms::generatePacket(QByteArray header,QByteArray payload)
{
	QByteArray packet;
	packet.append(0xAA);
	unsigned char checksum = 0;
	for (int j=0;j<header.size();j++)
	{
		if (header[j] == (char)0xAA)
		{
			packet.append(0xBB);
			packet.append(0x55);
		}
		else if (header[j] == (char)0xBB)
		{
			packet.append(0xBB);
			packet.append(0x44);
		}
		else if (header[j] == (char)0xCC)
		{
			packet.append(0xBB);
			packet.append(0x33);
		}
		else
		{
			packet.append(header[j]);
		}
		checksum += header[j];
	}
	for (int j=0;j<payload.size();j++)
	{
		if (payload[j] == (char)0xAA)
		{
			packet.append(0xBB);
			packet.append(0x55);
		}
		else if (payload[j] == (char)0xBB)
		{
			packet.append(0xBB);
			packet.append(0x44);
		}
		else if (payload[j] == (char)0xCC)
		{
			packet.append(0xBB);
			packet.append(0x33);
		}
		else
		{
			packet.append(payload[j]);
		}
		checksum += packet[j];
	}
	//packet.append(header);
	//packet.append(payload);
	packet.append(checksum);
	packet.append(0xCC);
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
				emit debugVerbose("SERIAL_CONNECT");

				if (serialThread->openPort(m_threadReqList[i].args[0].toString(),m_threadReqList[i].args[1].toInt()))
				{
					qDebug() << "Unable to connect to COM port";
					emit error("Unable to connect to com port " + m_threadReqList[i].args[0].toString() + " at baud " + QString::number(m_threadReqList[i].args[1].toInt()));
					//return;
					m_threadReqList.removeAt(i);
					i--;					continue;
				}
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
			else if (m_threadReqList[i].type == GET_LOCATION_ID_LIST)
			{
				//m_threadReqList[i].args[0] - unsigned char listtype
				//m_threadReqList[i].args[1] - unsigned short listmask
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					qDebug() << "GET_LOCATION_ID_LIST";
					emit debugVerbose("GET_LOCATION_ID_LIST");
					m_currentWaitingRequest = m_threadReqList[i];
					m_payloadWaitingForResponse = 0xDA5E;
					unsigned char listtype = m_threadReqList[i].args[0].toInt();
					unsigned short listmask = m_threadReqList[i].args[1].toInt();
					QByteArray header;
					QByteArray payload;
					header.append((char)0x01); //Length, no seq no nak
					header.append((char)0xDA); // Payload 0xDA5E, get list of location IDs
					header.append((char)0x5E);
					payload.append((char)((listtype) & 0xFF));
					payload.append((char)((listmask << 8) & 0xFF));
					payload.append((char)((listmask) & 0xFF));
					header.append((char)(payload.length() << 8) & 0xFF);
					header.append((char)(payload.length()) & 0xFF);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
					unsigned short locationid = m_threadReqList[i].args[0].toInt();
					QByteArray header;
					QByteArray payload;
					header.append((char)0x01); //Length, no seq no nak
					header.append((char)0xF8); // Payload 0xF8E0, get location ID Info
					header.append((char)0xE0);
					payload.append((char)((locationid << 8) & 0xFF));
					payload.append((char)((locationid) & 0xFF));
					header.append((char)(payload.length() << 8) & 0xFF);
					header.append((char)(payload.length()) & 0xFF);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
					int location = m_threadReqList[i].args[0].toInt();
					int offset= m_threadReqList[i].args[1].toInt();
					int size = m_threadReqList[i].args[2].toInt();
					QByteArray data = m_threadReqList[i].args[3].toByteArray();
					QByteArray header;
					QByteArray payload;
					header.append((char)0x01); //Length, no seq no nak
					header.append((char)0x01); // Payload 0x0100, update block in ram
					header.append((char)0x00);
					payload.append((char)((location << 8) & 0xFF));
					payload.append((char)((location) & 0xFF));
					payload.append((char)((offset << 8) & 0xFF));
					payload.append((char)((offset) & 0xFF));
					payload.append((char)((size << 8) & 0xFF));
					payload.append((char)((size) & 0xFF));
					payload.append(data);
					header.append((char)(payload.length() << 8) & 0xFF);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
					int location = m_threadReqList[i].args[0].toInt();
					int offset= m_threadReqList[i].args[1].toInt();
					int size = m_threadReqList[i].args[2].toInt();
					QByteArray header;
					QByteArray payload;
					header.append((char)0x00); //No Length, no seq no nak
					header.append((char)0x01); // Payload 0x0104, retrieve block
					header.append((char)0x04);
					payload.append((char)((location << 8) & 0xFF));
					payload.append((char)((location) & 0xFF));
					payload.append((char)((offset << 8) & 0xFF));
					payload.append((char)((offset) & 0xFF));
					payload.append((char)((size << 8) & 0xFF));
					payload.append((char)((size) & 0xFF));
					//header.append((char)(packet.length() << 8) & 0xFF);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0000;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x00);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0002;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x02);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
				}
			}
			else if (m_threadReqList[i].type == GET_MAX_PACKET_SIZE)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0004;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x04);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
				}
			}
			else if (m_threadReqList[i].type == ECHO_PACKET)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0006;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x06);
					payload.append(m_threadReqList[i].args[0].toByteArray());
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
				}
			}
			else if (m_threadReqList[i].type == SOFT_RESET)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0008;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x08);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
				}
			}
			else if (m_threadReqList[i].type == HARD_RESET)
			{
				if (!m_waitingForResponse)
				{
					m_waitingForResponse = true;
					m_timeoutMsecs = QDateTime::currentDateTime().currentMSecsSinceEpoch();
					m_currentWaitingRequest = m_threadReqList[i];
					QByteArray header;
					QByteArray payload;
					m_payloadWaitingForResponse = 0x0010;
					header.append((char)0x00);
					header.append((char)0x00);
					header.append((char)0x10);
					m_threadReqList.removeAt(i);
					i--;
					if (serialThread->writePacket(generatePacket(header,payload)) < 0)
					{
						qDebug() << "Error writing packet. Quitting thread";
						return;
					}
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
			serialThread->readSerial(20);
		}
		else
		{
			msleep(20);
		}
		if (QDateTime::currentDateTime().currentMSecsSinceEpoch() - m_timeoutMsecs > 500 && m_waitingForResponse)
		{
			//5 seconds
			qDebug() << "TIMEOUT waiting for response to payload:" << QString::number(m_payloadWaitingForResponse);
			m_waitingForResponse = false;
		}
		while (serialThread->bufferSize() != 0)
		{
			QByteArray packet = serialThread->readPacket();
			QPair<QByteArray,QByteArray> packetpair = parseBuffer(packet);
			if (packetpair.first.size() >= 3)
			{
				unsigned int payloadid = (unsigned int)packetpair.first[1] << 8;
				payloadid += (unsigned char)packetpair.first[2];


				if (payloadid != 0x0191)
				{
					qDebug() << "Non debug packet:" << QString::number(payloadid);
					if (m_waitingForResponse)
					{
						qDebug() << "Waiting for response: " << QString::number(m_payloadWaitingForResponse);
					}

				}
				if (m_waitingForResponse)
				{

					if (payloadid == m_payloadWaitingForResponse+1)
					{
						if (packetpair.first[0] & 0b00000010)
						{
							//NAK to our packet
							unsigned short errornum = packetpair.second[0] << 8;
							errornum += packetpair.second[1];
							emit commandFailed(m_currentWaitingRequest.sequencenumber,errornum);
						}
						else
						{
							//Packet is good.
							emit commandSuccessful(m_currentWaitingRequest.sequencenumber);
						}
						m_waitingForResponse = false;
					}
				}
				if (payloadid == 0x0191)
				{	//Datalog packet

					if (packetpair.first[0] & 0b00000010)
					{
						//NAK

						//emit commandFailed(int sequencenumber,int errornum);
					}
					else
					{
						emit dataLogPayloadReceived(packetpair.first,packetpair.second);
					}
				}
				else if (payloadid == 0xDA5F)
				{
					if (packetpair.first[0] & 0b00000010)
					{
					}
					else
					{
						//TODO double check to make sure that there aren't an odd number of items here...
						QList<unsigned short> idlist;
						for (int j=0;j<packetpair.second.size();j+=2)
						{
							unsigned short tmp = 0;
							tmp += packetpair.second[j] << 8;
							tmp += packetpair.second[j+1];
							idlist.append(tmp);
						}
						emit locationIdList(idlist);
					}
				}
				else if (payloadid == 0xF8E1) //Location ID Info
				{
					if (packetpair.first[0] & 0b00000010)
					{
					}
					else
					{
						//TODO double check to make sure that there aren't an odd number of items here...
						//QList<unsigned short> idlist;
						QList<LocationIdFlags> flaglist;
						if (packetpair.second.size() >= 2)
						{
							unsigned short test = packetpair.second[0] << 8;
							unsigned short parent;
							unsigned char rampage;
							unsigned char flashpage;
							unsigned short ramaddress;
							unsigned short flashaddress;
							unsigned short size;
							test += packetpair.second[1];
							for (int j=0;j<m_blockFlagList.size();j++)
							{
								if (test & m_blockFlagList[j])
								{
									flaglist.append(m_blockFlagList[j]);
								}
							}
							parent = packetpair.second[2] << 8;
							parent += packetpair.second[3];
							if (test & BLOCK_IS_RAM && test & BLOCK_IS_FLASH)
							{
								rampage = packetpair.second[4];
								flashpage = packetpair.second[5];
								ramaddress = packetpair.second[6] << 8;
								ramaddress += packetpair.second[7];
								flashaddress = packetpair.second[8] << 8;
								flashaddress += packetpair.second[9];
								size = packetpair.second[10] << 8;
								size += packetpair.second[11];
							}
							else if (test & BLOCK_IS_RAM)
							{
								rampage = packetpair.second[4];
								ramaddress = packetpair.second[5] << 8;
								ramaddress += packetpair.second[6];
								size = packetpair.second[7] << 8;
								size += packetpair.second[8];

							}
							else if (test & BLOCK_IS_FLASH)
							{
								flashpage = packetpair.second[4];
								flashaddress = packetpair.second[5] << 8;
								flashaddress += packetpair.second[6];
								size = packetpair.second[7] << 8;
								size += packetpair.second[8];
							}
							emit locationIdInfo(flaglist,parent,rampage,flashpage,ramaddress,flashaddress,size);
						}


						//emit locationIdList(idlist);
					}
				}
				else if (payloadid == 0x0001) //Interface version response
				{
					//Handle interface version
					if (packetpair.first[0] & 0b00000010)
					{
						//NAK
						qDebug() << "IFACE VERSION NAK";
					}
					else
					{
						emit interfaceVersion(QString(packetpair.second));
					}
				}
				else if (payloadid == 0x0003) //Firmware version response
				{
					if (packetpair.first[0] & 0b00000010)
					{
						//NAK
						qDebug() << "FIRMWARE VERSION NAK";
					}
					else
					{
						emit firmwareVersion(QString(packetpair.second));
					}
				}
				else if (payloadid == 0x0105)
				{
					if (packetpair.first[0] & 0b00000010)
					{
					}
					else
					{
						//Block from ram is here.
						emit blockRetrieved(m_currentWaitingRequest.sequencenumber,packetpair.first,packetpair.second);
					}
				}
				else
				{
					emit unknownPacket(packetpair.first,packetpair.second);
				}
			}
		}
	}
}
void FreeEmsComms::setLogFileName(QString filename)
{
	serialThread->setLogFileName(filename);
}

QPair<QByteArray,QByteArray> FreeEmsComms::parseBuffer(QByteArray buffer)
{
	if (buffer.size() <= 3)
	{

		qDebug() << "Not long enough to even contain a header!";
		return QPair<QByteArray,QByteArray>();
	}

	//Trim off 0xAA and 0xCC from the start and end
	buffer = buffer.mid(1);
	buffer = buffer.mid(0,buffer.length()-1);

	//qDebug() << "Packet:" << QString::number(buffer[1],16) << QString::number(buffer[buffer.length()-2],16);
	QByteArray header;
	//currPacket.clear();
	//Parse the packet here
	int headersize = 3;
	int iloc = 0;
	bool seq = false;
	bool len = false;
	if (buffer[iloc] & 0b00000100)
	{
		//Has header
		seq = true;
		//qDebug() << "Has seq";
		headersize += 1;
	}
	if (buffer[iloc] & 0b00000001)
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
	//qDebug() << QString::number(payloadid,16);
	//qDebug() << QString::number(buffer[iloc-1],16);
	//qDebug() << QString::number(buffer[iloc],16);
	//qDebug() << QString::number(buffer[iloc+1],16);
	//qDebug() << QString::number(buffer[iloc+2],16);
	iloc += 2;
	//qDebug() << QString::number(payloadid,16);
	if (seq)
	{
		//qDebug() << "Sequence number" << QString::number(currPacket[iloc]);
		iloc += 1;
	}
	QByteArray payload;
	if (len)
	{
		//qDebug() << "Length found, buffer size:" << buffer.length() << "iloc:" << QString::number(iloc);
		unsigned int length = buffer[iloc] << 8;
		length += buffer[iloc+1];
		//qDebug() << "Length:" << length;
		iloc += 2;
		//curr += length;
		payload.append(buffer.mid(iloc,length));
	}
	else
	{
		//qDebug() << "Buffer length:" << buffer.length();
		//qDebug() << "Attempted cut:" << buffer.length() - iloc;
		payload.append(buffer.mid(iloc),(buffer.length()-iloc) -1);
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
	unsigned char sum = 0;
	for (int i=0;i<header.size();i++)
	{
		sum += header[i];
	}
	for (int i=0;i<payload.size();i++)
	{
		sum += payload[i];
	}
	//qDebug() << "Payload sum:" << QString::number(sum);
	//qDebug() << "Checksum sum:" << QString::number((unsigned char)currPacket[currPacket.length()-1]);
	if (sum != (unsigned char)buffer[buffer.length()-1])
	{
		qDebug() << "BAD CHECKSUM!";
		qDebug() << "header size:" << header.size();
		qDebug() << "payload size:" << payload.size();
	}
	else
	{
		return QPair<QByteArray,QByteArray>(header,payload);
		//qDebug() << "Got full packet. Header length:" << header.length() << "Payload length:" << payload.length();
		/*for (int i=0;i<m_dataFieldList->size();i++)
		{
			//ui.tableWidget->item(i,1)->setText(QString::number(m_dataFieldList[i].getValue(&payload)));
		}*/
		//payload is our actual data.
		//unsigned int rpm = (payload[26] << 8) + payload[27];

		//qDebug() << "f" << f.getValue(&payload);
		//qDebug() << QString::number(rpm);
		//qDebug() << QString::number(((unsigned short)payload[8] << 8) + (unsigned short)payload[9]);
	}
	return QPair<QByteArray,QByteArray>();
}


