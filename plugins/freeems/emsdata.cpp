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

#include "emsdata.h"
#include "QsLog.h"

EmsData::EmsData() : QObject()
{
	m_interrogationInProgress = false;
	m_checkEmsDataInUse = false;
}
QByteArray EmsData::getLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return m_ramMemoryList[i]->data();
		}
	}
	return QByteArray();
}

QByteArray EmsData::getLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return m_flashMemoryList[i]->data();
		}
	}
	return QByteArray();
}
QByteArray EmsData::getDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->locationid == id)
		{
			return m_deviceRamMemoryList[i]->data();
		}
	}
	return QByteArray();
}

QByteArray EmsData::getDeviceFlashBlock(unsigned short id)
{
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (m_deviceFlashMemoryList[i]->locationid == id)
		{
			return m_deviceFlashMemoryList[i]->data();
		}
	}
	return QByteArray();
}

bool EmsData::hasDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
bool EmsData::hasLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
void EmsData::setLocalRamBlock(unsigned short id,QByteArray data)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			m_ramMemoryList[i]->setData(data);
			return;
		}
	}
}

void EmsData::setDeviceRamBlock(unsigned short id,QByteArray data)
{
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->locationid == id)
		{
			m_deviceRamMemoryList[i]->setData(data);
			return;
		}
	}
}

void EmsData::setLocalFlashBlock(unsigned short id,QByteArray data)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			m_flashMemoryList[i]->setData(data);
			return;
		}
	}
}

bool EmsData::hasLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
bool EmsData::hasDeviceFlashBlock(unsigned short id)
{
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (m_deviceFlashMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
void EmsData::populateDeviceRamAndFlashParents()
{
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (m_deviceFlashMemoryList[i]->hasParent && m_deviceFlashMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_deviceFlashMemoryList.size();j++)
			{
				if (m_deviceFlashMemoryList[i]->parent== m_deviceFlashMemoryList[j]->locationid)
				{
					m_deviceFlashMemoryList[i]->setParent(m_deviceFlashMemoryList[j]);
				}
			}
		}
	}
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->hasParent && m_deviceRamMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_deviceRamMemoryList.size();j++)
			{
				if (m_deviceRamMemoryList[i]->parent== m_deviceRamMemoryList[j]->locationid)
				{
					m_deviceRamMemoryList[i]->setParent(m_deviceRamMemoryList[j]);
				}
			}
		}
	}


	for (int i=0;i<m_duplicateFlashMemoryList.size();i++)
	{
		if (m_duplicateFlashMemoryList[i]->hasParent && m_duplicateFlashMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_duplicateFlashMemoryList.size();j++)
			{
				if (m_duplicateFlashMemoryList[i]->parent== m_duplicateFlashMemoryList[j]->locationid)
				{
					m_duplicateFlashMemoryList[i]->setParent(m_duplicateFlashMemoryList[j]);
				}
			}
		}
	}
	for (int i=0;i<m_duplicateRamMemoryList.size();i++)
	{
		if (m_duplicateRamMemoryList[i]->hasParent && m_duplicateRamMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_duplicateRamMemoryList.size();j++)
			{
				if (m_duplicateRamMemoryList[i]->parent== m_duplicateRamMemoryList[j]->locationid)
				{
					m_duplicateRamMemoryList[i]->setParent(m_duplicateRamMemoryList[j]);
				}
			}
		}
	}

}

void EmsData::setDeviceFlashBlock(unsigned short id,QByteArray data)
{
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (m_deviceFlashMemoryList[i]->locationid == id)
		{
			m_deviceFlashMemoryList[i]->setData(data);
			return;
		}
	}
}

void EmsData::clearAllMemory()
{
	m_deviceFlashMemoryList.clear();
	m_deviceRamMemoryList.clear();
	m_flashMemoryList.clear();
	m_ramMemoryList.clear();
}
void EmsData::addDeviceRamBlock(MemoryLocation *loc)
{
	m_deviceRamMemoryList.append(loc);
	m_duplicateRamMemoryList.append(new MemoryLocation(*loc));
}

void EmsData::addLocalFlashBlock(MemoryLocation *loc)
{
	m_flashMemoryList.append(loc);
}

