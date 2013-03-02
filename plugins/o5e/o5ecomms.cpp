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

#include "o5ecomms.h"
#include "o5edatapacketdecoder.h"
#include "serialport.h"

#include <QDebug>
#include <QMap>
#include <QStringList>

static quint32 Crc32_ComputeBuf(quint32 inCrc32, const void *buf, quint32 bufLen);


O5EComms::O5EComms(QObject *parent) : EmsComms(parent)
{
	qRegisterMetaType<QList<unsigned short> >("QList<unsigned short>");
	currentPacketNum=1;
	QTimer *timer = new QTimer();
	connect(timer,SIGNAL(timeout()),this,SLOT(packetCounter()));
	timer->start(1000);
	QFile inifile("../o5e.ini");
	inifile.open(QIODevice::ReadOnly);
	QString inistring = inifile.readAll();
	inifile.close();
	QStringList inilist = inistring.split("\n");
	QVariantMap map;
	QMap<QString,scalarclass> scalarMap;
	QString pagenum = "";
	foreach (QString line,inilist)
	{
		if (line.startsWith("page"))
		{
			if (pagenum != "")
			{
				pageMap[pagenum] = scalarMap;
			}
			//New page
			//Page line
			pagenum = line.split("=")[1];
			pagenum = pagenum.mid(0,pagenum.indexOf(";")).trimmed();
			qDebug() << "Page Num:" << pagenum;
		}
		else if (!line.startsWith(";"))
		{
			QStringList linesplit = line.split("=");
			if (linesplit.size() > 1)
			{
				QStringList linevalsplit = linesplit[1].split(",");
				if (linevalsplit.size() > 1)
				{
					if (linevalsplit[0].trimmed() == "scalar")
					{
						//Scalar
						scalarclass scalar;
						//qDebug() << linevalsplit[2].trimmed() << linevalsplit[4].trimmed();
						scalar.offset = linevalsplit[2].trimmed().toInt();
						if (linevalsplit[4].trimmed().startsWith("."))
						{
							scalar.scale = QString("0").append(linevalsplit[4].trimmed()).toFloat();
						}
						else
						{
							scalar.scale = linevalsplit[4].trimmed().toFloat();
						}
						if (linevalsplit[5].trimmed().startsWith("."))
						{
							scalar.translate = QString("0").append(linevalsplit[5].trimmed()).toFloat();
						}
						else
						{
							scalar.translate = linevalsplit[5].trimmed().toFloat();
						}
						scalarMap[linesplit[0].trimmed()] = scalar;

						if (linevalsplit[1].trimmed() == "S16")
						{
							scalar.size = 2;
							scalar.signedval = true;
						}
						else if (linevalsplit[1].trimmed() == "U08")
						{
							scalar.size = 1;
							scalar.signedval = false;
						}
						else if (linevalsplit[1].trimmed() == "U16")
						{
							scalar.size = 2;
							scalar.signedval = false;
						}
						else if (linevalsplit[1].trimmed() == "U32")
						{
							scalar.size = 4;
							scalar.signedval = false;
						}
						else if (linevalsplit[1].trimmed() == "S32")
						{
							scalar.size = 4;
							scalar.signedval = true;
						}
					}
				}
			}
		}
	}
	/*for (QMap<QString,QMap<QString,scalarclass> >::const_iterator i = pageMap.constBegin();i!=pageMap.constEnd();i++)
	{
		for (QMap<QString,scalarclass>::const_iterator j = i.value().constBegin();j!=i.value().constEnd();j++)
		{
			//qDebug() << "Page:" << i.key() << "Scalar:" << j.key() << j.value().offset << j.value().scale << j.value().translate;
		}
	}*/
}

void O5EComms::stop()
{

}

void O5EComms::setLogsEnabled(bool enabled)
{

}

DataPacketDecoder *O5EComms::getDecoder()
{
	return new O5EDataPacketDecoder();
}

Table3DData *O5EComms::getNew3DTableData()
{
	return 0;
}

Table2DData *O5EComms::getNew2DTableData()
{
	return 0;
}

void O5EComms::setLogDirectory(QString dir)
{

}

void O5EComms::setPort(QString portname)
{

}

void O5EComms::setBaud(int baudrate)
{

}

void O5EComms::setLogFileName(QString filename)
{

}

bool O5EComms::sendPacket(unsigned short payloadid,QList<QVariant> arglist,QList<int> argsizelist,bool haslength)
{
	return false;
}

