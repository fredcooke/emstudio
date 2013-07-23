#include <QString>
#include <QtTest>
#include "fedatapacketdecoder.h"

class ET_FEDataPacketDecoder : public QObject
{
    Q_OBJECT
    
public:
    ET_FEDataPacketDecoder();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void default_ctorNoThrow();
};

ET_FEDataPacketDecoder::ET_FEDataPacketDecoder()
{
}

void ET_FEDataPacketDecoder::initTestCase()
{
}

void ET_FEDataPacketDecoder::cleanupTestCase()
{
}

void ET_FEDataPacketDecoder::default_ctorNoThrow()
{
    FEDataPacketDecoder dpd;
}

QTEST_APPLESS_MAIN(ET_FEDataPacketDecoder)

#include "ET_FEDataPacketDecoder.moc"
