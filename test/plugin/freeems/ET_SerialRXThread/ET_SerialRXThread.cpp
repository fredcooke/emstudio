#include <QString>
#include <QtTest>
#include "serialrxthread.h"

class ET_SerialRXThread : public QObject
{
    Q_OBJECT
    
public:
    ET_SerialRXThread();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_SerialRXThread::ET_SerialRXThread()
{
}

void ET_SerialRXThread::initTestCase()
{
}

void ET_SerialRXThread::cleanupTestCase()
{
}

void ET_SerialRXThread::default_ctorNoThrow()
{
    SerialRXThread *const t = new SerialRXThread();
    QVERIFY( t );
    delete t;
}

QTEST_APPLESS_MAIN(ET_SerialRXThread)

#include "ET_SerialRXThread.moc"
