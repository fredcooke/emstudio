#include "o5ememorymetadata.h"

O5EMemoryMetaData::O5EMemoryMetaData()
{
}

bool O5EMemoryMetaData::loadMetaDataFromFile(QString filestr)
{
	return true;
}

bool O5EMemoryMetaData::parseMetaData(QString json)
{
	return true;
}

bool O5EMemoryMetaData::has2DMetaData(unsigned short locationid)
{
	return false;
}

const Table2DMetaData O5EMemoryMetaData::get2DMetaData(unsigned short locationid)
{
	return Table2DMetaData();
}

bool O5EMemoryMetaData::has3DMetaData(unsigned short locationid)
{
	return false;
}

const Table3DMetaData O5EMemoryMetaData::get3DMetaData(unsigned short locationid)
{
	return Table3DMetaData();
}

bool O5EMemoryMetaData::hasRORMetaData(unsigned short locationid)
{
	return false;
}

const ReadOnlyRamData O5EMemoryMetaData::getRORMetaData(unsigned short locationid)
{
	return ReadOnlyRamData();
}

bool O5EMemoryMetaData::hasLookupMetaData(unsigned short locationid)
{
	return false;
}

const LookupMetaData O5EMemoryMetaData::getLookupMetaData(unsigned short locationid)
{
	return LookupMetaData();
}

const QString O5EMemoryMetaData::getErrorString(unsigned short code)
{
	return "";
}
