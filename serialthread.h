#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QThread>
#include <QFile>
#include <QDebug>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <vector>

class SerialThread : public QThread
{
	Q_OBJECT
public:
    SerialThread(QObject *parent=0);
    void setLogFileName(QString filename);
    void setPort(QString portname);
    void setBaud(int baudrate);
private:
    QFile *m_logFile;
    QString m_logFileName;
    QString m_portName;
    int m_baud;
    int m_portHandle;
    int openPort(QString portName,int baudrate);

protected:
    void run();
signals:
    void parseBuffer(QByteArray buffer);
};

#endif // SERIALTHREAD_H
