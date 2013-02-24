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

#include "serialport.h"
#include <QDateTime>
#include <QMutexLocker>
SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
	m_serialLockMutex = new QMutex();
	m_interByteSendDelay=0;
	m_inpacket = false;
	m_inescape = false;
	m_packetErrorCount=0;
}
void SerialPort::setPort(QString portname)
{
	m_portName = portname;
}
void SerialPort::setBaud(int baudrate)
{
	m_baud = baudrate;
}
SerialPortStatus SerialPort::verifyFreeEMS(QString portname)
{
	int errornum = openPort(portname,115200,false);
	if (errornum == -2)
	{
		return UNABLE_TO_LOCK;
	}
	else if (errornum < 0)
	{
		return UNABLE_TO_CONNECT;
	}
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
		return UNABLE_TO_WRITE;
	}
	unsigned char buf[3];
#ifdef Q_OS_WIN32
	Sleep(100);
#else
	usleep(100000);
#endif
	//msleep(100);
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
					return LOADER_MODE;
				}
				else
				{
					//Probably not;
					closePort();
					return NONE;
				}
			}
		}
	}
	//nothing on the port here either.
	closePort();
	return NONE;
}

void SerialPort::setInterByteSendDelay(int milliseconds)
{
	m_interByteSendDelay = milliseconds;
}
int SerialPort::writeBytes(unsigned char *buf,int len)
{
	Q_UNUSED(buf)
	Q_UNUSED(len)
	int lenret = 0;
#ifdef Q_OS_WIN32
	if (!::WriteFile(m_portHandle, (void*)buf, (DWORD)len, (LPDWORD)&lenret, NULL))
	{

	}
#else
	lenret = write(m_portHandle,buf,len);
#endif
	return lenret;
}
int SerialPort::readBlocking(unsigned char *buf,int maxlen)
{
	int readlen = 0;
#ifdef Q_OS_WIN32
	::ReadFile(m_portHandle,(LPVOID)buf,maxlen,(LPDWORD)&readlen,NULL);
#else
	readlen = read(m_portHandle,buf,maxlen);
#endif
	return readlen;
}

int SerialPort::readBytes(unsigned char *buf,int maxlen)
{
	int readlen=0;
	QMutexLocker locker(m_serialLockMutex);
#ifdef Q_OS_WIN32
	DWORD evt;
	if (!WaitCommEvent(m_portHandle,&evt,NULL))
	{
		qDebug() << "Unable to WaitCommEvent!!";
		return -1;
	}
	if (!(evt & EV_RXCHAR))
	{
		//No RX
		return 0;
	}
	if (!ReadFile(m_portHandle,(LPVOID)buf,maxlen,(LPDWORD)&readlen,NULL))
	{
		//Serial error here
		qDebug() << "Serial Read error";
	}
#else
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_portHandle,&set);
	timeval time;
	time.tv_sec = 0;
	time.tv_usec = 0;
	if (select(m_portHandle+1,&set,NULL,NULL,&time))
	{
		readlen = read(m_portHandle,buf,maxlen);
	}
	else
	{
		return 0;
	}

#endif //Q_OS_WIN32
	return readlen;
}

int SerialPort::writePacket(QByteArray packet)
{
	QMutexLocker locker(m_serialLockMutex);
#ifdef Q_OS_WIN32
	int len=0;
	if (m_interByteSendDelay > 0)
	{
		for (int i=0;i<packet.size();i++)
		{
			char c = packet.data()[i];
			if (!::WriteFile(m_portHandle, (void*)&c, (DWORD)1, (LPDWORD)&len, NULL))
			{
				qDebug() << "Serial Write Error";
				return -1;
			}
			Sleep(m_interByteSendDelay);
		}
	}
	else
	{
		if (!::WriteFile(m_portHandle, (void*)packet.data(), (DWORD)packet.length(), (LPDWORD)&len, NULL))
		{
			qDebug() << "Serial Write Error";
			return -1;
		}
	}
#else
	if (m_interByteSendDelay > 0)
	{
		for (int i=0;i<packet.size();i++)
		{
			char c = packet.data()[i];
			if (write(m_portHandle,&c,1)<0)
			{
				//TODO: Error here
				qDebug() << "Serial write error";
				return -1;
			}
			//msleep(m_interByteSendDelay);
			usleep(m_interByteSendDelay * 1000);
		}
	}
	else
	{
		if (write(m_portHandle,packet.data(),packet.size())<0)
		{
			qDebug() << "Serial write error";
			return -1;
		}
	}

#endif //Q_OS_WIN32
	emit dataWritten(packet);
	return 0;
}

void SerialPort::closePort()
{
#ifdef Q_OS_WIN32
	CloseHandle(m_portHandle);
#else
	flock(m_portHandle,LOCK_UN);
	close(m_portHandle);
#endif
}

void SerialPort::flush()
{
#ifdef Q_OS_WIN32
	PurgeComm(m_portHandle,PURGE_RXCLEAR | PURGE_TXCLEAR);
#else
	tcflush(m_portHandle, TCIOFLUSH); // clear buffer
#endif
}

int SerialPort::openPort(QString portName,int baudrate,bool oddparity)
{
#ifdef Q_OS_WIN32
	m_portHandle=CreateFileA(portName.toAscii(), GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_portHandle == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	COMMCONFIG Win_CommConfig;

	unsigned long confSize = sizeof(COMMCONFIG);
	Win_CommConfig.dwSize = confSize;
	GetCommConfig(m_portHandle, &Win_CommConfig, &confSize);
	if (oddparity)
	{
		qDebug() << "SerialPort Odd parity selected";
		Win_CommConfig.dcb.Parity = 1; //Odd parity
	}
	else
	{
		qDebug() << "SerialPort No parity selected";
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
	COMMTIMEOUTS Win_CommTimeouts;
	Win_CommTimeouts.ReadIntervalTimeout = MAXDWORD; //inter-byte timeout value (Disabled)
	Win_CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD; //Multiplier
	Win_CommTimeouts.ReadTotalTimeoutConstant = 1; //Total timeout, 1/10th of a second to match *nix
	Win_CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	Win_CommTimeouts.WriteTotalTimeoutConstant = 0;
	SetCommConfig(m_portHandle, &Win_CommConfig, sizeof(COMMCONFIG));
	SetCommTimeouts(m_portHandle,&Win_CommTimeouts);
	SetCommMask(m_portHandle,EV_RXCHAR);
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
	newtio.c_cflag |=  CS8;
	newtio.c_cflag |= CLOCAL | CREAD;
	if (oddparity)
	{
		newtio.c_cflag |= (PARENB | PARODD);
	}
	else
	{
		newtio.c_cflag &= ~(PARENB | CSTOPB  | CSIZE | PARODD);
	}
	newtio.c_cflag &= ~CRTSCTS;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_iflag=IGNBRK;
	newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
	newtio.c_lflag=0;
	newtio.c_oflag=0;
	newtio.c_cc[VTIME]=10; //1/10th second timeout, to allow for quitting the read thread
	newtio.c_cc[VMIN]=0; //We want a pure timer timeout
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
	tcflush(m_portHandle, TCIOFLUSH); // clear buffer
	return 0;
#endif //Q_OS_WIN32
}
