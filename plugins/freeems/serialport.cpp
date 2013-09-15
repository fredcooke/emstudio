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

#include "serialport.h"
#include <QDateTime>
#include <QMutexLocker>
#include <cassert>

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

SerialPortStatus SerialPort::isSerialMonitor(QString portname)
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

	int const writei = writeBytes( QByteArray("\x0d") );
	if (writei < 0)
	{
		qDebug() << "Error writing to verify FreeEMS";
	closePort();
		return UNABLE_TO_WRITE;
	}

#ifdef Q_OS_WIN
	Sleep(100);
#else
	usleep(100000);
#endif

	unsigned char buf[3];
	int const count = readBytes( buf, sizeof(buf) );

	assert( count >= 0 );               /**< fatally catch read errs (temp) */
	SerialPortStatus status = NONE;     /**< default: assume !SM            */
	qDebug() << "Verify count: " << QString::number( count );
	if (count >= 2)
	{
		qDebug() << "Verify:" << QString::number(buf[0],16);
		qDebug() << "Verify:" << QString::number(buf[1],16);
		if ( (count >= 3) && (buf[0] == 0xE0 || buf[0] == 0xE1) )
		{
			qDebug() << "Verify:" << QString::number(buf[2],16);
			if (buf[2] == 0x3E)
				status = SM_MODE;   // Serial monitor IS running
		}
	}
	closePort();
	return status;
}

void SerialPort::setInterByteSendDelay(int milliseconds)
{
	m_interByteSendDelay = milliseconds;
}

int SerialPort::readBytes(unsigned char *buf,int maxlen)
{
	int readlen=0;
	QMutexLocker locker(m_serialLockMutex);	
#ifdef Q_OS_WIN
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

#endif //Q_OS_WIN
	return readlen;
}

int SerialPort::writeBytes(QByteArray packet)
{
	QMutexLocker locker(m_serialLockMutex);
#ifdef Q_OS_WIN
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

#endif //Q_OS_WIN
	emit dataWritten(packet);
	return 0;
}

void SerialPort::closePort()
{
#ifdef Q_OS_WIN
	CloseHandle(m_portHandle);
#else
	flock(m_portHandle,LOCK_UN);
	close(m_portHandle);
#endif
}

int SerialPort::openPort(QString portName,int baudrate,bool oddparity)
{
#ifdef Q_OS_WIN
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
	struct termios newtio;
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
	newtio.c_cc[VMIN]=0; //We want a pure timer timeout
	if (baudrate != -1)
	{
		if(cfsetispeed(&newtio, BAUD))
		{
		}

		if(cfsetospeed(&newtio, BAUD))
		{
		}
	}
	fcntl(m_portHandle, F_SETFL, 0); //Set to blocking
	tcsetattr(m_portHandle,TCSANOW,&newtio);
	return 0;
#endif //Q_OS_WIN
}
