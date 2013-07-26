#include <QString>
#include <QtTest>
#include "serialport.h"

class ET_SerialPort : public QObject
{
    Q_OBJECT
    
public:
    ET_SerialPort();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_SerialPort::ET_SerialPort()
{
}

void ET_SerialPort::initTestCase()
{
}

void ET_SerialPort::cleanupTestCase()
{
}

void ET_SerialPort::default_ctorNoThrow()
{
    SerialPort *const sp = new SerialPort();
    QVERIFY( sp );
    delete sp;
}

QTEST_APPLESS_MAIN(ET_SerialPort)

#include "ET_SerialPort.moc"
