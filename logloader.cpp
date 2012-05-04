#include "logloader.h"
#include <QFile>
#include <QDebug>
LogLoader::LogLoader(QObject *parent) : QThread(parent)
{
}
void LogLoader::loadFile(QString filename)
{
	m_filename = filename;
}

void LogLoader::run()
{
	QFile logfile(m_filename);
	logfile.open(QIODevice::ReadOnly);
	//QByteArray arr = logfile.readAll();

	//logfile.close();
	int curr = 0;
	//bool escape = false;
	bool inpacket = false;
	QByteArray currPacket;
	//while (!escape || curr >= arr.size())
	QByteArray retval;
	while (!logfile.atEnd())
	{
		//logfile.read(&retval,1);
		emit logProgress(logfile.pos(),logfile.size());
		retval = logfile.read(1);
		if (retval[0] == (char)0xAA && !inpacket)
		{
			//Start byte
			//qDebug() << "Start byte";
			inpacket = true;
		}
		if (retval[0] == (char)0xCC && inpacket)
		{
			//qDebug() << "Packet size:" << currPacket.size();
			QByteArray header;
			inpacket = false;
			//currPacket.clear();
			//Parse the packet here
			int headersize = 3;
			int iloc = 1;
			bool seq = false;
			bool len = false;
			if (currPacket[iloc] & 0b00000100)
			{
				//Has header
				seq = true;
				qDebug() << "Has seq";
				headersize += 1;
			}
			if (currPacket[iloc] & 0b00000001)
			{
				//Has length
				len = true;
				qDebug() << "Has length";
				headersize += 2;
			}
			header = currPacket.mid(1,headersize);
			iloc++;
			unsigned int payloadid = (unsigned int)currPacket[iloc] << 8;

			payloadid += (unsigned char)currPacket[iloc+1];
			//qDebug() << QString::number(payloadid,16);
			qDebug() << QString::number(currPacket[iloc-1],16);
			qDebug() << QString::number(currPacket[iloc],16);
			qDebug() << QString::number(currPacket[iloc+1],16);
			qDebug() << QString::number(currPacket[iloc+2],16);
			iloc += 2;
			//qDebug() << QString::number(payloadid,16);
			if (seq)
			{
				//qDebug() << "Sequence number" << QString::number(currPacket[iloc]);
				iloc += 1;
			}
			QByteArray payload;
			if (len)
			{
				unsigned int length = currPacket[iloc] << 8;
				length += currPacket[iloc+1];
				qDebug() << "Length:" << length;
				iloc += 2;
				//curr += length;
				payload.append(currPacket.mid(iloc,length));
			}
			else
			{
				payload.append(currPacket.mid(iloc),(currPacket.length()-iloc) -1);
			}
			//Last byte of currPacket should be out checksum.
			unsigned char sum = 0;
			for (int i=0;i<header.size();i++)
			{
				sum += header[i];
			}
			for (int i=0;i<payload.size();i++)
			{
				sum += payload[i];
			}
			//qDebug() << "Payload sum:" << QString::number(sum);
			//qDebug() << "Checksum sum:" << QString::number((unsigned char)currPacket[currPacket.length()-1]);
			if (sum != (unsigned char)currPacket[currPacket.length()-1])
			{
				qDebug() << "BAD CHECKSUM!";
			}
			else
			{
				emit payloadReceived(header,payload);
				/*for (int i=0;i<m_dataFieldList->size();i++)
				{
					//ui.tableWidget->item(i,1)->setText(QString::number(m_dataFieldList[i].getValue(&payload)));
				}*/
				//payload is our actual data.
				//unsigned int rpm = (payload[26] << 8) + payload[27];

				//qDebug() << "f" << f.getValue(&payload);
				//qDebug() << QString::number(rpm);
				//qDebug() << QString::number(((unsigned short)payload[8] << 8) + (unsigned short)payload[9]);
			}
			currPacket.clear();
			qDebug() << "loop";
			msleep(25);
		}
		if (inpacket)
		{
			if (retval[0] == (char)0xBB)
			{
				//Need to escape the next byte
				retval = logfile.read(1);
				if (retval[0] == (char)0x55)
				{
					currPacket.append(0xAA);
				}
				else if (retval[0] == (char)0x44)
				{
					currPacket.append(0xBB);
				}
				else if (retval[0] == (char)0x33)
				{
					currPacket.append(0xCC);
				}
			}
			else
			{
				currPacket.append(retval[0]);
			}
		}
		curr++;
	}
	emit endOfLog();
	logfile.close();
	return;
	/*for (int i=0;i<arr.size();i++)
	{
		//i++;
		//qDebug() << QString::number(arr[i],16);
		curr = i;

		curr+=3;
		curr += 1;
		i += curr-1;
		//i++;

	}*/
}
