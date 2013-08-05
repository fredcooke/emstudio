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

#ifndef FEMEMORYMETADATA_H
#define FEMEMORYMETADATA_H
#include <QMap>
#include <QString>
#include "memorymetadata.h"
#include "table2dmetadata.h"
#include "table3dmetadata.h"
#include "readonlyramdata.h"
#include "lookupmetadata.h"
#include "readonlyramblock.h"
#include "menusetup.h"

class FEMemoryMetaData : public MemoryMetaData
{
public:
	FEMemoryMetaData();
	bool loadMetaDataFromFile(QString filestr);
	const QMap<unsigned short,QString> errorMap() { return m_errorMap; }


	bool has2DMetaData(unsigned short locationid);
	const QList<Table2DMetaData> table2DMetaData() { return m_table2DMetaData; }
	const Table2DMetaData get2DMetaData(unsigned short locationid);

	bool has3DMetaData(unsigned short locationid);
	const QList<Table3DMetaData> table3DMetaData() { return m_table3DMetaData; }
	const Table3DMetaData get3DMetaData(unsigned short locationid);

	bool hasRORMetaData(unsigned short locationid);
	const ReadOnlyRamData getRORMetaData(unsigned short locationid);

	bool hasLookupMetaData(unsigned short locationid);
	const LookupMetaData getLookupMetaData(unsigned short locationid);

	bool hasConfigMetaData(QString name);
	const QMap<QString,QList<ConfigBlock> > configMetaData();
	const QList<ConfigBlock> getConfigMetaData(QString name);

	const MenuSetup menuMetaData() { return m_menuSetup; }

	const QString getErrorString(unsigned short code);

private:
	MenuSetup m_menuSetup;
	QMap<QString,QList<ConfigBlock> > m_configMetaData;
	bool parseMetaData(QString json);
	QMap<unsigned short,QString> m_errorMap;
	QMap<unsigned short,LookupMetaData> m_lookupMetaData;
	QList<ReadOnlyRamData> m_readOnlyMetaData;
	QMap<unsigned short,ReadOnlyRamBlock> m_readOnlyMetaDataMap;
	QList<Table3DMetaData> m_table3DMetaData;
	QList<Table2DMetaData> m_table2DMetaData;

};

#endif // FEMEMORYMETADATA_H


