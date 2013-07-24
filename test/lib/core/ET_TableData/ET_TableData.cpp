#include <QString>
#include <QtTest>
#include "tabledata.h"

class ET_TableData : public QObject
{
    Q_OBJECT
    
public:
    ET_TableData();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_TableData::ET_TableData()
{
}

void ET_TableData::initTestCase()
{
}

void ET_TableData::cleanupTestCase()
{
}

void ET_TableData::default_ctorNoThrow()
{
    TableData *const d = new TableData();
    QVERIFY( d );
    delete d;
}

QTEST_APPLESS_MAIN(ET_TableData)

#include "ET_TableData.moc"
