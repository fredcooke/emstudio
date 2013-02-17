/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/
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
