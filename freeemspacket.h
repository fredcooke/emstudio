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
};

#endif // FREEEMSPACKET_H
