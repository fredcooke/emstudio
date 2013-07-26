#include <QString>
#include <QtTest>
#include "fememorymetadata.h"

class ET_FEMemoryMetaData : public QObject
{
    Q_OBJECT
    
public:
    ET_FEMemoryMetaData();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_FEMemoryMetaData::ET_FEMemoryMetaData()
{
}

void ET_FEMemoryMetaData::initTestCase()
{
}

void ET_FEMemoryMetaData::cleanupTestCase()
{
}

void ET_FEMemoryMetaData::default_ctorNoThrow()
{
    FEMemoryMetaData *const mmd = new FEMemoryMetaData();
    QVERIFY( mmd );
    delete mmd;
}

QTEST_APPLESS_MAIN(ET_FEMemoryMetaData)

#include "ET_FEMemoryMetaData.moc"