void EmsData::addLocalRamBlock(MemoryLocation *loc)
{
	m_ramMemoryList.append(loc);
}

void EmsData::addDeviceFlashBlock(MemoryLocation *loc)
{
	m_deviceFlashMemoryList.append(loc);
	m_duplicateFlashMemoryList.append(new MemoryLocation(*loc));
}
QList<unsigned short> EmsData::getChildrenOfLocalRamLocation(unsigned short id)
{
	QList<unsigned short> retVal;
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->hasParent)
		{
			if (m_ramMemoryList[i]->parent == id)
			{
				retVal.append(m_ramMemoryList[i]->locationid);
			}
		}
	}
	return retVal;
}

unsigned short EmsData::getParentOfLocalRamLocation(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return m_ramMemoryList[i]->parent;
		}
	}
	return 0;
}
bool EmsData::localRamHasParent(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return m_ramMemoryList[i]->hasParent;
		}
	}
	return false;
}
bool EmsData::localRamHasChildren(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->hasParent && m_ramMemoryList[i]->parent == id)
		{
			return true;
		}
	}
	return false;
}

QList<unsigned short> EmsData::getChildrenOfLocalFlashLocation(unsigned short id)
{
	QList<unsigned short> retVal;
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->hasParent)
		{
			if (m_flashMemoryList[i]->parent == id)
			{
				retVal.append(m_flashMemoryList[i]->locationid);
			}
		}
	}
	return retVal;
}

unsigned short EmsData::getParentOfLocalFlashLocation(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return m_flashMemoryList[i]->parent;
		}
	}
	return 0;
}
bool EmsData::localFlashHasParent(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return m_flashMemoryList[i]->hasParent;
		}
	}
	return false;
}
bool EmsData::localFlashHasChildren(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->hasParent && m_flashMemoryList[i]->parent == id)
		{
			return true;
		}
	}
	return false;
}


QList<unsigned short> EmsData::getTopLevelDeviceFlashLocations()
{
	QList<unsigned short> retval;
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (!m_deviceFlashMemoryList[i]->hasParent)
		{
			retval.append(m_deviceFlashMemoryList[i]->locationid);
		}
	}
	return retval;
}
void EmsData::populateLocalRamAndFlash()
{
	if (m_ramMemoryList.size() == 0)
	{
		//Internal ram list is empty. Let's fill it.
		for (int i=0;i<m_deviceRamMemoryList.size();i++)
		{
			m_ramMemoryList.append(new MemoryLocation(*m_deviceRamMemoryList[i]));
		}
		//We have to do something special here, since m_ramMemoryList's parents point to m_deviceRamMemoryList.
		for (int i=0;i<m_ramMemoryList.size();i++)
		{
			if (m_ramMemoryList[i]->hasParent)
			{
				for (int j=0;j<m_ramMemoryList.size();j++)
				{
					if (m_ramMemoryList[i]->parent== m_ramMemoryList[j]->locationid)
					{
						m_ramMemoryList[i]->setParent(m_ramMemoryList[j]);
					}
				}
			}
		}
	}
	if (m_flashMemoryList.size() == 0)
	{
		for (int i=0;i<m_deviceFlashMemoryList.size();i++)
		{
			m_flashMemoryList.append(new MemoryLocation(*m_deviceFlashMemoryList[i]));
		}
		//We have to do something special here, since m_ramMemoryList's parents point to m_deviceRamMemoryList.
		for (int i=0;i<m_flashMemoryList.size();i++)
		{
			if (m_flashMemoryList[i]->hasParent)
			{
				for (int j=0;j<m_flashMemoryList.size();j++)
				{
					if (m_flashMemoryList[i]->parent== m_flashMemoryList[j]->locationid)
					{
						m_flashMemoryList[i]->setParent(m_flashMemoryList[j]);
					}
				}
			}
		}
	}
}
QList<unsigned short> EmsData::getTopLevelUniqueLocationIdList()
{
	QList<unsigned short> retval;
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (!m_deviceRamMemoryList[i]->hasParent)
		{
			retval.append(m_deviceRamMemoryList[i]->locationid);
		}
	}
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (!m_deviceFlashMemoryList[i]->hasParent && !retval.contains(m_deviceFlashMemoryList[i]->locationid))
		{
			retval.append(m_deviceFlashMemoryList[i]->locationid);
		}
	}
	return retval;


}
QList<unsigned short> EmsData::getUniqueLocationIdList()
{
	QList<unsigned short> retval;
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		retval.append(m_deviceRamMemoryList[i]->locationid);
	}
	for (int i=0;i<m_deviceFlashMemoryList.size();i++)
	{
		if (!retval.contains(m_deviceFlashMemoryList[i]->locationid))
		{
			retval.append(m_deviceFlashMemoryList[i]->locationid);
		}
	}
	return retval;
}

