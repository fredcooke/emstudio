#ifndef SERIALRXTHREAD_H
#define SERIALRXTHREAD_H

#include <QThread>

class SerialRXThread : public QThread
{
	Q_OBJECT
public:
	explicit SerialRXThread(QObject *parent = 0);	
	~SerialRXThread();
	void start(int handle);
private:
	bool m_terminate;
	int m_portHandle;
protected:
	void run();
signals:
	void incomingPacket(QByteArray packet);
	void dataWritten(QByteArray data);
	void dataRead(QByteArray data);
public slots:
	
};

#endif // SERIALRXTHREAD_H
