#ifndef EMSDATA_H
#define EMSDATA_H
#include <QMap>
#include <QList>
#include "configblock.h"
#include "headers.h"
#include "memorylocation.h"
class EmsData
{
public:
	EmsData();
	bool hasLocalRamBlock(unsigned short id);
	void setLocalRamBlock(unsigned short id,QByteArray data);
	QByteArray getLocalRamBlock(unsigned short id);
	void addLocalRamBlock(MemoryLocation *loc);

	bool hasDeviceRamBlock(unsigned short id);
	void setDeviceRamBlock(unsigned short id,QByteArray data);
	QByteArray getDeviceFlashBlock(unsigned short id);
	void addDeviceFlashBlock(MemoryLocation *loc);

	bool hasLocalFlashBlock(unsigned short id);
	void setLocalFlashBlock(unsigned short id,QByteArray data);
	QByteArray getLocalFlashBlock(unsigned short id);
	void addLocalFlashBlock(MemoryLocation *loc);

	bool hasDeviceFlashBlock(unsigned short id);
	void setDeviceFlashBlock(unsigned short id,QByteArray data);
	QByteArray getDeviceRamBlock(unsigned short id);
	void addDeviceRamBlock(MemoryLocation *loc);

	QList<unsigned short> getChildrenOfLocalRamLocation(unsigned short id);
	QList<unsigned short> getParentOfLocalRamLocation(unsigned short id);
	bool localRamHasParent(unsigned short id);
	bool localRamHasChildren(unsigned short id);

	QString serialize(unsigned short id);

	void populateLocalRamAndFlash();
	void populateDeviceRamAndFlashParents();
	QList<unsigned short> getTopLevelDeviceFlashLocations();
	QList<unsigned short> getTopLevelDeviceRamLocations();

	void passLocationInfo(unsigned short locationid,MemoryLocationInfo info);

	void clearAllMemory();

private:
	QMap<unsigned short,QList<ConfigBlock> > m_configBlockMap;
	QMap<unsigned short,ReadOnlyRamBlock> m_readOnlyMetaDataMap;
	QList<MemoryLocation*> m_ramMemoryList;
	QList<MemoryLocation*> m_flashMemoryList;
	QList<MemoryLocation*> m_deviceFlashMemoryList;
	QList<MemoryLocation*> m_deviceRamMemoryList;
	QList<MemoryLocation*> m_tempMemoryList;
	QList<Table3DMetaData> m_table3DMetaData;
	QList<Table2DMetaData> m_table2DMetaData;
	QList<ConfigData> m_configMetaData;
	QList<ReadOnlyRamData> m_readOnlyMetaData;
	QMap<unsigned short,QString> m_errorMap;
};

#endif // EMSDATA_H
