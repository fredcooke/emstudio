/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

#ifndef EMSDATA_H
#define EMSDATA_H
#include <QMap>
#include <QList>
#include <QVariant>

#include "configblock.h"
#include "memorylocation.h"
#include "memorymetadata.h"
#include "memorylocationinfo.h"
#include "readonlyramblock.h"
#include "configdata.h"

class EmsData : public QObject
{
	Q_OBJECT
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
	unsigned short getParentOfLocalRamLocation(unsigned short id);
	bool localRamHasParent(unsigned short id);
	bool localRamHasChildren(unsigned short id);

	QList<unsigned short> getChildrenOfLocalFlashLocation(unsigned short id);
	unsigned short getParentOfLocalFlashLocation(unsigned short id);
	bool localFlashHasParent(unsigned short id);
	bool localFlashHasChildren(unsigned short id);

	QString serialize(unsigned short id,bool isram);

	void populateLocalRamAndFlash();
	void populateDeviceRamAndFlashParents();
	QList<unsigned short> getTopLevelDeviceFlashLocations();
	QList<unsigned short> getTopLevelDeviceRamLocations();


	QList<unsigned short> getDuplicateTopLevelDeviceFlashLocations();
	QList<unsigned short> getDuplicateTopLevelDeviceRamLocations();

	void passLocationInfo(unsigned short locationid,MemoryLocationInfo info);

	void clearAllMemory();

	void setMetaData(MemoryMetaData *data) { m_memoryMetaData = data; }

	void setInterrogation(bool inprogress) { m_interrogationInProgress = inprogress; }

	bool isReadOnlyRamBlock(unsigned short locationid)
	{
		for (int i=0;i<m_deviceRamMemoryList.size();i++)
		{
			if (m_deviceRamMemoryList[i]->locationid == locationid)
			{
				if (m_deviceRamMemoryList[i]->isReadOnly)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}
	QList<unsigned short> getTopLevelUniqueLocationIdList();
	QList<unsigned short> getUniqueLocationIdList();
	MemoryLocation* getLocalRamBlockInfo(unsigned short locationid);
	MemoryLocation* getLocalFlashBlockInfo(unsigned short locationid);

	void markLocalFlashLocationDirty(unsigned short location,unsigned short offset,unsigned short size);
	void markLocalFlashLocationClean(unsigned short locationid);
	void markLocalRamLocationDirty(unsigned short location,unsigned short offset,unsigned short size);
	void markLocalRamLocationClean(unsigned short locationid);
	bool isLocalRamDirty(unsigned short locationid);
	bool isLocalFlashDirty(unsigned short locationid);
	QList<QPair<unsigned short,QByteArray> > getDirtyFlashLocations() { return m_dirtyLocalFlashMemoryList; }
	QList<QPair<unsigned short,QByteArray> > getDirtyRamLocations() { return m_dirtyLocalRamMemoryList; }
	void clearDirtyRamLocations() { m_dirtyLocalRamMemoryList.clear(); }
	void clearDirtyFlashLocations() { m_dirtyLocalFlashMemoryList.clear(); }


private:
	bool m_checkEmsDataInUse;
	bool m_interrogationInProgress;
	MemoryMetaData *m_memoryMetaData;
	QMap<unsigned short,QList<ConfigBlock> > m_configBlockMap;
	QMap<unsigned short,ReadOnlyRamBlock> m_readOnlyMetaDataMap;
	QList<MemoryLocation*> m_ramMemoryList;
	QList<MemoryLocation*> m_flashMemoryList;
	QList<MemoryLocation*> m_deviceFlashMemoryList;
	QList<MemoryLocation*> m_deviceRamMemoryList;
	QList<MemoryLocation*> m_duplicateFlashMemoryList;
	QList<MemoryLocation*> m_duplicateRamMemoryList;
	QList<MemoryLocation*> m_tempMemoryList;
	QList<Table3DMetaData> m_table3DMetaData;
	QList<Table2DMetaData> m_table2DMetaData;
	QList<ConfigData*> m_configMetaData;
	QList<ReadOnlyRamData> m_readOnlyMetaData;
	QMap<unsigned short,QString> m_errorMap;
	QList<QPair<unsigned short,QByteArray> > m_dirtyLocalFlashMemoryList;
	QList<QPair<unsigned short,QByteArray> > m_dirtyLocalRamMemoryList;
	bool verifyMemoryBlock(unsigned short locationid,QByteArray header,QByteArray payload);
	double calcAxis(int val,QList<QPair<QString,double> > metadata);
	int backConvertAxis(double val,QList<QPair<QString,double> > metadata);

signals:
	void updateRequired(unsigned short locationid);
	void ramBlockUpdateRequest(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data);
	void flashBlockUpdateRequest(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data);
	void configRecieved(ConfigBlock,QVariant);
	void localFlashLocationDirty(unsigned short locationid);
	void localRamLocationDirty(unsigned short locationid);
public slots:
	void ramBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload);
	void flashBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload);
	void ramBytesLocalUpdate(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data);
	void flashBytesLocalUpdate(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data);
};

#endif // EMSDATA_H
