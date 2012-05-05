#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "datafield.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
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


	/*new LogField("syncLostOnThisEvent", types.UINT8), // Where in the input pattern it all went very badly wrong
	new LogField("syncCaughtOnThisEvent", types.UINT8), // Where in the input pattern that things started making sense
	new LogField("syncResetCalls", types.UINT8), // Sum of losses, corrections and state clears
	new LogField("primaryTeethSeen", types.UINT8), // Free running counters for number of input events, useful at lower RPM
	new LogField("secondaryTeethSeen", types.UINT8), // Free running counters for number of input events, useful at lower RPM
	new LogField("serialOverrunErrors", types.UINT8), // Incremented when an overrun occurs due to high ISR load, just a fact of life at high RPM
	new LogField("serialHardwareErrors", types.UINT8), // Sum of noise, parity, and framing errors
	new LogField("serialAndCommsCodeErrors", types.UINT8), // Sum of checksum, escape mismatches, starts inside, and over/under length
	new LogField("inputEventTimeTolerance"), // Required to tune noise rejection over RPM TODO add to LT1 and MissingTeeth
	new LogField("zsp10"), // Spare US variable
	new LogField("zsp9"), // Spare US variable
	new LogField("zsp8"), // Spare US variable
	new LogField("zsp7"), // Spare US variable
	new LogField("zsp6"), // Spare US variable
	new LogField("zsp5"), // Spare US variable
	new LogField("zsp4"), // Spare US variable
	new LogField("zsp3"), // Spare US variable
	new LogField("clockInMilliSeconds"), // Migrate to start of all large datalogs once analysed
	new LogField("clock8thMSsInMillis", 8), // Migrate to start of all large datalogs once analysed
	new LogField("ignitionLimiterFlags", types.BITS8, ignitionLimiterFlagsNames),
	new LogField("injectionLimiterFlags", types.BITS8, injectionLimiterFlagsNames)
	*/


	ui.setupUi(this);
	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
	connect(ui.loadLogPushButton,SIGNAL(clicked()),this,SLOT(loadLogButtonClicked()));
	connect(ui.playLogPushButton,SIGNAL(clicked()),this,SLOT(playLogButtonClicked()));
	connect(ui.pauseLogPushButton,SIGNAL(clicked()),this,SLOT(pauseLogButtonClicked()));
	connect(ui.stopLogPushButton,SIGNAL(clicked()),this,SLOT(stopLogButtonClicked()));

	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,50);
	ui.tableWidget->setRowCount(m_dataFieldList.size());
	for (int i=0;i<m_dataFieldList.size();i++)
	{
		ui.tableWidget->setItem(i,0,new QTableWidgetItem(m_dataFieldList[i].description()));
		ui.tableWidget->setItem(i,1,new QTableWidgetItem("0"));
	}

	logLoader = new LogLoader(this);
	connect(logLoader,SIGNAL(endOfLog()),this,SLOT(logFinished()));
	connect(logLoader,SIGNAL(payloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(logLoader,SIGNAL(logProgress(qlonglong,qlonglong)),this,SLOT(logProgress(qlonglong,qlonglong)));

	emsComms = new FreeEmsComms(this);
	connect(emsComms,SIGNAL(payloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	//logLoader->start();
	//410,170
	widget = new GaugeWidget(this);
	widget->setGeometry(410,170,1200,600);
	widget->show();
	pidcount = 0;
	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
	timer->start(1000);

	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(100);
}
void MainWindow::timerTick()
{
	ui.label_3->setText("PPS: " + QString::number(pidcount));
	pidcount = 0;
}

void MainWindow::loadLogButtonClicked()
{
	QFileDialog file;
	if (file.exec())
	{
		if (file.selectedFiles().size() > 0)
		{
			QString filename = file.selectedFiles()[0];
			ui.statusLabel->setText("Status: File loaded and not playing");
			logLoader->loadFile(filename);

		}
	}
}
void MainWindow::logFinished()
{
	ui.statusLabel->setText("Status: File loaded and log finished");
}

void MainWindow::playLogButtonClicked()
{
	logLoader->start();
	ui.statusLabel->setText("Status: File loaded and playing");
}

void MainWindow::pauseLogButtonClicked()
{

}

void MainWindow::stopLogButtonClicked()
{

}
void MainWindow::connectButtonClicked()
{
	emsComms->setPort(ui.portNameLineEdit->text());
	emsComms->setBaud(ui.baudRateLineEdit->text().toInt());
	emsComms->start();
}

void MainWindow::logProgress(qlonglong current,qlonglong total)
{
	setWindowTitle(QString::number(current) + "/" + QString::number(total) + " - " + QString::number((float)current/(float)total));
}
void MainWindow::guiUpdateTimerTick()
{
	QMap<QString,double>::const_iterator i = m_valueMap.constBegin();
	while (i != m_valueMap.constEnd())
	{
		widget->propertyMap.setProperty(i.key().toAscii(),QVariant::fromValue(i.value()));
		i++;
	}
}

void MainWindow::logPayloadReceived(QByteArray header,QByteArray payload)
{
	pidcount++;
	if (payload.length() != 96)
	{
		//Wrong sized payload!
	//	return;
	}
	for (int i=0;i<m_dataFieldList.size();i++)
	{
		double value = m_dataFieldList[i].getValue(&payload);
		m_valueMap[m_dataFieldList[i].name()] = value;
		ui.tableWidget->item(i,1)->setText(QString::number(value));
		/*if (m_dataFieldList[i].name() == "RPM")
		{
			widget->propertyMap.setProperty("0105",m_dataFieldList[i].getValue(&payload));
		}
		else if (m_dataFieldList[i].name() == "Advance")
		{
			widget->propertyMap.setProperty("0106",m_dataFieldList[i].getValue(&payload));
		}
		else if (m_dataFieldList[i].name() == "EffectivePW")
		{
			widget->propertyMap.setProperty("0107",m_dataFieldList[i].getValue(&payload));

		}
		else if (m_dataFieldList[i].name() == "IAT")
		{
			double value =  (m_dataFieldList[i].getValue(&payload) * 5/9) + 459.76;
			widget->propertyMap.setProperty("0108",value);
		}
		else if (m_dataFieldList[i].name() == "EGO")
		{
			widget->propertyMap.setProperty("0109",m_dataFieldList[i].getValue(&payload));
		}
		else if (m_dataFieldList[i].name() == "MAP")
		{
			widget->propertyMap.setProperty("0110",m_dataFieldList[i].getValue(&payload));
		}*/

		//qDebug() << "Length:" << payload.length();
//		qDebug() << "Updating:" << m_dataFieldList[i].name() << m_dataFieldList[i].getValue(&payload);
		//
	}

}

MainWindow::~MainWindow()
{
}
