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

#ifndef SERIALRXTHREAD_H
#define SERIALRXTHREAD_H

#include <QThread>
#include <QMutex>
#include "serialport.h"
#ifndef Q_OS_WIN
#define HANDLE int
#endif
#ifdef Q_OS_WIN
#include <windows.h>
#endif
class SerialRXThread : public QThread
{
	Q_OBJECT
public:
	explicit SerialRXThread(QObject *parent = 0);
	QByteArray readSinglePacket(SerialPort *port);
	~SerialRXThread();
	void start(SerialPort *serialport);
	void stop() { m_terminate = true; }
private:
	bool m_terminate;
	SerialPort *m_serialPort;
protected:
	void run();
signals:
	void incomingPacket(QByteArray packet);
	void dataRead(QByteArray data);
public slots:
	
};

#endif // SERIALRXTHREAD_H
