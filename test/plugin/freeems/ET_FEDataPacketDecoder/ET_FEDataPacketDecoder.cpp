#include <QString>
#include <QtTest>
#include <QSignalSpy>
#include "fedatapacketdecoder.h"

class ET_FEDataPacketDecoder : public QObject
{
    Q_OBJECT
public:
    ET_FEDataPacketDecoder() : d(0) {}

private:
    FEDataPacketDecoder *d;

private Q_SLOTS:
    void init();
    void cleanup();
    void default_hasFieldSize();
    void default_allowsNegFieldIndex();
    void default_getAField();
    void default_allowsMultiplePopulates();
    void default_badPayloadDecoded();
};

void ET_FEDataPacketDecoder::init()
{
    QVERIFY( !d );
    d = new FEDataPacketDecoder();
    QVERIFY( d );
}

void ET_FEDataPacketDecoder::cleanup()
{
    QVERIFY( d );
    delete d;
    d = 0;
}

void ET_FEDataPacketDecoder::default_hasFieldSize()
{
    /* a default decoder does have some fields associated
     * with it, the value is insignificant as long as it's
     * not zero
     */
    QVERIFY( d->fieldSize() > 0 );
}

void ET_FEDataPacketDecoder::default_allowsNegFieldIndex()
{
    /* I'm assuming that if it doesn't throw, then a
     * negative index is accepted. There is no other
     * way of telling.
     */
    DataField f = d->getField(-1);
    /* not much more I can do here */
}

void ET_FEDataPacketDecoder::default_getAField()
{
    /* since a default decoder does have fields then I
     * can safely assume that the first one must exist
     * I have no idea if it's valid, but I can guarantee
     * That I can retrieve it
     */
    DataField f = d->getField(0);
    /* not much more I can do here */
}

void ET_FEDataPacketDecoder::default_allowsMultiplePopulates()
{
    /* this is a nasty one - I know the behavior is undesired, but
     * it exists. let this test serve as a catch for when we do
     * correct it in the future.
     */
    int orig_size = d->fieldSize();
    d->populateDataFields();
    int new_size = d->fieldSize();
    QVERIFY( new_size == (2*orig_size) );
}

void ET_FEDataPacketDecoder::default_badPayloadDecoded()
{
    /* when supplied w/data an emit will always occur */
    QByteArray const p( "junk" );
    QSignalSpy s( d, SIGNAL(payloadDecoded(QVariantMap)) );
    d->decodePayload( p );
    QVERIFY( s.count()==1 );
}

QTEST_APPLESS_MAIN(ET_FEDataPacketDecoder)

#include "ET_FEDataPacketDecoder.moc"
