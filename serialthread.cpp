#include "serialthread.h"
#include <QDateTime>
SerialThread::SerialThread(QObject *parent) : QThread(parent)
{
}
void SerialThread::setPort(QString portname)
{
	m_portName = portname;
}

void SerialThread::setBaud(int baudrate)
{
	m_baud = baudrate;
}
void SerialThread::setLogFileName(QString filename)
{
	m_logFileName = filename;
}
void SerialThread::readSerial(int timeout)
{
	qint64 currms = QDateTime::currentMSecsSinceEpoch();
	int readlen = m_buffer.size();
	QByteArray qbuffer;
	if (m_buffer.size() > 10240)
	{
		//Error here somehow;
		Q_ASSERT(m_buffer.size() < 10240);
	}
	unsigned char buffer[10240];
	for (int i=0;i<m_buffer.size();i++)
	{
		buffer[i] = m_buffer[i];
	}
	m_buffer.clear();
	bool inpacket = false;
	bool inescape = false;
	while (currms + timeout > QDateTime::currentMSecsSinceEpoch())
	{
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
				m_logFile->write(qbuffer);
				m_logFile->flush();
				//emit parseBuffer(qbuffer);
				m_queuedMessages.append(qbuffer);
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
		readlen = read(m_portHandle,buffer,1024);
		if (readlen == 0)
		{
			msleep(10);
		}
	}
}
void SerialThread::writePacket(QByteArray packet)
{
	write(m_portHandle,packet.data(),packet.length());
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
	m_logFile = new QFile(m_logFileName);
	m_logFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
	unsigned char buffer[1024];
	int readlen = 0;
	QByteArray qbuffer;
	bool inpacket= false;
	bool inescape=true;
	while (true)
	{
		readlen = read(m_portHandle,buffer,1024);
		if (readlen == 0)
		{
			msleep(10);
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
				m_logFile->write(qbuffer);
				m_logFile->flush();
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

int SerialThread::openPort(QString portName,int baudrate)
{
	m_portHandle = open(portName.toAscii(),O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_portHandle < 0)
	{
		//printf("Error opening Com: %s\n",portName);
		//debug(obdLib::DEBUG_ERROR,"Error opening com port %s",portName);

		return -1;
	}
	//printf("Com Port Opened %i\n",portHandle);
	//debug(obdLib::DEBUG_VERBOSE,"Com Port Opened %i",portHandle);
	fcntl(m_portHandle, F_SETFL, 0); //Set it to blocking. This is required? Wtf?
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
	newtio.c_cflag |= (PARENB | PARODD);
	newtio.c_cflag &= ~CRTSCTS;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_iflag=IGNBRK;
	//newtio.c_cflag |= (CLOCAL | CREAD | PARODD);
	/*newtio.c_lflag &= !(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_oflag &= !(OPOST);
	newtio.c_cc[VMIN] = 0; // Min number of bytes to read
	newtio.c_cc[VTIME] = 100; //10 second read timeout*/
	newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
	newtio.c_lflag=0;
	newtio.c_oflag=0;
	//newtio.c_cc[VTIME]=1;
	//newtio.c_cc[VMIN]=60;
	newtio.c_cc[VTIME]=2;
	newtio.c_cc[VMIN]=1;
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
	tcsetattr(m_portHandle,TCSANOW,&newtio);
	return 0;
}
