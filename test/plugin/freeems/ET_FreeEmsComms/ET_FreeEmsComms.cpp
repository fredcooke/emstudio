#include <QString>
#include <QtTest>
#include "freeemscomms.h"

class ET_FreeEmsComms : public QObject
{
    Q_OBJECT
    
public:
    ET_FreeEmsComms();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_FreeEmsComms::ET_FreeEmsComms()
{
}

void ET_FreeEmsComms::initTestCase()
{
}

void ET_FreeEmsComms::cleanupTestCase()
{
}

void ET_FreeEmsComms::default_ctorNoThrow()
{
    FreeEmsComms *const fc = new FreeEmsComms();
    QVERIFY( fc );
    delete fc;
}

QTEST_APPLESS_MAIN(ET_FreeEmsComms)

#include "ET_FreeEmsComms.moc"