int O5EComms::getLocationIdInfo(unsigned short locationid)
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_LOCATION_ID_INFO;
	req.sequencenumber = currentPacketNum++;
	req.addArg(locationid,2);
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getInterfaceVersion()
{
	qDebug() << "getInterfaceVersion";
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_INTERFACE_VERSION;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getFirmwareVersion()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_FIRMWARE_VERSION;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getMaxPacketSize()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_MAX_PACKET_SIZE;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getDecoderName()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_DECODER_NAME;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getFirmwareBuildDate()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_FIRMWARE_BUILD_DATE;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getCompilerVersion()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_COMPILER_VERSION;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::getOperatingSystem()
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_OPERATING_SYSTEM;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::echoPacket(QByteArray packet)
{
	return 0;
}

int O5EComms::getLocationIdList(unsigned char listtype, unsigned short listmask)
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = GET_LOCATION_ID_LIST;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::softReset()
{
	return 0;
}

int O5EComms::hardReset()
{
	return 0;
}

bool O5EComms::sendSimplePacket(unsigned short payload)
{
	return false;
}

void O5EComms::connectSerial(QString port,int baud)
{
	emit connected();
}

void O5EComms::disconnectSerial()
{

}

void O5EComms::loadLog(QString filename)
{

}

void O5EComms::playLog()
{

}

//virtual void populateDataFields()=0;
QByteArray O5EComms::generatePacket(QByteArray header,QByteArray payload)
{
	return QByteArray();
}

int O5EComms::updateBlockInRam(unsigned short location,unsigned short offset, unsigned short size,QByteArray data)
{
	return 0;
}

int O5EComms::updateBlockInFlash(unsigned short location,unsigned short offset, unsigned short size,QByteArray data)
{
	return 0;
}

int O5EComms::retrieveBlockFromRam(unsigned short location, unsigned short offset, unsigned short size)
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = RETRIEVE_BLOCK_IN_RAM;
	req.sequencenumber = currentPacketNum++;
	req.addArg(location,2);
	m_reqList.append(req);
	return currentPacketNum-1;

}

int O5EComms::retrieveBlockFromFlash(unsigned short location, unsigned short offset, unsigned short size)
{
	QMutexLocker locker(&reqListMutex);
	RequestClass req;
	req.type = RETRIEVE_BLOCK_IN_FLASH;
	req.sequencenumber = currentPacketNum++;
	req.addArg(location,2);
	m_reqList.append(req);
	return currentPacketNum-1;
}

int O5EComms::burnBlockFromRamToFlash(unsigned short location,unsigned short offset, unsigned short size)
{
	return 0;
}

void O5EComms::setInterByteSendDelay(int milliseconds)
{

}

