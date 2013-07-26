#include <QString>
#include <QtTest>
#include "fetable3ddata.h"

class ET_FETable3DData : public QObject
{
    Q_OBJECT
    
public:
    ET_FETable3DData();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_FETable3DData::ET_FETable3DData()
{
}

void ET_FETable3DData::initTestCase()
{
}

void ET_FETable3DData::cleanupTestCase()
{
}

void ET_FETable3DData::default_ctorNoThrow()
{
    FETable3DData *const t3d = new FETable3DData();
    QVERIFY( t3d );
    delete t3d;
}

QTEST_APPLESS_MAIN(ET_FETable3DData)

#include "ET_FETable3DData.moc"
