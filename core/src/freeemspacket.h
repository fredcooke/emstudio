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

#ifndef FREEEMSPACKET_H
#define FREEEMSPACKET_H
#include <QByteArray>
class FreeEmsPacket
{
public:
	enum PayloadID
	{
		InterfaceVersionRequest=0,
		InterfaceVersionResponse,
		FirmwareVersionRequest,
		FirmwareVersionResponse,
		MaxPacketSizeRequest,
		MaxPacketSizeResponse,
		EchoRequest,
		EchoResponse,
		SoftResetCommand,
		SoftResetMalformedResponse,
		HardResetCommand,
		HardResetMalformedResponse,
		DummyValueOne,
		ErrorResponse,
		DummyValueTwo,
		DebugData
	};

	//Packet format:
	//[start byte][data packet][end byte]
	//[start byte] == 0xAA
	//[end byte] == 0xCC
	//escaped 0xAA == 0xBB 0x55
	//escaped 0xCC == 0xBB 0x55
	//escaped escape == 0xBB 0x44

	//[data packet]
	//[header][payload][checksum]

	//[header]
	//[flag,1][payloadid,2][sequence number,1,optional][length,2,optional]
	//Length EXCLUDES header and checksum, so ONLY payload.
	//checksum is BEFORE any escaping, and includes header+payload
	QByteArray updateRamBlockPacket(int locationid,int offset,int size,QByteArray data);

	FreeEmsPacket();
	unsigned short length;
	unsigned short offset;
	unsigned short size;
	unsigned short locationid;
	unsigned char listtype;
	unsigned short listmask;
	QByteArray rawbuffer;
};

#endif // FREEEMSPACKET_H