void O5EComms::setlogsDebugEnabled(bool enabled)
{

}
void O5EComms::packetCounter()
{
	qDebug() << "PPS:" << currentPacketCount;
	currentPacketCount = 0;
}
QByteArray O5EComms::readPacket(SerialPort *port)
{
	bool makenextwrite = true;
	int looptime = 0;
	unsigned int packetnum = 0;
	unsigned char buf[1024];
	int lenbytesread = 0;
	int totallenbytesread=0;
	int lenbyteslefttoread=2;
	int timeout = 0;
	QByteArray lenbytes;
	while (totallenbytesread < 2 && timeout < 500)
	{
		lenbytesread = port->readBytes(buf,lenbyteslefttoread);
		totallenbytesread+=lenbytesread;
		lenbyteslefttoread-=lenbytesread;
		lenbytes.append((const char*)buf,lenbytesread);
		timeout++;
		usleep(1000);
	}
	if (lenbytesread == 0)
	{
		usleep(1000);
		looptime++;
		if (looptime > 400)
		{
			looptime = 0;
			makenextwrite = true;
			port->flush();
			qDebug() << "No bytes, breaking out and continuing";
		}
		qDebug() << "No bytes, breaking out";
		return QByteArray();
	}
	looptime = 0;
	unsigned int newlen = (((unsigned char)lenbytes[0]) << 8) + (unsigned char)lenbytes[1];
	unsigned int datalenread =0;
	QByteArray packet;
	int timeoutcount = 0;
	unsigned int datatoberead = newlen;
	unsigned int currentdataread = 0;
	while (datalenread != newlen )
	{
		usleep(10000);
		currentdataread = port->readBytes(buf,datatoberead);
		datalenread += currentdataread;
		datatoberead -= datalenread;
		packet.append((const char*)buf,currentdataread);
		//qDebug() << "Timeout" << datalenread;
		timeoutcount++;
		if (timeoutcount > 10)
		{
			break;
		}
	}
	//Read the data
	if (datalenread == 0)
	{
		port->readBytes(buf,1); //Read the single code
		qDebug() << "Code:" << buf[0];
	}
	//qDebug() << "Code:" << QString::number((unsigned char)packet[0],16);
	if (datalenread == newlen && datalenread != 0)
	{
		QByteArray crcbytes;
		datalenread = 0;
		newlen = 4;
		currentdataread = 0;
		datatoberead=newlen;
		while (datalenread != newlen )
		{
			usleep(5000);
			currentdataread = port->readBytes(buf,datatoberead);
			datalenread += currentdataread;
			datatoberead -= datalenread;
			crcbytes.append((const char*)buf,currentdataread);
			//qDebug() << "CRC Timeout" << datalenread;
			timeoutcount++;
			if (timeoutcount > 20)
			{
				break;
			}
		}
		//qDebug() << "Code:" << QString::number((unsigned char)packet[0],16);
		unsigned int newcrc = ((unsigned char)crcbytes[0]) << 24;
		newcrc += ((unsigned char)crcbytes[1]) << 16;
		newcrc += ((unsigned char)crcbytes[2]) << 8;
		newcrc += ((unsigned char)crcbytes[3]);
		if (newcrc != Crc32_ComputeBuf(0,packet.data(),packet.size()))
		{
			qDebug() << "Error, invalid CRC";
			qDebug() << "Packet size:" << packet.size();
			makenextwrite = true;
			port->flush();
			//continue;
			return QByteArray();
		}
		if (packet[0] == 0x00)
		{
			// qDebug() << "Packet" << packet.size();
			currentPacketCount++;
			//emit dataLogPayloadReceived(QByteArray(),packet);
			unsigned int clearlen = port->readBytes(buf,1024); //CRC
			return packet;
		}
		if (datalenread == 4)
		{
			makenextwrite = true;
		}
	}
	unsigned int clearlen = port->readBytes(buf,1024); //CRC
	//qDebug() << "Clearlen:" << clearlen;
	//usleep(10000);
	int stopper = 1;
	return QByteArray();
}
void O5EComms::run()
{
	while (true)
	{
		//qDebug() << "Loop:" << m_reqList.size();
		/*	QMutexLocker(reqListMutex);
	RequestClass req;
	req.type = GET_INTERFACE_VERSION;
	req.sequencenumber = currentPacketNum++;
	m_reqList.append(req);
	return currentPacketNum;*/
		reqListMutex.lock();
		for (int i=0;i<m_reqList.size();i++)
		{
			m_privReqList.append(m_reqList[i]);
		}
		m_reqList.clear();
		reqListMutex.unlock();
		for (int i=0;i<m_privReqList.size();i++)
		{
			if (m_privReqList[i].type == GET_INTERFACE_VERSION)
			{
				usleep(100000);
				emit interfaceVersion("O5E Simulator");
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_FIRMWARE_VERSION)
			{
				usleep(100000);
				emit firmwareVersion("0.0.1");
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_MAX_PACKET_SIZE)
			{
				usleep(100000);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_LOCATION_ID_LIST)
			{
				usleep(100000);
				QList<unsigned short> idlists;
				for (QMap<QString,QMap<QString,scalarclass> >::const_iterator j = pageMap.constBegin();j!=pageMap.constEnd();j++)
				{
					idlists.append(j.key().toInt());
					/*for (QMap<QString,scalarclass>::const_iterator j = i.value().constBegin();j!=i.value().constEnd();j++)
					{
						//qDebug() << "Page:" << i.key() << "Scalar:" << j.key() << j.value().offset << j.value().scale << j.value().translate;
					}*/
				}
				emit locationIdList(idlists);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_DECODER_NAME)
			{
				usleep(100000);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_FIRMWARE_BUILD_DATE)
			{
				usleep(100000);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_COMPILER_VERSION)
			{
				usleep(100000);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_OPERATING_SYSTEM)
			{
				usleep(100000);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == GET_LOCATION_ID_INFO)
			{
				usleep(100000);
				unsigned short locid = m_privReqList[i].args[0].toInt();
				//void locationIdInfo(unsigned short locationid,MemoryLocationInfo info);
				MemoryLocationInfo info;
				info.isRam = true;
				info.isFlash = true;
				info.locationid = locid;
				info.size = 1024;
				info.type = DATA_CONFIG;
				emit locationIdInfo(locid,info);
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == RETRIEVE_BLOCK_IN_RAM)
			{
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}
			else if (m_privReqList[i].type == RETRIEVE_BLOCK_IN_FLASH)
			{
				emit commandSuccessful(m_privReqList[i].sequencenumber);
			}

		}
		m_privReqList.clear();
		usleep(10000); //Sleep to avoid CPU spinning
	}
	emit connected();
	SerialPort port;
	int openerror = port.openPort("/dev/ttyACM0",115200,false);
	qDebug() << "Opened:" << openerror;
	if (openerror < 0)
	{
		return;
	}

	unsigned char buf[1024];

	bool makenextwrite = true;
	int looptime = 0;
	unsigned int packetnum = 0;

	buf[0] = 'Q';
	port.flush();
	port.writeBytes(buf,1);
	usleep(1000000);
	int count = port.readBytes(buf,200);
	qDebug() << "Version size:" << count;
	QString verstr = "";
	for (int i=0;i<count;i++)
	{
		verstr += QString::number(buf[i],16) + ",";
	}
	qDebug() << verstr;
	qDebug() << "Version:" << QString::fromAscii((const char*)buf+count-20,19);

	buf[0] = 0;
	buf[1] = 7;
	buf[2]  = 'r';
	buf[3] = 0; //Page
	buf[4] = 1; //Page
	buf[5] = 0; //Offset
	buf[6] = 0; //Offset
	buf[7] = 0; //Length
	buf[8] = 0xFF; //Length
	quint32 crc = Crc32_ComputeBuf(0,buf + 2,7);
	buf[9] = crc >> 24;
	buf[10] = crc >> 16;
	buf[11] = crc >> 8;
	buf[12] = crc >> 0;
	//port.writeBytes(buf,13);
	//usleep(5000000);
	//QByteArray response = readPacket(&port);
	//qDebug() << "Page 1:" << response.size();
	while (true)
	{
		if (makenextwrite)
		{
			buf[0] = 0;
			buf[1] = 1;
			buf[2] = 'A';
			quint32 crc = Crc32_ComputeBuf(0,buf + 2,1);
			buf[3] = crc >> 24;
			buf[4] = crc >> 16;
			buf[5] = crc >> 8;
			buf[6] = crc >> 0;
			//qDebug() << "Writing:" << port.writeBytes(buf,7);
			port.writeBytes(buf,7);
			//usleep(13000);
			makenextwrite = true;
		}
		QByteArray reply = readPacket(&port);
		if (reply.size() > 0)
		{
			emit dataLogPayloadReceived(QByteArray(),reply);
		}
		int stopper = 1;
	}
}
/*----------------------------------------------------------------------------*\
 *  NAME:
 *     Crc32_ComputeBuf() - computes the CRC-32 value of a memory buffer
 *  DESCRIPTION:
 *     Computes or accumulates the CRC-32 value for a memory buffer.
 *     The 'inCrc32' gives a previously accumulated CRC-32 value to allow
 *     a CRC to be generated for multiple sequential buffer-fuls of data.
 *     The 'inCrc32' for the first buffer must be zero.
 *  ARGUMENTS:
 *     inCrc32 - accumulated CRC-32 value, must be 0 on first call
 *     buf     - buffer to compute CRC-32 value for
 *     bufLen  - number of bytes in buffer
 *  RETURNS:
 *     crc32 - computed CRC-32 value
 *  ERRORS:
 *     (no errors are possible)
\*----------------------------------------------------------------------------*/

