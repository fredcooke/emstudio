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

#include "serialrxthread.h"
#include <QDateTime>
#include <QDebug>
SerialRXThread::SerialRXThread(QObject *parent) : QThread(parent)
{
	m_terminate = false;
}
void SerialRXThread::start(SerialPort *serialport)
{
	m_serialPort = serialport;
	QThread::start();
}
SerialRXThread::~SerialRXThread()
{
	m_terminate = true;
	this->terminate();
	this->wait(1000);
}
QByteArray SerialRXThread::readSinglePacket(SerialPort *port)
{

	unsigned char buf;
	int len = 0;
	bool m_inpacket = false;
	bool m_inescape = false;
	QByteArray qbuffer;
	int attempts = 0; //This allows for a 2 second timeout.
	while (attempts <= 50)
	{
		len = port->readBytes(&buf,1);
		if (len < 0)
		{
			qDebug() << "Timeout";
		}
		else if (len == 0)
		{
			//This should be an error
			qDebug() << "Nothing to read:" << attempts;
			//perror("Error:");
			//printf("\n");
			//return; //Disable this, now that we are using timeouts.
			msleep(10); //Need a sleep now, due to select timeout.
			attempts++;
			continue;
		}
		else
		{
			if (buf == 0xAA)
			{
				if (m_inpacket)
				{
					//Start byte in the middle of a packet
					//Clear out the buffer and start fresh
					m_inescape = false;
					//emit dataRead(bufToEmit);
					qbuffer.clear();
					qDebug() << "Buffer error";
				}
				//Start of packet
				m_inpacket = true;
			}
			else if (buf == 0xCC && m_inpacket)
			{
				//End of packet
				m_inpacket = false;

				//New Location of checksum
				unsigned char sum = 0;
				for (int i=0;i<qbuffer.size()-1;i++)
				{
					sum += qbuffer[i];
				}

				if (sum != (unsigned char)qbuffer[qbuffer.size()-1])
				{
					qDebug() << "BAD CHECKSUM!";
					qbuffer.clear();
				}
				else
				{
					return qbuffer.mid(0,qbuffer.length()-1);
				}
			}
			else
			{
				if (m_inpacket && !m_inescape)
				{
					if (buf == 0xBB)
					{
						//Need to escape the next byte
						m_inescape = true;
					}
					else
					{
						qbuffer.append(buf);
					}

				}
				else if (m_inpacket && m_inescape)
				{
					if (buf == 0x55)
					{
						qbuffer.append((char)0xAA);
					}
					else if (buf == 0x44)
					{
						qbuffer.append((char)0xBB);
					}
					else if (buf == 0x33)
					{
						qbuffer.append((char)0xCC);
					}
					else
					{
						qDebug() << "Error, escaped character is not valid!:" << QString::number(buf,16);
					}
					m_inescape = false;
				}
				else
				{
					//qDebug() << "Byte out of a packet:" << QString::number(buffer[i],16);
				}
			}
		}
	}
	return QByteArray();
}

void SerialRXThread::run()
{
	//qint64 currms = QDateTime::currentMSecsSinceEpoch();
	QString byteoutofpacket;
	QByteArray qbuffer;
	unsigned char buffer[10240];
	bool m_inpacket = false;
	bool m_inescape = false;
	int m_packetErrorCount=0;
	int readlen=0;
	while (!m_terminate)
	{
		readlen = m_serialPort->readBytes(buffer,1024);
		if (readlen < 0)
		{
			//Nothing on the port
			qDebug() << "Timeout";
			//msleep(10);
		}
		else
		{
			//emit dataRead(QByteArray((const char*)buffer,readlen));
			/*if (m_logsEnabled)
			{
				m_logInFile->write((const char*)buffer,readlen);
				m_logInFile->flush();
			}*/
			//emit dataRead(QByteArray((const char*)buffer,readlen));
		}
		if (readlen == 0)
		{
			//This should be an error
			//qDebug() << "Nothing to read";
			//perror("Error:");
			//printf("\n");
			//return; //Disable this, now that we are using timeouts.
			msleep(10); //Need a sleep now, due to select timeout.
			continue;
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
					QByteArray bufToEmit;
					bufToEmit.append(0xAA);
					QByteArray nbuffer(qbuffer);
					nbuffer.replace(0xBB,QByteArray().append(0xBB).append(0x44));
					nbuffer.replace(0xAA,QByteArray().append(0xBB).append(0x55));
					nbuffer.replace(0xCC,QByteArray().append(0xBB).append(0x33));
					bufToEmit.append(nbuffer);
					bufToEmit.append(0xCC);
					emit dataRead(bufToEmit);
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
				QByteArray bufToEmit;
				bufToEmit.append(0xAA);
				QByteArray nbuffer(qbuffer);
				nbuffer.replace(0xBB,QByteArray().append(0xBB).append(0x44));
				nbuffer.replace(0xAA,QByteArray().append(0xBB).append(0x55));
				nbuffer.replace(0xCC,QByteArray().append(0xBB).append(0x33));
				bufToEmit.append(nbuffer);
				bufToEmit.append(0xCC);
				emit dataRead(bufToEmit);
				/*if (m_logsEnabled)
				{
					m_logWriteMutex->lock();
					m_logInOutFile->write(QByteArray().append(0xAA));
					QByteArray nbuffer(qbuffer);
					nbuffer.replace(0xBB,QByteArray().append(0xBB).append(0x44));
					nbuffer.replace(0xAA,QByteArray().append(0xBB).append(0x55));
					nbuffer.replace(0xCC,QByteArray().append(0xBB).append(0x33));
					m_logInOutFile->write((const char*)nbuffer.data(),nbuffer.size());
					m_logInOutFile->write(QByteArray().append(0xCC));
					m_logInOutFile->flush();
					m_logWriteMutex->unlock();
				}*/
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
					//m_queuedMessages.append(qbuffer.mid(0,qbuffer.length()-1));
					emit incomingPacket(qbuffer.mid(0,qbuffer.length()-1));
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
	//m_buffer.append(qbuffer);
	if (readlen > 0)
	{
	//m_buffer.write(buffer,readlen);
	//	m_buffer.append((const char*)buffer,readlen);
	}
	//return m_packetErrorCount;
	return;
}
