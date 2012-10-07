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
#include "serialthread.h"
#include <QDateTime>
SerialThread::SerialThread(QObject *parent) : QThread(parent)
{
	m_logInFile = 0;
	m_logInOutFile = 0;
	m_logOutFile = 0;
	m_interByteSendDelay=0;
	m_logFileName = "IOLog";
	m_inpacket = false;
	m_inescape = false;
	m_logFileName = "log";
	m_logsEnabled = false;
	m_packetErrorCount=0;
}
void SerialThread::openLogs()
{
	m_logInFile = new QFile(m_logDirectory + "/" + m_logFileName + ".bin");
	m_logInFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	m_logInOutFile = new QFile(m_logDirectory + "/" + m_logFileName + ".both.bin");
	m_logInOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	m_logOutFile = new QFile(m_logDirectory + "/" + m_logFileName + ".toecu.bin");
	m_logOutFile->open(QIODevice::ReadWrite | QIODevice::Truncate);

}

void SerialThread::setPort(QString portname)
{
	m_portName = portname;
}
void SerialThread::setLogDirectory(QString dir)
{
	m_logDirectory = dir;
}

void SerialThread::setLogsEnabled(bool enabled)
{
	m_logsEnabled = enabled;
}

void SerialThread::setBaud(int baudrate)
{
	m_baud = baudrate;
}
bool SerialThread::verifyFreeEMS(QString portname)
{
	openPort(portname,115200,false);
	unsigned char ret = 0x0D;
	int writei =0;
#ifdef Q_OS_WIN32
	::WriteFile(m_portHandle, (void*)&ret, (DWORD)1, (LPDWORD)&writei, NULL);
#else
	writei = write(m_portHandle,&ret,1);
#endif
	if (writei <= 0)
	{
		qDebug() << "Error writing to verify FreeEMS";
		return false;
	}
	unsigned char buf[3];
	msleep(100);
	int count = 0;
#ifdef Q_OS_WIN32
	::ReadFile(m_portHandle,(LPVOID)buf,3,(LPDWORD)&count,NULL);
#else
	count = read(m_portHandle,buf,3);
#endif
	if (count > 0)
	{
		qDebug() << "Verify:" << QString::number(buf[0],16);
		qDebug() << "Verify:" << QString::number(buf[1],16);
		if (buf[0] == 0xE0 || buf[0] == 0xE1)
		{
			if (count > 1)
			{
				qDebug() << "Verify:" << QString::number(buf[2],16);
				if (buf[2] == 0x3E)
				{
					//Serial monitor running
					closePort();
					return false;
				}
				else
				{
					//Probably not;
					closePort();
					return true;
				}
			}
		}
	}
	//nothing on the port here either.
	closePort();
	return true;
}

void SerialThread::setLogFileName(QString filename)
{
	m_logFileName = filename;
	if (m_logInFile || m_logInOutFile || m_logOutFile)
	{
		if (m_logInFile)
		{
			m_logInFile->close();
		}
		if (m_logInOutFile)
		{
			m_logInOutFile->close();
		}
		if (m_logOutFile)
		{
			m_logOutFile->close();
		}
		if (m_logsEnabled)
		{
			openLogs();
		}
	}
	//m_logFile = new QFile(m_logFileName);
}
void SerialThread::setInterByteSendDelay(int milliseconds)
{
	m_interByteSendDelay = milliseconds;
}

