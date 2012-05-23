/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "datapacketdecoder.h"
#include <QDebug>
DataPacketDecoder::DataPacketDecoder(QObject *parent) : QObject(parent)
{
	populateDataFields();
}
void DataPacketDecoder::decodePayload(QByteArray payload)
{
	QVariantMap m_valueMap;
	for (int i=0;i<m_dataFieldList.size();i++)
	{
		if (m_dataFieldList[i].isFlag())
		{
			bool value = m_dataFieldList[i].flagValue(&payload);
			qDebug() << "Decoding flag:" << m_dataFieldList[i].name() << "Value:" << value;
			m_valueMap[m_dataFieldList[i].name()] = value;
		}
		else
		{
			double value = m_dataFieldList[i].getValue(&payload);
			qDebug() << "Decoding value:" << m_dataFieldList[i].name() << "Value:" << value;
			m_valueMap[m_dataFieldList[i].name()] = value;
		}
		//ui.tableWidget->item(i,1)->setText(QString::number(value));
	}
	emit payloadDecoded(m_valueMap);
}

void DataPacketDecoder::populateDataFields()
{
	m_dataFieldList.append(DataField("IAT","Intake Air Temperature",0,2,100,-273.15));
	m_dataFieldList.append(DataField("CHT","Coolant/Head Temperature",2,2,100,-273.15));
	m_dataFieldList.append(DataField("TPS","Throttle Position Sensor",4,2,640.0));
	m_dataFieldList.append(DataField("EGO","Exhaust Gas Oxygen",6,2,32768.0));
	m_dataFieldList.append(DataField("MAP","Manifold Air Pressure",8,2,100.0));
	m_dataFieldList.append(DataField("AAP","Ambient Atmosphere Pressure",10,2,100.0));
	m_dataFieldList.append(DataField("BRV","Battery Reference Voltage",12,2,1000.0));
	m_dataFieldList.append(DataField("MAT","Manifold Air Temperature",14,2,100.0,-273.15));
	m_dataFieldList.append(DataField("EGO2","Exhaust Gas Oxygen 2",16,2,32768.0));
	m_dataFieldList.append(DataField("IAP","Intercooler Absolute Pressure",18,2,100.0));
	m_dataFieldList.append(DataField("MAF","Mass Air Flow",20,2,1.0));
	m_dataFieldList.append(DataField("DMAP","Delta Map",22,2,1.0));
	m_dataFieldList.append(DataField("DTPS","Delta TPS",24,2,1.0));
	m_dataFieldList.append(DataField("RPM","Engine Speed",26,2,2.0));
	m_dataFieldList.append(DataField("DRPM","Delta RPM",28,2,1.0));
	m_dataFieldList.append(DataField("DDRPM","Delta Delta RPM",30,2,1.0));
	m_dataFieldList.append(DataField("LoadMain","Configurable Unit of Load",32,2,512.0));
	m_dataFieldList.append(DataField("VEMain","Volumentric Efficiency",34,2,512.0));
	m_dataFieldList.append(DataField("Lambda","Integral Lambda",36,2,32768.0));
	m_dataFieldList.append(DataField("AirFlow","Raw Intermediate",38,2,1.0));
	m_dataFieldList.append(DataField("densityAndFuel","raw Intermediate",40,2,1.0));
	m_dataFieldList.append(DataField("BasePW","Raw PW Before corrections",42,2,1250));
	m_dataFieldList.append(DataField("ETE","Engine Temp Enrichment Percentage",44,2,100.0/32768.0));
	m_dataFieldList.append(DataField("TFCTotal","Total Transient Fuel Corrections",46,2,100.0/32768.0));
	m_dataFieldList.append(DataField("EffectivePW","Actual PW of fuel delivery",48,2,1250.0));
	m_dataFieldList.append(DataField("IDT","PW duration before fuel flow begins",50,2,1250.0));
	m_dataFieldList.append(DataField("RefPW","Reference electrical PW",52,2,1250.0));
	m_dataFieldList.append(DataField("Advance","Ignition advance",54,2,50.0));
	m_dataFieldList.append(DataField("Dwell","Dwell period",56,2,1520.0));
	m_dataFieldList.append(DataField("tempClock","Once per datalog message",58,1,1.0));
	m_dataFieldList.append(DataField("spareChar","Unused at this time",59,1,1.0));
	//m_dataFieldList.append(DataField("coreStatusA","Duplicate",60,1,1.0,0,true));
	//m_dataFieldList.append(DataField("decoderFlags","Various decoder state flags",61,1,1.0,0,true));
	//m_dataFieldList.append(DataField("flaggableFlags","Flags to go with flaggables",62,2,1.0,0,true));

	//Flaggable flags
	m_dataFieldList.append(DataField("callsToUISRs","to ensure we aren't accidentally triggering unused ISRs.",62,2,1.0,0,0,0,true,0));
	m_dataFieldList.append(DataField("lowVoltageConditions","low voltage conditions.",62,2,1.0,0,0,0,true,1));
	m_dataFieldList.append(DataField("decoderSyncLosses","Number of times cam, crank or combustion sync is lost.",62,2,1.0,0,0,0,true,2));
	m_dataFieldList.append(DataField("spare","spare",62,2,1.0,0,0,0,true,3));
	m_dataFieldList.append(DataField("decoderSyncStateClears","Sync loss called when not synced yet, thus discarding data and preventing sync.",62,2,1.0,0,0,0,true,4));
	m_dataFieldList.append(DataField("serialNoiseErrors","Incremented when noise is detected",62,2,1.0,0,0,0,true,5));
	m_dataFieldList.append(DataField("serialFramingErrors","Incremented when a framing error occurs",62,2,1.0,0,0,0,true,6));
	m_dataFieldList.append(DataField("serialParityErrors","Incremented when a parity error occurs",62,2,1.0,0,0,0,true,7));
	m_dataFieldList.append(DataField("serialOverrunErrors","Incremented when overrun occurs (duplicated in KeyUserDebug below)",62,2,1.0,0,0,0,true,8));
	m_dataFieldList.append(DataField("serialEscapePairMismatches","Incremented when an escape is found but not followed by an escapee",62,2,1.0,0,0,0,true,9));
	m_dataFieldList.append(DataField("serialStartsInsideAPacket","Incremented when a start byte is found inside a packet",62,2,1.0,0,0,0,true,10));
	m_dataFieldList.append(DataField("serialPacketsOverLength","Incremented when the buffer fills up before the end",62,2,1.0,0,0,0,true,11));
	m_dataFieldList.append(DataField("serialChecksumMismatches","Incremented when calculated checksum did not match the received one",62,2,1.0,0,0,0,true,12));
	m_dataFieldList.append(DataField("serialPacketsUnderLength","Incremented when a packet is found that is too short",62,2,1.0,0,0,0,true,13));
	m_dataFieldList.append(DataField("commsDebugMessagesNotSent","Incremented when a debug message can't be sent due to the TX buffer",62,2,1.0,0,0,0,true,14));
	m_dataFieldList.append(DataField("commsErrorMessagesNotSent","Incremented when an error message can't be sent due to the TX buffer",62,2,1.0,0,0,0,true,15));


	m_dataFieldList.append(DataField("currentEvent","Which input event was last",64,1,1.0));
	m_dataFieldList.append(DataField("syncLostWithThisID","UID for reason beind loss of sync",65,1,1.0));
	m_dataFieldList.append(DataField("syncLostOnThisEvent","Where in the input pattern sync was lost",66,1,1.0));
	m_dataFieldList.append(DataField("syncCaughtOnThisevent","Where in the input pattern sync was recovered",67,1,1.0));
	m_dataFieldList.append(DataField("syncResetCalls","Sum of losses, corrections, and state clears",68,1,1.0));
	m_dataFieldList.append(DataField("primaryTeethSeen","",69,1,1.0));
	m_dataFieldList.append(DataField("secondaryTeethSeen","",70,1,1.0));
	m_dataFieldList.append(DataField("serialOverrunErrors","",71,1,1.0));
	m_dataFieldList.append(DataField("serialHardwareErrors","",72,1,1.0));
	m_dataFieldList.append(DataField("serialAndCommsCodeErrors","",73,1,1.0));
	m_dataFieldList.append(DataField("inputEventTimeTolerance","",74,2,1.0));
	m_dataFieldList.append(DataField("zsp10","",76,2,1.0));
	m_dataFieldList.append(DataField("zsp9","",78,2,1.0));
	m_dataFieldList.append(DataField("zsp8","",80,2,1.0));
	m_dataFieldList.append(DataField("zsp7","",82,2,1.0));
	m_dataFieldList.append(DataField("zsp6","",84,2,1.0));
	m_dataFieldList.append(DataField("zsp5","",86,2,1.0));
	m_dataFieldList.append(DataField("zsp4","",88,2,1.0));
	m_dataFieldList.append(DataField("zsp3","",90,2,1.0));
	m_dataFieldList.append(DataField("clockInMilliSeconds","Clock in milliseconds",92,2,1.0));//was 88
	m_dataFieldList.append(DataField("clock8thMssInMillis","Clock in 8th milliseconds",94,2,8.0));
	m_dataFieldList.append(DataField("ignitionLimiterFlags","",96,1,1.0));
	m_dataFieldList.append(DataField("injectionLimiterFlags","",97,1,1.0));


}
