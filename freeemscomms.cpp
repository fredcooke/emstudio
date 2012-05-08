#include "freeemscomms.h"
#include <QDebug>
FreeEmsComms::FreeEmsComms(QObject *parent) : QThread(parent)
{
	serialThread = new SerialThread(this);
	connect(serialThread,SIGNAL(parseBuffer(QByteArray)),this,SLOT(parseBuffer(QByteArray)));

	logLoader = new LogLoader(this);
	connect(logLoader,SIGNAL(parseBuffer(QByteArray)),this,SLOT(parseBuffer(QByteArray)));
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

void FreeEmsComms::run()
{
	bool serialconnected = false;
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
			}
		}
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
				if (payloadid == 0x0191)
				{	//Datalog packet

					if (packetpair.first[0] & 0b00000010)
					{
						//NAK
					}
					else
					{
						emit dataLogPayloadReceived(header,payload);
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