QList<unsigned short> EmsData::getTopLevelDeviceRamLocations()
{
	QList<unsigned short> retval;
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (!m_deviceRamMemoryList[i]->hasParent)
		{
			retval.append(m_deviceRamMemoryList[i]->locationid);
		}
	}
	return retval;
}

QString EmsData::serialize(unsigned short id,bool isram)
{
	QString val = "";
	QByteArray block;
	if (isram)
	{
		block = getDeviceRamBlock(id);
	}
	else
	{
		block = getDeviceFlashBlock(id);
	}
	for (int j=0;j<block.size();j++)
	{
		val += QString::number((unsigned char)block[j],16).toUpper() + ",";
	}
	val = val.mid(0,val.length()-1);
	return val;
}

void EmsData::passLocationInfo(unsigned short locationid,MemoryLocationInfo info)
{
	if (info.isRam && info.isFlash)
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = info.size;
		loc->ramAddress = info.ramaddress;
		loc->ramPage = info.rampage;
		loc->flashAddress = info.flashaddress;
		loc->flashPage = info.flashpage;
		loc->isReadOnly = info.isReadOnly;
		if (info.hasParent)
		{
			loc->parent = info.parent;
			loc->hasParent = true;
		}
		else
		{
			loc->hasParent = false;
		}
		loc->isRam = true;
		loc->isFlash = true;
		if (!hasDeviceFlashBlock(locationid))
		{
			addDeviceFlashBlock(new MemoryLocation(*loc));
		}
		if (!hasDeviceRamBlock(locationid))
		{
			addDeviceRamBlock(loc);
		}
		else
		{
			delete loc;
		}

	}
	else if (info.isFlash)
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = info.size;
		loc->flashAddress = info.flashaddress;
		loc->flashPage = info.flashpage;
		loc->isReadOnly = info.isReadOnly;
		if (info.hasParent)
		{
			loc->parent = info.parent;
			loc->hasParent = true;
		}
		else
		{
			loc->hasParent = false;
		}
		loc->isRam = false;
		loc->isFlash = true;
		if (!hasDeviceFlashBlock(locationid))
		{
			addDeviceFlashBlock(loc);
		}
		else
		{
			delete loc;
		}

	}
	else if (info.isRam)
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = info.size;
		loc->ramAddress = info.ramaddress;
		loc->ramPage = info.rampage;
		loc->isReadOnly = info.isReadOnly;
		if (info.hasParent)
		{
			loc->parent = info.parent;
			loc->hasParent = true;
		}
		else
		{
			loc->hasParent = false;
		}
		loc->isRam = true;
		loc->isFlash = false;
		if (!hasDeviceRamBlock(locationid))
		{
			addDeviceRamBlock(loc);
		}
		else
		{
			delete loc;
		}

	}
	else
	{
	}
	/*if (flags.contains(FreeEmsComms::BLOCK_IS_RAM) && flags.contains((FreeEmsComms::BLOCK_IS_FLASH)))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isRam = true;
		loc->isFlash = true;
		loc->ramAddress = ramaddress;
		loc->ramPage = rampage;
		loc->flashAddress = flashaddress;
		loc->flashPage = flashpage;
		//m_deviceRamMemoryList.append(loc);
		emsData->addDeviceRamBlock(loc);
		emsData->addDeviceFlashBlock(new MemoryLocation(*loc));
		//m_flashMemoryList.append(new MemoryLocation(*loc));
		//m_deviceFlashMemoryList.append(new MemoryLocation(*loc));

	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isFlash = true;
		loc->isRam = false;
		loc->flashAddress = flashaddress;
		loc->flashPage = flashpage;
		//m_deviceFlashMemoryList.append(loc);
		emsData->addDeviceFlashBlock(loc);
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isRam = true;
		loc->isFlash = false;
		loc->ramAddress = ramaddress;
		loc->ramPage = rampage;
		//m_deviceRamMemoryList.append(loc);
		emsData->addDeviceRamBlock(loc);
	}*/
}