int SerialThread::readSerial(int timeout)
{
	if (!m_logInFile || !m_logInOutFile)
	{
		if (m_logsEnabled)
		{
			openLogs();
		}

	}
	qint64 currms = QDateTime::currentMSecsSinceEpoch();
	int readlen = m_buffer.size();
	QByteArray qbuffer;
	if (m_buffer.size() > 10240)
	{
		//Error here somehow;
		Q_ASSERT(m_buffer.size() < 10240);
	}
	unsigned char buffer[10240];
	qbuffer.append(m_buffer);
	/*for (int i=0;i<m_buffer.size();i++)
	{
		buffer[i] = m_buffer[i];
		qbuffer.append(m_buffer[i]);
	}*/
	m_buffer.clear();
	//bool inpacket = false;
	//bool inescape = false;
	QString byteoutofpacket;
	while (currms + timeout > QDateTime::currentMSecsSinceEpoch())
	{
#ifdef Q_OS_WIN32
		if (!ReadFile(m_portHandle,(LPVOID)buffer,1024,(LPDWORD)&readlen,NULL))
		{
			//Serial error here
			qDebug() << "Serial Read error";
		}
#else
		readlen = read(m_portHandle,buffer,1024);
#endif //Q_OS_WIN32
		if (readlen < 0)
		{
			//Nothing on the port
			//qDebug() << "Timeout";
			msleep(10);
		}
		else
		{
			if (m_logsEnabled)
			{
				m_logInFile->write((const char*)buffer,readlen);
				m_logInFile->flush();
			}
		}
		if (readlen == 0)
		{
			//This should be an error
			qDebug() << "Nothing to read";
			perror("Error:");
			printf("\n");
			return -1;
			msleep(10);
		}
		for (int i=0;i<readlen;i++)
		{
			if (buffer[i] == 0xAA)
			{
				if (m_inpacket)
				{
					//Start byte in the middle of a packet
					//Clear out the buffer and start fresh
					m_inescape = false;
					if (m_logsEnabled)
					{
						m_logWriteMutex.lock();
						m_logInOutFile->write(QByteArray().append(0xAA));
						QByteArray nbuffer(qbuffer);
						nbuffer.replace(0xBB,QByteArray().append(0xBB).append(0x44));
						nbuffer.replace(0xAA,QByteArray().append(0xBB).append(0x55));
						nbuffer.replace(0xCC,QByteArray().append(0xBB).append(0x33));
						m_logInOutFile->write((const char*)nbuffer.data(),nbuffer.size());
						m_logInOutFile->write(QByteArray().append(0xCC));
						m_logInOutFile->flush();
						m_logWriteMutex.unlock();
					}
					qbuffer.clear();
					qDebug() << "Buffer error";
					m_packetErrorCount++;
				}
				//qbuffer.append(buffer[i]);
				//qDebug() << "Start of packet";
				//Start of packet
				m_inpacket = true;
			}
			else if (buffer[i] == 0xCC && m_inpacket)
			{
				//qDebug() << "End of packet. Size:" << qbuffer.size();
				//End of packet
				m_inpacket = false;
				//qbuffer.append(buffer[i]);

				//m_logFile->flush();
				//emit parseBuffer(qbuffer);


				//New Location of checksum
				unsigned char sum = 0;
				for (int i=0;i<qbuffer.size()-1;i++)
				{
					sum += qbuffer[i];
				}
				if (m_logsEnabled)
				{
					m_logWriteMutex.lock();
					m_logInOutFile->write(QByteArray().append(0xAA));
					QByteArray nbuffer(qbuffer);
					nbuffer.replace(0xBB,QByteArray().append(0xBB).append(0x44));
					nbuffer.replace(0xAA,QByteArray().append(0xBB).append(0x55));
					nbuffer.replace(0xCC,QByteArray().append(0xBB).append(0x33));
					m_logInOutFile->write((const char*)nbuffer.data(),nbuffer.size());
					m_logInOutFile->write(QByteArray().append(0xCC));
					m_logInOutFile->flush();
					m_logWriteMutex.unlock();
				}
				//qDebug() << "Payload sum:" << QString::number(sum);
				//qDebug() << "Checksum sum:" << QString::number((unsigned char)currPacket[currPacket.length()-1]);
				if (sum != (unsigned char)qbuffer[qbuffer.size()-1])
				{
					qDebug() << "BAD CHECKSUM!";
					m_packetErrorCount++;
					//return QPair<QByteArray,QByteArray>();
				}
				else
				{
					m_packetErrorCount=0;
					m_queuedMessages.append(qbuffer.mid(0,qbuffer.length()-1));
				}
				//return qbuffer;
				QString output;
				for (int i=0;i<qbuffer.size();i++)
				{
					int num = (unsigned char)qbuffer[i];
					output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
				}
				//qDebug() << "Full packet:";
				//qDebug() << output;
				qbuffer.clear();
			}
			else
			{
				if (m_inpacket && !m_inescape)
				{
					if (buffer[i] == 0xBB)
					{
						//Need to escape the next byte
						//retval = logfile.read(1);
						m_inescape = true;
					}
					else
					{
						qbuffer.append(buffer[i]);
					}

				}
				else if (m_inpacket && m_inescape)
				{
					if (buffer[i] == 0x55)
					{
						qbuffer.append((char)0xAA);
					}
					else if (buffer[i] == 0x44)
					{
						qbuffer.append((char)0xBB);
					}
					else if (buffer[i] == 0x33)
					{
						qbuffer.append((char)0xCC);
					}
					else
					{
						qDebug() << "Error, escaped character is not valid!:" << QString::number(buffer[i],16);
						m_packetErrorCount++;
					}
					m_inescape = false;
				}
				else
				{
					//qDebug() << "Byte out of a packet:" << QString::number(buffer[i],16);
					byteoutofpacket += QString::number(buffer[i],16) + " ";
				}
			}
		}
		//qDebug() << "Bytes out of a packet:" << byteoutofpacket;
	}
	m_buffer.append(qbuffer);
	if (readlen > 0)
	{
	//m_buffer.write(buffer,readlen);
	//	m_buffer.append((const char*)buffer,readlen);
	}
	return m_packetErrorCount;
}
int SerialThread::writePacket(QByteArray packet)
{
	if (!m_logInFile || !m_logInOutFile)
	{
		if (m_logsEnabled)
		{
			openLogs();
		}
	}
	if (m_logsEnabled)
	{
		m_logWriteMutex.lock();
		m_logInOutFile->write(packet);
		m_logInOutFile->flush();
		m_logOutFile->write(packet);
		m_logOutFile->flush();
		m_logWriteMutex.unlock();
	}
#ifdef Q_OS_WIN32
	int len=0;
	for (int i=0;i<packet.size();i++)
	{
		char c = packet.data()[i];
		if (!::WriteFile(m_portHandle, (void*)&c, (DWORD)1, (LPDWORD)&len, NULL))
		{
			qDebug() << "Serial Write Error";
			return -1;
		}
		msleep(m_interByteSendDelay);
	}
	return 0;
#else
	for (int i=0;i<packet.size();i++)
	{
		char c = packet.data()[i];
		if (write(m_portHandle,&c,1)<0)
		{
			//TODO: Error here
			qDebug() << "Serial write error";
			return -1;
		}
		msleep(m_interByteSendDelay);
	}
	return 0;
#endif //Q_OS_WIN32
}

