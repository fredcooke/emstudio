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

#ifndef HEADERS_H
#define HEADERS_H
#include <QPair>
#include <QList>
#include <QString>


#define define2string_p(x) #x
#define define2string(x) define2string_p(x)
//#define TABLE_3D_PAYLOAD_SIZE 1024 //Pushing this into meta data rather than having it hardcoded
//#define TABLE_2D_PAYLOAD_SIZE 64 //Pushing this into meta data rather than having it hardcoded


enum DataType
{
	DATA_UNDEFINED=1,
	DATA_TABLE_2D=2,
	DATA_TABLE_3D=3,
	DATA_TABLE_LOOKUP=4,
	DATA_CONFIG=5
};
enum SerialPortStatus
{
	UNABLE_TO_CONNECT, //Unable to open the com port
	UNABLE_TO_LOCK, //Unable to aquire a lock on the com port
	UNABLE_TO_READ,
	UNABLE_TO_WRITE,
	NO_DATA, //Opened com port, but no respose to messages/no data?
	INVALID_DATA, //Opened com port, garbage data
	LOADER_MODE, //Opened com port, in loader mode
	NONE //No error
};

class ConfigData
{
public:
	QString configTitle;
	QString configDescription;
	unsigned short locationId;
	unsigned char offset;
	unsigned char elementSize;
	QList<QPair<QString,double> > elementCalc;
	QString type;
	unsigned char arraySize;
};


class ReadOnlyRamData
{
public:
	QString dataTitle;
	QString dataDescription;
	unsigned short locationId;
	unsigned char offset;
	unsigned char size;
	QList<QPair<QString,double> > calc;
};
class LookupMetaData
{
public:
	QString title;
	unsigned short locationid;
	bool editable;
};

class ReadOnlyRamBlock
{
public:
	QString title;
	unsigned short locationid;
	QList<ReadOnlyRamData> m_ramData;
};

class Table3DMetaData
{
public:
	Table3DMetaData() { xDp=2; yDp=2; zDp=2; valid=false;}
	QString tableTitle;
	bool valid;
	unsigned int size;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	QString zAxisTitle;
	QString xHighlight;
	QString yHighlight;
	int xDp;
	int yDp;
	int zDp;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
	QList<QPair<QString,double> > zAxisCalc;
};
class Table2DMetaData
{
public:
	Table2DMetaData() { xDp=2; yDp=2; valid=false;}
	QString tableTitle;
	bool valid;
	unsigned int size;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	QString xHighlight;
	int xDp;
	int yDp;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
};
class MemoryLocationInfo
{
public:
	unsigned short locationid;
	unsigned short rawflags;
	QList<unsigned short> flags;
	QList<QPair<QString,QString> > propertymap;
	bool isRam;
	bool isFlash;
	bool hasParent;
	bool isReadOnly;
	DataType type;
	unsigned short parent;
	unsigned char rampage;
	unsigned char flashpage;
	unsigned short ramaddress;
	unsigned short flashaddress;
	unsigned short size;
};


#endif // HEADERS_H
