#ifndef SERIALRXTHREAD_H
#define SERIALRXTHREAD_H

#include <QThread>
#include <QMutex>
#include "serialport.h"
#ifndef Q_OS_WIN32
#define HANDLE int
#endif
#ifdef Q_OS_WIN32
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
