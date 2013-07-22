#include <QString>
#include <QtTest>
#include "core.h"

class ET_Core : public QObject
{
    Q_OBJECT
    
public:
    ET_Core();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_falseMethodIsFalse();
    void default_trueMethodIsTrue();
};

ET_Core::ET_Core()
{
}

void ET_Core::initTestCase()
{
}

void ET_Core::cleanupTestCase()
{
}

void ET_Core::default_falseMethodIsFalse()
{
    Core c;
    QVERIFY( c.falseMethod() == false );
}

void ET_Core::default_trueMethodIsTrue()
{
    Core c;
    QVERIFY( c.trueMethod() == true );
}

QTEST_APPLESS_MAIN(ET_Core)

#include "ET_Core.moc"
