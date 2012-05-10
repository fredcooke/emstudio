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
QByteArray FreeEmsComms::generatePacket(QByteArray header,QByteArray payload)
{
	QByteArray packet;
	packet.append(0xAA);
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
	}
	//packet.append(header);
	//packet.append(payload);
	packet.append(0xCC);
	return packet;
}

void FreeEmsComms::run()
{
	bool serialconnected = false;
	bool waitingforresponse=false;
	int waitingpayloadid=0;
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
				serialconnected = true;
				if (serialThread->openPort(m_threadReqList[i].args[0].toString(),m_threadReqList[i].args[1].toInt()))
				{
					qDebug() << "Unable to connect to COM port";
					emit error("Unable to connect to com port " + m_threadReqList[i].args[0].toString() + " at baud " + QString::number(m_threadReqList[i].args[1].toInt()));
				}
				m_threadReqList.removeAt(i);
				i--;
			}
			else if (m_threadReqList[i].type == UPDATE_BLOCK_IN_RAM)
			{
				if (!m_waitingForResponse)
				{
					m_currentWaitingRequest = m_threadReqList[i];
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
					serialThread->writePacket(generatePacket(header,payload));
				}

			}
			else if (m_threadReqList[i].type == RETRIEVE_BLOCK_IN_RAM)
			{
				if (!m_waitingForResponse)
				{
					m_currentWaitingRequest = m_threadReqList[i];
					int location = m_threadReqList[i].args[0].toInt();
					int offset= m_threadReqList[i].args[1].toInt();
					int size = m_threadReqList[i].args[2].toInt();
					QByteArray header;
					QByteArray packet;
					header.append((char)0x00); //No Length, no seq no nak
					header.append((char)0x01); // Payload 0x0104, retrieve block
					header.append((char)0x04);
					packet.append((char)((location << 8) & 0xFF));
					packet.append((char)((location) & 0xFF));
					packet.append((char)((offset << 8) & 0xFF));
					packet.append((char)((offset) & 0xFF));
					packet.append((char)((size << 8) & 0xFF));
					packet.append((char)((size) & 0xFF));
					//header.append((char)(packet.length() << 8) & 0xFF);
					m_threadReqList.removeAt(i);
					i--;
					serialThread->writePacket(generatePacket(header,packet));
				}
			}
			else if (false)
			{
				serialThread->writePacket(QByteArray());
				waitingforresponse = true;
				waitingpayloadid = 0x0101;
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
		while (serialThread->bufferSize() != 0)
		{
			QByteArray packet = serialThread->readPacket();
			QPair<QByteArray,QByteArray> packetpair = parseBuffer(packet);
			if (packetpair.first.size() >= 3)
			{
				unsigned int payloadid = (unsigned int)packetpair.first[1] << 8;
				payloadid += (unsigned char)packetpair.first[2];
				if (m_waitingForResponse)
				{
					if (payloadid == m_payloadWaitingForResponse+1)
					{
						if (packetpair.first[0] & 0b00000010)
						{
							//NAK to our packet
							emit commandFailed(m_currentWaitingRequest.sequencenumber,0);
						}
						else
						{
							//Packet is good.
							emit commandSuccessfull(m_currentWaitingRequest.sequencenumber);
						}
						m_waitingForResponse = false;
					}
				}
				if (payloadid == 0x0191)
				{	//Datalog packet

					if (packetpair.first[0] & 0b00000010)
					{
						//NAK
					}
					else
					{
						emit dataLogPayloadReceived(packetpair.first,packetpair.second);
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


