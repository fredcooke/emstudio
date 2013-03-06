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

#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <qglobal.h>
#include "headers.h"
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
    void setPort(QString portname);
    void setBaud(int baudrate);
    int openPort(QString portName,int baudrate,bool oddparity = true);
    void flush();
    //HANDLE portHandle() { return m_portHandle; }
    void closePort();
    int writePacket(QByteArray packet);
    int writeBytes(unsigned char *buf,int len);
    int readBytes(unsigned char *buf,int maxlen);
    int readBlocking(unsigned char *buf,int maxlen);
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
