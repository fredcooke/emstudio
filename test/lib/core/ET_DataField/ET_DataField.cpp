#include <QString>
#include <QtTest>
#include "datafield.h"

class ET_DataField : public QObject
{
    Q_OBJECT
    
public:
    ET_DataField();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_DataField::ET_DataField()
{
}

void ET_DataField::initTestCase()
{
}

void ET_DataField::cleanupTestCase()
{
}

void ET_DataField::default_ctorNoThrow()
{
    DataField *const d = new DataField();
    QVERIFY( d );
    delete d;
}

QTEST_APPLESS_MAIN(ET_DataField)

#include "ET_DataField.moc"
