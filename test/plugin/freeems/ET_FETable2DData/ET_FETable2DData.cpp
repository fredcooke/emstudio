#include <QString>
#include <QtTest>
#include "fetable2ddata.h"

class ET_FETable2DData : public QObject
{
    Q_OBJECT
    
public:
    ET_FETable2DData();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_FETable2DData::ET_FETable2DData()
{
}

void ET_FETable2DData::initTestCase()
{
}

void ET_FETable2DData::cleanupTestCase()
{
}

void ET_FETable2DData::default_ctorNoThrow()
{
    FETable2DData *const t2d = new FETable2DData();
    QVERIFY( t2d );
    delete t2d;
}

QTEST_APPLESS_MAIN(ET_FETable2DData)

#include "ET_FETable2DData.moc"