static quint32 Crc32_ComputeBuf(quint32 inCrc32, const void *buf, quint32 bufLen)
{
    static const quint32 crcTable[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535,
	0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD,
	0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D,
	0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4,
	0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
	0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC,
	0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB,
	0xB6662D3D, 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F,
	0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB,
	0x086D3D2D, 0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA,
	0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 0x4DB26158, 0x3AB551CE,
	0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A,
	0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409,
	0xCE61E49F, 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
	0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739,
	0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344, 0x8708A3D2, 0x1E01F268,
	0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0,
	0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8,
	0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF,
	0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703,
	0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7,
	0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE,
	0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6,
	0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D,
	0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5,
	0x47B2CF7F, 0x30B5FFE9, 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605,
	0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    quint32 crc32;
    unsigned char *byteBuf;
    size_t i;

    /** accumulate crc32 for buffer **/
    crc32 = inCrc32 ^ 0xFFFFFFFF;
    byteBuf = (unsigned char *)buf;

    for (i = 0; i < bufLen; i++) {
	crc32 = (crc32 >> 8) ^ crcTable[(crc32 ^ byteBuf[i]) & 0xFF];
    }

    return (crc32 ^ 0xFFFFFFFF);
}

Q_EXPORT_PLUGIN2(MegaSquirtPlugin, O5EComms)