QByteArray SerialThread::readPacket()
{
	if (m_queuedMessages.size() > 0)
	{
		QByteArray ret = m_queuedMessages[0];
		m_queuedMessages.removeAt(0);
		return ret;
	}
	else
	{
		return QByteArray();
	}
}

void SerialThread::run()
{
	if (openPort(m_portName,m_baud))
	{
		qDebug() << "Error opening com port";
		return;
	}
	//m_logFile = new QFile(m_logFileName);
	//m_logFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	unsigned char buffer[1024];
	int readlen = 0;
	QByteArray qbuffer;
	bool inpacket= false;
	bool inescape=true;
	while (true)
	{
#ifdef Q_OS_WIN32
		if (!ReadFile(m_portHandle,(LPVOID)buffer,1024,(LPDWORD)&readlen,NULL))
		{
			//Serial error here
			qDebug() << "Serial Read error";
		}
#else
		readlen = read(m_portHandle,buffer,1024);
#endif //Q_OS_WIN32
		if (readlen == 0)
		{
			msleep(10);
		}
		else if (readlen == -1)
		{
			qDebug() << "Serial Read error";
		}
		for (int i=0;i<readlen;i++)
		{
			if (buffer[i] == 0xAA)
			{
				if (inpacket)
				{
					//Start byte in the middle of a packet
					//Clear out the buffer and start fresh
					inescape = false;
					qbuffer.clear();
				}
				qbuffer.append(buffer[i]);
				//qDebug() << "Start of packet";
				//Start of packet
				inpacket = true;
			}
			else if (buffer[i] == 0xCC && inpacket)
			{
				//qDebug() << "End of packet. Size:" << qbuffer.size();
				//End of packet
				inpacket = false;
				qbuffer.append(buffer[i]);
				//m_logFile->write(qbuffer);
				//m_logFile->flush();
				emit parseBuffer(qbuffer);
				QString output;
				for (int i=0;i<qbuffer.size();i++)
				{
					int num = (unsigned char)qbuffer[i];
					output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
				}
				//qDebug() << "Full packet:";
				//qDebug() << output;
				qbuffer.clear();
			}
			else
			{
				if (inpacket && !inescape)
				{
					if (buffer[i] == 0xBB)
					{
						//Need to escape the next byte
						//retval = logfile.read(1);
						inescape = true;
					}
					else
					{
						qbuffer.append(buffer[i]);
					}

				}
				else if (inpacket && inescape)
				{
					if (buffer[i] == 0x55)
					{
						qbuffer.append((char)0xAA);
					}
					else if (buffer[i] == 0x44)
					{
						qbuffer.append((char)0xBB);
					}
					else if (buffer[i] == 0x33)
					{
						qbuffer.append((char)0xCC);
					}
					inescape = false;
				}
			}
		}
	}
}
/*void SerialThread::sendMessageForResponse(QByteArray header,QByteArray payload)
{

}*/
void SerialThread::closePort()
{
#ifdef Q_OS_WIN32
	CloseHandle(m_portHandle);
#else
	flock(m_portHandle,LOCK_UN);
	close(m_portHandle);
#endif
}