void EmsData::ramBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload)
{
	Q_UNUSED(header)
	QLOG_TRACE() << "Ram Block retrieved:" << "0x" + QString::number(locationid,16).toUpper();

	if (!hasDeviceRamBlock(locationid))
	{
		//This should not happen
		/*RawDataBlock *block = new RawDataBlock();
		block->locationid = locationid;
		block->header = header;
		block->data = payload;
		//m_flashRawBlockList.append(block);
		m_deviceRamRawBlockList.append(block);*/
	}
	else
	{
		//Check to see if it's supposed to be a table, and if so, check size
		if (!verifyMemoryBlock(locationid,header,payload))
		{
			//QMessageBox::information(this,"Error","RAM Location ID 0x" + QString::number(locationid,16).toUpper() + " should be 1024 sized, but it is " + QString::number(payload.size()) + ". This should never happen");
			QLOG_ERROR() << "RAM Location ID 0x" + QString::number(locationid,16).toUpper() + " should be 1024 sized, but it is " + QString::number(payload.size()) + ". This should never happen";
			return;
		}
		if (getDeviceRamBlock(locationid).isEmpty())
		{
			//This should not happen
			QLOG_ERROR() << "Ram block on device while ram block on tuner is empty! This should not happen" << "0x" + QString::number(locationid,16).toUpper();
			QLOG_ERROR() << "Current block size:" << getDeviceRamBlock(locationid).size();
			setLocalRamBlock(locationid,payload);
			setDeviceRamBlock(locationid,payload);
		}
		else
		{
			if (m_interrogationInProgress)
			{
				//checkEmsData->setDeviceRamBlock(locationid,payload);
				for (int i=0;i<m_duplicateRamMemoryList.size();i++)
				{
					if (m_duplicateRamMemoryList[i]->locationid == locationid)
					{
						m_duplicateRamMemoryList[i]->setData(payload);
						break;
					}
				}
				m_checkEmsDataInUse = true;
			}
			else
			{
				if (getDeviceRamBlock(locationid) != payload)
				{
					QLOG_ERROR() << "Ram block on device does not match ram block on tuner! This should ONLY happen during a manual update!";
					QLOG_ERROR() << "Tuner ram size:" << getDeviceRamBlock(locationid).size();
					setDeviceRamBlock(locationid,payload);
				}
				if (payload != getLocalRamBlock(locationid))
				{
					//We need to prompt the user, local ram is out of date.
					setLocalRamBlock(locationid,payload);
				}
			}
		}
		//updateDataWindows(locationid);
		emit updateRequired(locationid);
	}
	return;
}

