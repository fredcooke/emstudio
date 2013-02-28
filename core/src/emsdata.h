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
#include "configblock.h"
#include "headers.h"
#include "memorylocation.h"
#include "memorymetadata.h"
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
	QList<unsigned short> getParentOfLocalRamLocation(unsigned short id);
	bool localRamHasParent(unsigned short id);
	bool localRamHasChildren(unsigned short id);

	QString serialize(unsigned short id,bool isram);

	void populateLocalRamAndFlash();
	void populateDeviceRamAndFlashParents();
	QList<unsigned short> getTopLevelDeviceFlashLocations();
	QList<unsigned short> getTopLevelDeviceRamLocations();


	QList<unsigned short> getDuplicateTopLevelDeviceFlashLocations();
	QList<unsigned short> getDuplicateTopLevelDeviceRamLocations();

	void passLocationInfo(unsigned short locationid,MemoryLocationInfo info);

	void clearAllMemory();

	void setMetaData(MemoryMetaData data) { m_memoryMetaData = data; }

	void setInterrogation(bool inprogress) { m_interrogationInProgress = inprogress; }

private:
	bool m_checkEmsDataInUse;
	bool m_interrogationInProgress;
	MemoryMetaData m_memoryMetaData;
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
	QList<ConfigData> m_configMetaData;
	QList<ReadOnlyRamData> m_readOnlyMetaData;
	QMap<unsigned short,QString> m_errorMap;
	bool verifyMemoryBlock(unsigned short locationid,QByteArray header,QByteArray payload);
signals:
	void updateRequired(unsigned short locationid);
public slots:
	void ramBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload);
	void flashBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload);
};

#endif // EMSDATA_H
