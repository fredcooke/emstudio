 #include <QtTest/QtTest>
#include "../../memorylocation.h"
class MemoryLocationTest : public QObject
{
	Q_OBJECT
private:
private slots:
	void initTestCase();
	void testParentEdit();
	void cleanupTestCase();
};
void MemoryLocationTest::initTestCase()
{

}
void MemoryLocationTest::testParentEdit()
{
	MemoryLocation *child = new MemoryLocation();
	child->isRam = true;
	child->ramAddress = 0x2;
	child->size = 0x2;
	MemoryLocation *parent = new MemoryLocation();
	parent->isRam = true;
	parent->ramAddress = 0x0;
	parent->size = 0x5;
	child->setParent(parent);
	parent->addChild(child);
	QByteArray parentdata;
	parentdata.append(0x01);
	parentdata.append(0x02);
	parentdata.append(0x03);
	parentdata.append(0x04);
	parentdata.append(0x05);

	QCOMPARE(child->data().size(),2);

	parent->setData(parentdata);

	QCOMPARE(child->data(),QByteArray().append(0x03).append(0x04));

	child->setData(QByteArray().append(0x01).append(0x02));

	QCOMPARE(parent->data(),QByteArray().append(0x01).append(0x02).append(0x01).append(0x02).append(0x05));
}

void MemoryLocationTest::cleanupTestCase()
{
}


QTEST_MAIN(MemoryLocationTest)
#include "memorylocationtest.moc"