void EmsData::flashBlockUpdate(unsigned short locationid, QByteArray header, QByteArray payload)
{
	QLOG_TRACE() << "Flash Block retrieved:" << "0x" + QString::number(locationid,16).toUpper();
	Q_UNUSED(header)
	if (!verifyMemoryBlock(locationid,header,payload))
	{
		//QMessageBox::information(this,"Error","Flash Location ID 0x" + QString::number(locationid,16).toUpper() + " should be 1024 sized, but it is " + QString::number(payload.size()) + ". This should never happen");
		QLOG_ERROR() << "Flash Location ID 0x" + QString::number(locationid,16).toUpper() + " should be 1024 sized, but it is " + QString::number(payload.size()) + ". This should never happen";
		return;
	}
	if (hasDeviceFlashBlock(locationid))
	{
			if (getDeviceFlashBlock(locationid).isEmpty())
			{
				setLocalFlashBlock(locationid,payload);
				setDeviceFlashBlock(locationid,payload);
				emit updateRequired(locationid);
				return;
			}
			else
			{
				if (m_interrogationInProgress)
				{
					//checkEmsData->setDeviceFlashBlock(locationid,payload);
					for (int i=0;i<m_duplicateFlashMemoryList.size();i++)
					{
						if (m_duplicateFlashMemoryList[i]->locationid == locationid)
						{
							m_duplicateFlashMemoryList[i]->setData(payload);
							break;
						}
					}
					m_checkEmsDataInUse = true;
				}
				else
				{
					if (getDeviceFlashBlock(locationid) != payload)
					{
						QLOG_ERROR() << "Flash block in memory does not match flash block on tuner! This should not happen!";
						QLOG_ERROR() << "Flash size:" << getDeviceFlashBlock(locationid).size();
						QLOG_ERROR() << "Flash ID:" << "0x" + QString::number(locationid,16).toUpper();
						setDeviceFlashBlock(locationid,payload);
					}
					if (getLocalFlashBlock(locationid) != payload)
					{
						setLocalFlashBlock(locationid,payload);
					}
				}
			}
	}
	emit updateRequired(locationid);
	//updateDataWindows(locationid);
	return;
}
void EmsData::ramBytesLocalUpdate(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data)
{
	if (!hasLocalRamBlock(locationid))
	{
		QLOG_WARN() << "Write requested when there is no local ram block!";
		return;
	}
	if (getLocalRamBlock(locationid).mid(offset,size) == data)
	{
		QLOG_WARN() << "Data in application ram memory unchanged, no reason to send write for single value";
		return;
	}
	QLOG_TRACE() << "Updating ram locationid" << locationid << "with" << data.size() << "bytes at offset" << offset;
	setLocalRamBlock(locationid,getLocalRamBlock(locationid).replace(offset,size,data));
	//emit updateRequired(locationid);
	emit ramBlockUpdateRequest(locationid,offset,size,data);
}

void EmsData::flashBytesLocalUpdate(unsigned short locationid,unsigned short offset,unsigned short size,QByteArray data)
{
	if (!hasLocalFlashBlock(locationid))
	{
		QLOG_ERROR() << "Requested flash update for locationid" << locationid << "but location has no flash!";
		return;
	}
	if (getLocalFlashBlock(locationid).mid(offset,size) == data)
	{
		QLOG_WARN() << "Data in application flash memory unchanged, no reason to send write for single value";
		return;
	}
	QLOG_TRACE() << "Updating flash locationid" << locationid << "with" << data.size() << "bytes at offset" << offset;
	setLocalFlashBlock(locationid,getLocalFlashBlock(locationid).replace(offset,size,data));
	//emit updateRequired(locationid);
	emit flashBlockUpdateRequest(locationid,offset,size,data); //don't emit a signal for flash updates, those are always manual.
}

bool EmsData::verifyMemoryBlock(unsigned short locationid,QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	if (m_memoryMetaData->has2DMetaData(locationid))
	{
		if ((unsigned int)payload.size() != m_memoryMetaData->get2DMetaData(locationid).size)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	if (m_memoryMetaData->has3DMetaData(locationid))
	{
		if ((unsigned int)payload.size() != m_memoryMetaData->get3DMetaData(locationid).size)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	//If we get here, the table does not exist in meta data
	return true;

}


QList<unsigned short> EmsData::getDuplicateTopLevelDeviceFlashLocations()
{
	if (!m_checkEmsDataInUse)
	{
		return QList<unsigned short>();
	}
	QList<unsigned short> retval;
	for (int i=0;i<m_duplicateFlashMemoryList.size();i++)
	{
		if ((m_duplicateFlashMemoryList[i]->data() != getDeviceFlashBlock(m_duplicateFlashMemoryList[i]->locationid)) && !m_duplicateFlashMemoryList[i]->hasParent)
		{
			retval.append(m_duplicateFlashMemoryList[i]->locationid);
		}
	}
	return retval;
}

QList<unsigned short> EmsData::getDuplicateTopLevelDeviceRamLocations()
{
	if (!m_checkEmsDataInUse)
	{
		return QList<unsigned short>();
	}
	QList<unsigned short> retval;
	for (int i=0;i<m_duplicateRamMemoryList.size();i++)
	{
		if ((m_duplicateRamMemoryList[i]->data() != getDeviceRamBlock(m_duplicateRamMemoryList[i]->locationid)) && !m_duplicateRamMemoryList[i]->hasParent)
		{
			retval.append(m_duplicateRamMemoryList[i]->locationid);
		}
	}
	return retval;
}