int SerialThread::openPort(QString portName,int baudrate,bool oddparity)
{
#ifdef Q_OS_WIN32
	m_portHandle=CreateFileA(portName.toAscii(), GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_portHandle == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	COMMCONFIG Win_CommConfig;
	//COMMTIMEOUTS Win_CommTimeouts;
	unsigned long confSize = sizeof(COMMCONFIG);
	Win_CommConfig.dwSize = confSize;
	GetCommConfig(m_portHandle, &Win_CommConfig, &confSize);
	if (oddparity)
	{
		Win_CommConfig.dcb.Parity = 1; //Odd parity
	}
	else
	{
		Win_CommConfig.dcb.Parity = 0; //No parity
	}
	Win_CommConfig.dcb.fRtsControl = RTS_CONTROL_DISABLE;
	Win_CommConfig.dcb.fOutxCtsFlow = FALSE;
	Win_CommConfig.dcb.fOutxDsrFlow = FALSE;
	Win_CommConfig.dcb.fDtrControl = DTR_CONTROL_DISABLE;
	Win_CommConfig.dcb.fDsrSensitivity = FALSE;
	Win_CommConfig.dcb.fNull=FALSE;
	Win_CommConfig.dcb.fTXContinueOnXoff = FALSE;
	Win_CommConfig.dcb.fInX=FALSE;
	Win_CommConfig.dcb.fOutX=FALSE;
	Win_CommConfig.dcb.fBinary=TRUE;
	Win_CommConfig.dcb.DCBlength = sizeof(DCB);
	if (baudrate != -1)
	{
		Win_CommConfig.dcb.BaudRate = baudrate;
	}
	else
	{
		Win_CommConfig.dcb.BaudRate = 115200;
	}
	Win_CommConfig.dcb.ByteSize = 8;
	//Win_CommTimeouts.ReadIntervalTimeout = 50;
	//Win_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	//Win_CommTimeouts.ReadTotalTimeoutConstant = 110;
	//Win_CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	//Win_CommTimeouts.WriteTotalTimeoutConstant = 110;
	SetCommConfig(m_portHandle, &Win_CommConfig, sizeof(COMMCONFIG));
	//SetCommTimeouts(m_portHandle,&Win_CommTimeouts);
	return 0;
#else

	m_portHandle = open(portName.toAscii(),O_RDWR | O_NOCTTY | O_NDELAY); //Should open the port non blocking
	if (m_portHandle < 0)
	{
		//printf("Error opening Com: %s\n",portName);
		//debug(obdLib::DEBUG_ERROR,"Error opening com port %s",portName);
		perror("Error opening COM port Low Level");
		qDebug() << "Port:" << portName;
		return -1;
	}
	if (flock(m_portHandle,LOCK_EX | LOCK_NB))
	{
		qDebug() << "Unable to maintain lock on serial port" << portName;
		qDebug() << "This port is likely open in another process";
		return -2;
	}
	//printf("Com Port Opened %i\n",portHandle);
	//debug(obdLib::DEBUG_VERBOSE,"Com Port Opened %i",portHandle);
	//fcntl(m_portHandle, F_SETFL, FASYNC); //Set it to blocking. This is required? Wtf?
	//struct termios oldtio;
	struct termios newtio;
	//bzero(&newtio,sizeof(newtio));
	tcgetattr(m_portHandle,&newtio);
	long BAUD = B115200;
	switch (baudrate)
	{
		case 38400:
			BAUD = B38400;
			break;
		case 115200:
			BAUD  = B115200;
			break;
		case 19200:
			BAUD  = B19200;
			break;
		case 9600:
			BAUD  = B9600;
			break;
		case 4800:
			BAUD  = B4800;
			break;
		default:
			BAUD = B115200;
			break;
	}  //end of switch baud_rate
	if (strspn("/dev/pts",portName.toAscii()) >= 8)
	{
		//debug(obdLib::DEBUG_WARN,"PTS Detected... disabling baud rate selection on: %s",portName);
		//printf("PTS detected... disabling baud rate selection: %s\n",portName);
		baudrate = -1;
	}
	else
	{
	}
	newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
	newtio.c_cflag |= CLOCAL | CREAD;
	if (oddparity)
	{
		newtio.c_cflag |= (PARENB | PARODD);
	}
	else
	{
		newtio.c_cflag &= ~(PARENB | PARODD);
	}
	newtio.c_cflag &= ~CRTSCTS;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_iflag=IGNBRK;
	newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
	newtio.c_lflag=0;
	newtio.c_oflag=0;
	newtio.c_cc[VTIME]=1; //1/10th second timeout, to allow for quitting the read thread
	newtio.c_cc[VMIN]=1; //We want a pure timer timeout
	if (baudrate != -1)
	{
		if(cfsetispeed(&newtio, BAUD))
		{
			//perror("cfsetispeed");
		}

		if(cfsetospeed(&newtio, BAUD))
		{
			//perror("cfsetospeed");
		}
		//debug(obdLib::DEBUG_VERBOSE,"Setting baud rate to %i on port %s\n",baudrate,portName);
	}
	fcntl(m_portHandle, F_SETFL, 0); //Set to blocking
	tcsetattr(m_portHandle,TCSANOW,&newtio);
	return 0;
#endif //Q_OS_WIN32
}
