#ifndef O5EMEMORYMETADATA_H
#define O5EMEMORYMETADATA_H

#include "memorymetadata.h"
class O5EMemoryMetaData : public MemoryMetaData
{
public:
	O5EMemoryMetaData();
	bool loadMetaDataFromFile(QString filestr);
	bool parseMetaData(QString json);
	const QMap<unsigned short,QString> errorMap() { return m_errorMap; }


	bool has2DMetaData(unsigned short locationid);
	const QList<Table2DMetaData> table2DMetaData() { return m_table2DMetaData; }
	const Table2DMetaData get2DMetaData(unsigned short locationid);

	bool has3DMetaData(unsigned short locationid);
	const QList<Table3DMetaData> table3DMetaData() { return m_table3DMetaData; }
	const Table3DMetaData get3DMetaData(unsigned short locationid);

	bool hasRORMetaData(unsigned short locationid);
	const QList<ReadOnlyRamData> rORMetaData() { return m_readOnlyMetaData; }
	const ReadOnlyRamData getRORMetaData(unsigned short locationid);

	bool hasLookupMetaData(unsigned short locationid);
	const QMap<unsigned short,LookupMetaData> lookupMetaData() { return m_lookupMetaData; }
	const LookupMetaData getLookupMetaData(unsigned short locationid);

	const QString getErrorString(unsigned short code);

private:

	QMap<unsigned short,QString> m_errorMap;
	QMap<unsigned short,LookupMetaData> m_lookupMetaData;
	QList<ReadOnlyRamData> m_readOnlyMetaData;
	QMap<unsigned short,ReadOnlyRamBlock> m_readOnlyMetaDataMap;
	QList<Table3DMetaData> m_table3DMetaData;
	QList<Table2DMetaData> m_table2DMetaData;

};

#endif // O5EMEMORYMETADATA_H
