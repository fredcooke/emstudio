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

#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <qglobal.h>
#ifdef Q_OS_WIN32
#include <windows.h>
#else
#define HANDLE int
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <vector>
class SerialPort : public QObject
{
	Q_OBJECT
public:
    SerialPort(QObject *parent=0);
    bool verifyFreeEMS(QString portname);
    void setPort(QString portname);
    void setBaud(int baudrate);
    int openPort(QString portName,int baudrate,bool oddparity = true);
    //HANDLE portHandle() { return m_portHandle; }
    void closePort();
    int writePacket(QByteArray packet);
    int writeBytes(unsigned char *buf,int len);
    int readBytes(unsigned char *buf,int maxlen);
    //void sendMessageForResponse(QByteArray header,QByteArray payload);
    int bufferSize() { return m_queuedMessages.size(); }
    void setInterByteSendDelay(int milliseconds);
private:
    QMutex *m_serialLockMutex;
    void openLogs();
    unsigned int m_packetErrorCount;
    bool m_logsEnabled;
    QString m_logDirectory;
    bool m_inpacket;
    bool m_inescape;
    int m_interByteSendDelay;
    QList<QByteArray> m_queuedMessages;
    QByteArray m_buffer;
    //QFile *m_logInFile;
    //QFile *m_logOutFile;
    //QFile *m_logInOutFile;
    QString m_logFileName;
    QString m_portName;
    int m_baud;
    HANDLE m_portHandle;


protected:
    void run();
signals:
    void parseBuffer(QByteArray buffer);
    void dataWritten(QByteArray data);
};

#endif // SERIALPORT_H
