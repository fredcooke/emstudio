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
#include "QsLog.h"

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
	m_serialLockMutex = new QMutex();
	m_interByteSendDelay=0;
	m_inpacket = false;
	m_inescape = false;
	m_packetErrorCount=0;
	m_serialPort = new QSerialPort();
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
	if (openPort(portname,115200,false) < 0)
	{
		return NONE;
	}
	int retry = 0;

	while (retry++ <= 3)
	{
		//m_port->clear();
		//m_port->flush();
		//m_privBuffer.clear();
		writeBytes(QByteArray().append(0x0D));
		QByteArray verifybuf;
		int verifylen = readBytes(&verifybuf,3,1000);
		qDebug() << "Verify len:" << verifylen;
		if ((unsigned char)verifybuf[0] == 0xE0)
		{
			if ((unsigned char)verifybuf[2] == 0x3E)
			{
				qDebug() << "In SM mode";
				closePort();
				return SM_MODE;
			}
		}
		else if ((unsigned char)verifybuf[0] == 0xE1)
		{
			if ((unsigned char)verifybuf[2] == 0x3E)
			{
				qDebug() << "In SM mode two";
				closePort();
				return SM_MODE;
			}
		}
		else
		{
			//Likely not in SM mode
			//qDebug() << "Bad return:" << QString::number((unsigned char)verifybuf[0],16) << QString::number((unsigned char)verifybuf[2],16);
		}
	}
	//Timed out.
	closePort();
	return NONE;
}

void SerialPort::setInterByteSendDelay(int milliseconds)
{
	m_interByteSendDelay = milliseconds;
}

int SerialPort::readBytes(QByteArray *array, int maxlen,int timeout)
{
	if (m_privBuffer.size() >= maxlen)
	{
		*array = m_privBuffer.mid(0,maxlen);
		m_privBuffer.remove(0,maxlen);
		return maxlen;
	}
	while (m_serialPort->waitForReadyRead(timeout))
	{
		m_privBuffer.append(m_serialPort->readAll());
		if (m_privBuffer.size() >= maxlen)
		{
			*array = m_privBuffer.mid(0,maxlen);
			m_privBuffer.remove(0,maxlen);
			return maxlen;
		}
	}
	return 0;
}

int SerialPort::writeBytes(QByteArray packet)
{
	QMutexLocker locker(m_serialLockMutex);
	if (m_interByteSendDelay > 0)
	{
		for (int i=0;i<packet.size();i++)
		{
			char c = packet.data()[i];

			//if (write(m_portHandle,&c,1)<0)
			if (m_serialPort->write(QByteArray().append(packet.at(i))) == -1)
			{
				//TODO: Error here
				QLOG_ERROR() << "Serial write error" << m_serialPort->errorString();
				return -1;
			}
			m_serialPort->waitForBytesWritten(1); //Verify.
			//usleep(m_interByteSendDelay * 1000);
		}

	}
	else
	{
		if (m_serialPort->write(packet) == -1)
		{
			QLOG_ERROR() << "Serial write error" << m_serialPort->errorString();
			return -1;
		}
		m_serialPort->waitForBytesWritten(1); //Verify.
	}

}

void SerialPort::closePort()
{
	QLOG_DEBUG() << "SerialPort::closePort thread:" << QThread::currentThread();
	m_serialPort->close();
	//delete m_serialPort;
	m_privBuffer.clear();
}

int SerialPort::openPort(QString portName,int baudrate,bool oddparity)
{
	//QLOG_DEBUG() << "SerialPort::openPort thread:" << QThread::currentThread();
	//m_serialPort = new QSerialPort();
	m_serialPort->setPortName(portName);
	if (!m_serialPort->open(QIODevice::ReadWrite))
	{
		return -1;
	}
	m_serialPort->setBaudRate(baudrate);
	if (oddparity)
	{
		m_serialPort->setParity(QSerialPort::OddParity);
	}
	else
	{
		m_serialPort->setParity(QSerialPort::NoParity);
	}
	return 0;
}
