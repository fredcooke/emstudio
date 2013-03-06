/**************************************************************************************
 * Copyright (c) 2013, Michael Carpenter  (malcom2073@gmail.com)                      *
 * All rights reserved.                                                               *
 *                                                                                    *
 * Redistribution and use in source and binary forms, with or without                 *
 * modification, are permitted provided that the following conditions are met:        *
 *     * Redistributions of source code must retain the above copyright               *
 *       notice, this list of conditions and the following disclaimer.                *
 *     * Redistributions in binary form must reproduce the above copyright            *
 *       notice, this list of conditions and the following disclaimer in the          *
 *       documentation and/or other materials provided with the distribution.         *
 *     * Neither the name EMStudio nor the                                            *
 *       names of its contributors may be used to endorse or promote products         *
 *       derived from this software without specific prior written permission.        *
 *                                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND    *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE             *
 * DISCLAIMED. IN NO EVENT SHALL MICHAEL CARPENTER BE LIABLE FOR ANY                  *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;       *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND        *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                       *
 **************************************************************************************/

#include "o5edatapacketdecoder.h"
#include <QDebug>
O5EDataPacketDecoder::O5EDataPacketDecoder() : DataPacketDecoder()
{
}
void O5EDataPacketDecoder::populateDataFields()
{

}

int O5EDataPacketDecoder::fieldSize()
{
	return m_dataList.size();
}

DataField O5EDataPacketDecoder::getField(int num)
{
	if (num >= 0 && num < m_dataList.size())
	{
		return m_dataList[num];
	}
	return DataField();
}
void O5EDataPacketDecoder::setDataDefinitions(QList<DataField> datalist)
{
	m_dataList = datalist;
}

void O5EDataPacketDecoder::decodePayload(QByteArray payload)
{
	/*QString val = "";
	for (int i=0;i<10;i++)
	{
		val += QString::number((unsigned char)payload[i]) + ",";
	}
	qDebug() << val;

	double rpm = (double)((((unsigned char)payload[1]) << 8) + (unsigned char)payload[2]);
	if (rpm == 0)
	{
//		map["RPM"] = 1;
	}
	else
	{
		map["RPM"] = rpm;
	}*/
	QVariantMap map;
	for (int i=0;i<m_dataList.size();i++)
	{
		if (m_dataList[i].size() + m_dataList[i].offset() < payload.size())
		{
			double val = m_dataList[i].getValue(&payload);
			map[m_dataList[i].name()] = val;
		}
	}
	//qDebug() << "Decoding RPM:" << map["RPM"];
	emit payloadDecoded(map);
}
