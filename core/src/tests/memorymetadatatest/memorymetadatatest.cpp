 #include <QtTest/QtTest>
#include "../../memorymetadata.h"
class MemoryMetaDataTest : public QObject
{
	Q_OBJECT
private:
	MemoryMetaData *metadata;
private slots:
	void initTestCase();
	void testLoadFile();
	void testErrorsLoaded();
	void cleanupTestCase();
};
void MemoryMetaDataTest::testErrorsLoaded()
{
	QCOMPARE(metadata->errorMap().size() == 0,false);
}
void MemoryMetaDataTest::cleanupTestCase()
{
	delete metadata;
}

void MemoryMetaDataTest::initTestCase()
{
	metadata=0;
	metadata = new MemoryMetaData();
	QCOMPARE(metadata == 0,false);
}

void MemoryMetaDataTest::testLoadFile()
{
	bool ok = metadata->loadMetaDataFromFile("");
	QCOMPARE(ok,false);
	ok = metadata->loadMetaDataFromFile("../../../freeems.config.json");
	QCOMPARE(ok,true);

}
QTEST_MAIN(MemoryMetaDataTest)
#include "memorymetadatatest.moc"

