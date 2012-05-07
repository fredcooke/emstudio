#include "datapacketdecoder.h"

DataPacketDecoder::DataPacketDecoder(QObject *parent) : QObject(parent)
{
	populateDataFields();
}
void DataPacketDecoder::decodePayload(QByteArray payload)
{
	QMap<QString,double> m_valueMap;
	for (int i=0;i<m_dataFieldList.size();i++)
	{
		double value = m_dataFieldList[i].getValue(&payload);
		m_valueMap[m_dataFieldList[i].name()] = value;
		//ui.tableWidget->item(i,1)->setText(QString::number(value));
	}
	emit payloadDecoded(m_valueMap);
}

void DataPacketDecoder::populateDataFields()
{
	m_dataFieldList.append(DataField("IAT","Intake Air Temperature",0,2,100.0));
	m_dataFieldList.append(DataField("CHT","Coolant/Head Temperature",2,2,100.0));
	m_dataFieldList.append(DataField("TPS","Throttle Position Sensor",4,2,100.0));
	m_dataFieldList.append(DataField("EGO","Exhaust Gas Oxygen",6,2,32768.0));
	m_dataFieldList.append(DataField("MAP","Manifold Air Pressure",8,2,100.0));
	m_dataFieldList.append(DataField("AAP","Ambient Atmosphere Pressure",10,2,100.0));
	m_dataFieldList.append(DataField("BRV","Battery Reference Voltage",12,2,1000.0));
	m_dataFieldList.append(DataField("MAT","Manifold Air Temperature",14,2,100.0));
	m_dataFieldList.append(DataField("EGO2","Exhaust Gas Oxygen 2",16,2,32768.0));
	m_dataFieldList.append(DataField("IAP","Intercooler Absolute Pressure",18,2,100.0));
	m_dataFieldList.append(DataField("MAF","Mass Air Flow",20,2,1.0));
	m_dataFieldList.append(DataField("DMAP","Delta Map",22,2,1.0));
	m_dataFieldList.append(DataField("DTPS","Delta TPS",24,2,1.0));
	m_dataFieldList.append(DataField("RPM","Engine Speed",26,2,1.0));
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
	m_dataFieldList.append(DataField("tempClock","Once per datalog message",59,1,1.0));
	m_dataFieldList.append(DataField("coreStatusA","Duplicate",60,1,1.0));
	m_dataFieldList.append(DataField("decoderFlags","Various decoder state flags",61,1,1.0));
	m_dataFieldList.append(DataField("flaggableFlags","Flags to go with flaggables",62,2,1.0));
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
