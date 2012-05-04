#include "freeemspacket.h"

FreeEmsPacket::FreeEmsPacket()
{
}
QByteArray FreeEmsPacket::updateRamBlockPacket(int locationid,int offset,int size,QByteArray data)
{
	QByteArray ret;
	int flags;
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
