/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QThread>
#include <QFile>
#include <QDebug>
#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#endif
#include <vector>
class SerialThread : public QThread
{
	Q_OBJECT
public:
    SerialThread(QObject *parent=0);
    void setLogFileName(QString filename);
    void setPort(QString portname);
    void setBaud(int baudrate);
    int openPort(QString portName,int baudrate);
    QByteArray readPacket();
    void readSerial(int timeout);
    void writePacket(QByteArray packet);
    //void sendMessageForResponse(QByteArray header,QByteArray payload);
    int bufferSize() { return m_queuedMessages.size(); }
    void setInterByteSendDelay(int milliseconds);
private:
    int m_interByteSendDelay;
    QList<QByteArray> m_queuedMessages;
    QByteArray m_buffer;
    QFile *m_logInFile;
    QFile *m_logInOutFile;
    QString m_logFileName;
    QString m_portName;
    int m_baud;
    int m_portHandle;


protected:
    void run();
signals:
    void parseBuffer(QByteArray buffer);
};

#endif // SERIALTHREAD_H
