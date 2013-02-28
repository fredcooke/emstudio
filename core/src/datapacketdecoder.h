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

#ifndef DATAPACKETDECODER_H
#define DATAPACKETDECODER_H

#include <QObject>
#include <QMap>
#include <QVariantMap>
#include "datafield.h"
class DataPacketDecoder : public QObject
{
    Q_OBJECT
public:
	virtual void populateDataFields()=0;
	virtual int fieldSize()=0;
	virtual DataField getField(int num)=0;
//signals:
	//void payloadDecoded(QVariantMap data);
public slots:
	virtual void decodePayload(QByteArray payload)=0;
};

#endif // DATAPACKETDECODER_H
