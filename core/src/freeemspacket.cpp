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

#include "freeemspacket.h"

FreeEmsPacket::FreeEmsPacket()
{
}
QByteArray FreeEmsPacket::updateRamBlockPacket(int locationid,int offset,int size,QByteArray data)
{
	QByteArray ret;
	int flags=0;
	int payloadlength = data.size() + 6;
	//TODO Actually set some flag values here

	//Header
	ret.append(flags);
	ret.append((char)0x01);
	ret.append((char)0x00);

	ret.append((payloadlength >> 8) & 0xFF);
	ret.append((payloadlength) & 0xFF);

	//Packet
	ret.append((locationid >> 8) & 0xFF);
	ret.append((locationid) & 0xFF);
	ret.append((offset >> 8) & 0xFF);
	ret.append((offset) & 0xFF);
	ret.append((size >> 8) & 0xFF);
	ret.append((size) & 0xFF);
	ret.append(data);
	return ret;
}
