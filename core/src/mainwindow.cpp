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

#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "datafield.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSettings>
#include <tableview2d.h>
#include <qjson/parser.h>
#include "logloader.h"
//#include "freeems/freeemscomms.h"
//#include "freeems/fedatapacketdecoder.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	m_offlineMode = false;
	m_checkEmsDataInUse = false;
	m_currentEcuClock = -1;
	qRegisterMetaType<MemoryLocationInfo>("MemoryLocationInfo");
	qRegisterMetaType<DataType>("DataType");
	qRegisterMetaType<SerialPortStatus>("SerialPortStatus");
	qDebug() << "EMStudio commit:" << define2string(GIT_COMMIT);
	qDebug() << "Full hash:" << define2string(GIT_HASH);
	progressView=0;
	emsComms=0;
	m_interrogationInProgress = false;
	m_debugLogs = false;

	emsData = new EmsData();
	connect(emsData,SIGNAL(updateRequired(unsigned short)),this,SLOT(updateDataWindows(unsigned short)));

	//Create this, even though we only need it during offline to online transitions.
	checkEmsData = new EmsData();

	m_settingsFile = "settings.ini";
	//TODO: Figure out proper directory names
#ifdef Q_OS_WIN32
	QString appDataDir = getenv("%AppData%");
	appDataDir = appDataDir.replace("\\","/");
	if (!QDir(appDataDir).exists("EMStudio"))
	{
		QDir(appDataDir).mkpath("EMStudio");
	}
	m_defaultsDir = QApplication::instance()->applicationDirPath();

	m_settingsDir = appDataDir + "/" + "EMStudio";
	//%HOMEPATH%//m_localHomeDir
	m_localHomeDir = QString(getenv("%USERPROFILE%")).replace("\\","/") + "/EMStudio";
	//m_settingsFile = appDataDir + "/" + "EMStudio/EMStudio-config.ini";
//#elif Q_OS_MAC //<- Does not exist. Need OSX checking capabilities somewhere...
	//Linux and Mac function identically here for now...
#else //if Q_OS_LINUX
	QString appDataDir = getenv("HOME");
	if (!QDir(appDataDir).exists(".EMStudio"))
	{
		QDir(appDataDir).mkpath(".EMStudio");
	}
	m_defaultsDir = "/usr/share/EMStudio";
	m_settingsDir = appDataDir + "/" + ".EMStudio";
	m_localHomeDir = appDataDir + "/" + "EMStudio";
	//m_settingsFile = appDataDir + "/" + ".EMStudio/EMStudio-config.ini";
#endif
	if (!QFile::exists(m_localHomeDir + "/logs"))
	{
		QDir(m_localHomeDir).mkpath(m_localHomeDir + "/logs");
	}
	//Settings, then defaults, then fallback to local
	if (QFile::exists(m_settingsDir + "/EMStudio-config.ini"))
	{
		m_settingsFile = m_settingsDir + "/EMStudio-config.ini";
	}
	else if (QFile::exists(m_defaultsDir + "/EMStudio-config.ini"))
	{
		m_settingsFile = m_defaultsDir + "/EMStudio-config.ini";
	}
	else if (QFile::exists("EMStudio-config.ini"))
	{
		m_settingsFile = "EMStudio-config.ini";
	}

	//Settings, then defaults, then fallback to local

	QString decoderfilestr = "";
	if (QFile::exists(m_settingsDir + "/" + "definitions/decodersettings.json"))
	{
		decoderfilestr = m_settingsDir + "/" + "definitions/decodersettings.json";
	}
	else if (QFile::exists(m_defaultsDir + "/definitions/decodersettings.json"))
	{
		decoderfilestr = m_defaultsDir + "/definitions/decodersettings.json";
	}
	else if (QFile::exists("decodersettings.json"))
	{
		decoderfilestr = "decodersettings.json";
	}
	else
	{
		//QMessageBox::information(0,"Error","Error: No freeems.config.json file found!");
	}
	if (QFile::exists(decoderfilestr))
	{
		QFile decoderfile(decoderfilestr);
		decoderfile.open(QIODevice::ReadOnly);
		QByteArray decoderfilebytes = decoderfile.readAll();
		decoderfile.close();

		QJson::Parser decoderparser;
		QVariant decodertop = decoderparser.parse(decoderfilebytes);
		if (!decodertop.isValid())
		{
			QString errormsg = QString("Error parsing JSON from config file on line number: ") + QString::number(decoderparser.errorLine()) + " error text: " + decoderparser.errorString();
			QMessageBox::information(0,"Error",errormsg);
			qDebug() << "Error parsing JSON";
			qDebug() << "Line number:" << decoderparser.errorLine() << "error text:" << decoderparser.errorString();
			//return;
		}
		else
		{
			QVariantMap decodertopmap = decodertop.toMap();
			QVariantMap decoderlocationmap = decodertopmap["locations"].toMap();
			QString str = decoderlocationmap["locationid"].toString();
			bool ok = false;
			unsigned short locid = str.toInt(&ok,16);
			QVariantList decodervalueslist = decoderlocationmap["values"].toList();
			QList<ConfigBlock> blocklist;
			for (int i=0;i<decodervalueslist.size();i++)
			{
				QVariantMap tmpmap = decodervalueslist[i].toMap();
				ConfigBlock block;
				block.setLocationId(locid);
				block.setName(tmpmap["name"].toString());
				block.setType(tmpmap["type"].toString());
				block.setElementSize(tmpmap["sizeofelement"].toInt());
				block.setSize(tmpmap["size"].toInt());
				block.setOffset(tmpmap["offset"].toInt());
				block.setSizeOverride(tmpmap["sizeoverride"].toString());
				block.setSizeOverrideMult(tmpmap["sizeoverridemult"].toDouble());
				QList<QPair<QString, double> > calclist;
				QVariantList calcliststr = tmpmap["calc"].toList();
				for (int j=0;j<calcliststr.size();j++)
				{
					qDebug() << "XCalc:" << calcliststr[j].toMap()["type"].toString() << calcliststr[j].toMap()["value"].toDouble();
					calclist.append(QPair<QString,double>(calcliststr[j].toMap()["type"].toString(),calcliststr[j].toMap()["value"].toDouble()));
				}
				block.setCalc(calclist);
				blocklist.append(block);
			}
			m_configBlockMap[locid] = blocklist;
		}
	}

	//return;
	m_currentRamLocationId=0;
	//populateDataFields();
	m_localRamDirty = false;
	m_deviceFlashDirty = false;
	m_waitingForRamWriteConfirmation = false;
	m_waitingForFlashWriteConfirmation = false;
	ui.setupUi(this);
	connect(ui.actionSave_Offline_Data,SIGNAL(triggered()),this,SLOT(menu_file_saveOfflineDataClicked()));
	connect(ui.actionEMS_Status,SIGNAL(triggered()),this,SLOT(menu_windows_EmsStatusClicked()));
	connect(ui.actionLoad_Offline_Data,SIGNAL(triggered()),this,SLOT(menu_file_loadOfflineDataClicked()));
	this->setWindowTitle(QString("EMStudio ") + QString(define2string(GIT_COMMIT)));
	emsinfo.emstudioCommit = define2string(GIT_COMMIT);
	emsinfo.emstudioHash = define2string(GIT_HASH);
	ui.actionDisconnect->setEnabled(false);
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_connectClicked()));
	connect(ui.actionDisconnect,SIGNAL(triggered()),this,SLOT(menu_disconnectClicked()));
	connect(ui.actionEMS_Info,SIGNAL(triggered()),this,SLOT(menu_windows_EmsInfoClicked()));
	connect(ui.actionGauges,SIGNAL(triggered()),this,SLOT(menu_windows_GaugesClicked()));
	connect(ui.actionTables,SIGNAL(triggered()),this,SLOT(menu_windows_TablesClicked()));
	connect(ui.actionFlags,SIGNAL(triggered()),this,SLOT(menu_windows_FlagsClicked()));
	connect(ui.actionInterrogation_Progress,SIGNAL(triggered()),this,SLOT(menu_windows_interrogateProgressViewClicked()));
	connect(ui.actionExit_3,SIGNAL(triggered()),this,SLOT(close()));
	connect(ui.actionPacket_Status,SIGNAL(triggered()),this,SLOT(menu_windows_PacketStatusClicked()));
	connect(ui.actionAbout,SIGNAL(triggered()),this,SLOT(menu_aboutClicked()));
	connect(ui.actionEnable_Datalogs,SIGNAL(triggered()),this,SLOT(menu_enableDatalogsClicked()));
	connect(ui.actionDisable_Datalog_Stream,SIGNAL(triggered()),this,SLOT(menu_disableDatalogsClicked()));
	ui.actionInterrogation_Progress->setEnabled(false);

	emsInfo=0;
	dataTables=0;
	dataFlags=0;
	dataGauges=0;
	pluginLoader = new QPluginLoader(this);
	if (QFile::exists("plugins/libfreeemsplugin.so"))
	{
		m_pluginFileName = "plugins/libfreeemsplugin.so";
	}
	else if (QFile::exists("/usr/share/emstudio/plugins/libfreeemsplugin.so"))
	{
		m_pluginFileName = "/usr/share/emstudio/plugins/libfreeemsplugin.so";
	}
	else if (QFile::exists("plugins/freeemsplugin.lib"))
	{
		m_pluginFileName = "plugins/freeemsplugin.lib";
	}
	qDebug() << "Loading plugin from:" << m_pluginFileName;
	//m_pluginFileName = "plugins/libmsplugin.so";
	//m_pluginFileName = "plugins/libo5eplugin.so";

	/*pluginLoader->setFileName(m_pluginFileName);
	//loader->setFileName("plugins/libmsplugin.so");
	if (!pluginLoader->load())
	{
		qDebug() << "Unable to load plugin. error:" << pluginLoader->errorString();
		exit(-1);
	}


	emsComms = qobject_cast<EmsComms*>(pluginLoader->instance());
	if (!emsComms)
	{
		qDebug() << "Unable to load plugin!!!";
		qDebug() << pluginLoader->errorString();
		exit(-1);
	}

	dataPacketDecoder = emsComms->getDecoder();
	connect(dataPacketDecoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));



	emsComms->setLogFileName(m_logFileName);*/
	m_logFileName = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");

	//connect(emsComms,SIGNAL(error(QString)),this,SLOT(error(QString)));
	/*connect(emsComms,SIGNAL(error(SerialPortStatus,QString)),this,SLOT(error(SerialPortStatus,QString)));
	connect(emsComms,SIGNAL(commandTimedOut(int)),this,SLOT(commandTimedOut(int)));
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(disconnected()),this,SLOT(emsCommsDisconnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(firmwareVersion(QString)),this,SLOT(firmwareVersion(QString)));
	connect(emsComms,SIGNAL(compilerVersion(QString)),this,SLOT(emsCompilerVersion(QString)));
	connect(emsComms,SIGNAL(interfaceVersion(QString)),this,SLOT(interfaceVersion(QString)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)),Qt::QueuedConnection);
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,MemoryLocationInfo)),this,SLOT(locationIdInfo(unsigned short,MemoryLocationInfo)));
	connect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(ramBlockUpdate(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(flashBlockUpdate(unsigned short,QByteArray,QByteArray)));

	connect(emsComms,SIGNAL(decoderName(QString)),this,SLOT(emsDecoderName(QString)));
	connect(emsComms,SIGNAL(operatingSystem(QString)),this,SLOT(emsOperatingSystem(QString)));
	connect(emsComms,SIGNAL(firmwareBuild(QString)),this,SLOT(emsFirmwareBuildDate(QString)));*/
	emsData->setInterrogation(true);


	emsInfo = new EmsInfoView();
	emsInfo->setFirmwareVersion(m_firmwareVersion);
	emsInfo->setInterfaceVersion(m_interfaceVersion);
	connect(emsInfo,SIGNAL(displayLocationId(int,bool,DataType)),this,SLOT(emsInfoDisplayLocationId(int,bool,DataType)));

	emsMdiWindow = ui.mdiArea->addSubWindow(emsInfo);
	emsMdiWindow->setGeometry(emsInfo->geometry());
	emsMdiWindow->hide();
	emsMdiWindow->setWindowTitle(emsInfo->windowTitle());

	aboutView = new AboutView();
	aboutView->setHash(define2string(GIT_HASH));
	aboutView->setCommit(define2string(GIT_COMMIT));
	aboutMdiWindow = ui.mdiArea->addSubWindow(aboutView);
	aboutMdiWindow->setGeometry(aboutView->geometry());
	aboutMdiWindow->hide();
	aboutMdiWindow->setWindowTitle(aboutView->windowTitle());

	dataGauges = new GaugeView();
	//connect(dataGauges,SIGNAL(destroyed()),this,SLOT(dataGaugesDestroyed()));
	gaugesMdiWindow = ui.mdiArea->addSubWindow(dataGauges);
	gaugesMdiWindow->setGeometry(dataGauges->geometry());
	gaugesMdiWindow->hide();
	gaugesMdiWindow->setWindowTitle(dataGauges->windowTitle());

	if (QFile::exists(m_defaultsDir + "/" + "dashboards/gauges.qml"))
	{
		//qml file is in the program files directory, or in /usr/share
		dataGauges->setFile(m_defaultsDir + "/" + "dashboards/gauges.qml");
	}
	else if (QFile::exists("src/gauges.qml"))
	{
		//We're operating out of the src directory
		dataGauges->setFile("src/gauges.qml");
	}
	else if (QFile::exists("gauges.qml"))
	{
		//Running with no install, but not src?? Still handle it.
		dataGauges->setFile("gauges.qml");
	}

	dataTables = new TableView();
	//connect(dataTables,SIGNAL(destroyed()),this,SLOT(dataTablesDestroyed()));
	//dataTables->passDecoder(dataPacketDecoder);
	tablesMdiWindow = ui.mdiArea->addSubWindow(dataTables);
	tablesMdiWindow->setGeometry(dataTables->geometry());
	tablesMdiWindow->hide();
	tablesMdiWindow->setWindowTitle(dataTables->windowTitle());

	statusView = new EmsStatus(this);
	this->addDockWidget(Qt::RightDockWidgetArea,statusView);
	connect(statusView,SIGNAL(hardResetRequest()),this,SLOT(emsStatusHardResetRequested()));
	connect(statusView,SIGNAL(softResetRequest()),this,SLOT(emsStatusSoftResetRequested()));

	dataFlags = new FlagView();
	dataFlags->passDecoder(dataPacketDecoder);
	flagsMdiWindow = ui.mdiArea->addSubWindow(dataFlags);
	flagsMdiWindow->setGeometry(dataFlags->geometry());
	flagsMdiWindow->hide();
	flagsMdiWindow->setWindowTitle(dataFlags->windowTitle());

	packetStatus = new PacketStatusView();
	/*connect(emsComms,SIGNAL(packetSent(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketSent(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetAcked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketAck(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray,unsigned short)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray,unsigned short)));
	connect(emsComms,SIGNAL(decoderFailure(QByteArray)),packetStatus,SLOT(passDecoderFailure(QByteArray)));*/
	packetStatusMdiWindow = ui.mdiArea->addSubWindow(packetStatus);
	packetStatusMdiWindow->setGeometry(packetStatus->geometry());
	packetStatusMdiWindow->hide();
	packetStatusMdiWindow->setWindowTitle(packetStatus->windowTitle());

	//Load settings
	qDebug() << "Local settings file is:" << m_settingsFile;
	QSettings settings(m_settingsFile,QSettings::IniFormat);
	settings.beginGroup("comms");
	m_comPort = settings.value("port","/dev/ttyUSB0").toString();
	m_comBaud = settings.value("baud",115200).toInt();
	m_comInterByte = settings.value("interbytedelay",0).toInt();
	m_saveLogs = settings.value("savelogs",true).toBool();
	m_clearLogs = settings.value("clearlogs",false).toBool();
	m_logsToKeep = settings.value("logstokeep",0).toInt();
	//emsComms->setLogFileName(m_localHomeDir + "/logs/" + m_logFileName); m_localHomeDir
	m_logDirectory = settings.value("logdir",m_localHomeDir + "/logs").toString();
	m_debugLogs = settings.value("debuglogs",false).toBool();
	settings.endGroup();

	/*emsComms->setBaud(m_comBaud);
	emsComms->setPort(m_comPort);
	emsComms->setLogDirectory(m_logDirectory);
	emsComms->setLogsEnabled(m_saveLogs);
	emsComms->setInterByteSendDelay(m_comInterByte);
	emsComms->setlogsDebugEnabled(m_debugLogs);*/

	pidcount = 0;

	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
	timer->start(1000);

	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(250);

	statusBar()->addWidget(ui.ppsLabel);
	statusBar()->addWidget(ui.statusLabel);


	logfile = new QFile("myoutput.log");
	logfile->open(QIODevice::ReadWrite | QIODevice::Truncate);
}
void MainWindow::menu_windows_interrogateProgressViewClicked()
{
	interrogateProgressMdiWindow->show();
	QApplication::postEvent(interrogateProgressMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(interrogateProgressMdiWindow, new QEvent(QEvent::WindowActivate));
}

void MainWindow::menu_file_saveOfflineDataClicked()
{

	QString filename = QFileDialog::getSaveFileName(this,"Save Offline File",".","Offline JSON Files (*.json)");
	if (filename == "")
	{
		return;
	}
	QVariantMap top;
	QVariantMap flashMap;
	QVariantMap metaMap;
	QVariantMap ramMap;
	QMap<unsigned short,MemoryLocationInfo>::const_iterator i = m_memoryInfoMap.constBegin();
	while (i != m_memoryInfoMap.constEnd())
	{
		QVariantMap id;
		id["id"] = QString::number((unsigned short)i.key(),16).toUpper();
		id["flags"] = QString::number((unsigned short)i.value().rawflags,16).toUpper();
		id["rampage"] = QString::number((unsigned char)i.value().rampage,16).toUpper();
		id["flashpage"] = QString::number((unsigned char)i.value().flashpage,16).toUpper();
		id["ramaddress"] = QString::number((unsigned short)i.value().ramaddress,16).toUpper();
		id["flashaddress"] = QString::number((unsigned short)i.value().flashaddress,16).toUpper();
		id["parent"] = QString::number((unsigned short)i.value().parent,16).toUpper();
		id["hasparent"] = i.value().hasParent;
		id["isram"] = i.value().isRam;
		id["isflash"] = i.value().isFlash;
		id["size"] = QString::number((unsigned short)i.value().size,16).toUpper();
		if (i.value().type == DATA_TABLE_2D)
		{
			id["type"] = "DATA_TABLE_2D";
		}
		else if (i.value().type == DATA_TABLE_3D)
		{
			id["type"] = "DATA_TABLE_3D";
		}
		else if (i.value().type == DATA_TABLE_LOOKUP)
		{
			id["type"] = "DATA_TABLE_LOOKUP";
		}
		else if (i.value().type == DATA_CONFIG)
		{
			id["type"] = "DATA_CONFIG";
		}
		else if (i.value().type == DATA_UNDEFINED)
		{
			id["type"] = "DATA_UNDEFINED";
		}
		QVariantMap properties;
		for (int j=0;j<i.value().propertymap.size();j++)
		{
			properties[i.value().propertymap[j].first] = i.value().propertymap[j].second;
		}
		id["properties"] = properties;
		metaMap[QString::number((unsigned short)i.key(),16).toUpper()] = id;
		if (!i.value().hasParent)
		{
			if (i.value().isFlash)
			{
				QString val = emsData->serialize(i.key(),false);
				flashMap[QString::number((unsigned short)i.key(),16).toUpper()] = val;
			}
			if (i.value().isRam)
			{
				QString val = emsData->serialize(i.key(),true);
				ramMap[QString::number((unsigned short)i.key(),16).toUpper()] = val;
			}
		}
		i++;
	}
	top["ram"] = ramMap;
	top["flash"] = flashMap;
	top["meta"] = metaMap;
	QJson::Serializer serializer;
	QByteArray out = serializer.serialize(top);
	QFile outfile(filename);
	outfile.open(QIODevice::ReadWrite | QIODevice::Truncate);
	outfile.write(out);
	outfile.flush();
	outfile.close();
}

void MainWindow::menu_file_loadOfflineDataClicked()
{

	QString filename = QFileDialog::getOpenFileName(this,"Load Offline File",".","Offline JSON Files (*.json)");
	if (filename == "")
	{
		qDebug() << "No offline file selected!";
		return;
	}
	//QByteArray out = serializer.serialize(top);
	QFile outfile(filename);
	outfile.open(QIODevice::ReadOnly);
	QByteArray out = outfile.readAll();
	outfile.close();

	QJson::Parser parser;
	bool ok = false;
	QVariant outputvar = parser.parse(out,&ok);
	if (!ok)
	{
		qDebug() << "Error parsing json:" << parser.errorString();
		return;
	}
	m_offlineMode = true;
	QVariantMap top = outputvar.toMap();
	QVariantMap ramMap = top["ram"].toMap();
	QVariantMap flashMap = top["flash"].toMap();
	QVariantMap metaMap = top["meta"].toMap();
	QVariantMap::const_iterator i = metaMap.constBegin();

    if (m_memoryInfoMap.keys().size() > 0)
    {
        //We already have memory in place! Likely loading ontop of an already loaded set of data.
        //Verify that meta data is equal.
        while (i != metaMap.constEnd())
        {
            unsigned short locationid = i.key().toInt(&ok,16);
            bool hasParent = i.value().toMap()["hasparent"].toBool();
            bool isFlash = i.value().toMap()["isflash"].toBool();
            bool isRam = i.value().toMap()["isram"].toBool();
            //m_tempMemoryList.append(loc);
            //unsigned short flags =i.value().toMap()["flags"].toString().toInt(&ok,16);
            unsigned char rampage = i.value().toMap()["rampage"].toString().toInt(&ok,16);
            unsigned short ramaddress = i.value().toMap()["ramaddress"].toString().toInt(&ok,16);
            unsigned char flashpage = i.value().toMap()["flashpage"].toString().toInt(&ok,16);
            unsigned short flashaddress = i.value().toMap()["flags"].toString().toInt(&ok,16);
            unsigned short parent = i.value().toMap()["parent"].toString().toInt(&ok,16);
            unsigned short size = i.value().toMap()["size"].toString().toInt(&ok,16);
            //unsigned short id = i.key().toInt(&ok,16);
            QString type = i.value().toMap()["type"].toString();
            if (m_memoryInfoMap.contains(locationid))
            {
                if (m_memoryInfoMap[locationid].size != size)
                {
                    QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " size in tuner is " +
                                             QString::number(m_memoryInfoMap[locationid].size) + " but json has size of " +  QString::number(size) +
                                             ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                    return;
                }
                if (isFlash == m_memoryInfoMap[locationid].isFlash && isFlash)
                {
                    if (flashaddress != m_memoryInfoMap[locationid].flashaddress)
                    {
                        QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " flash address mismatch. Tuner flash address is 0x" +
                                                 QString::number(m_memoryInfoMap[locationid].flashaddress,16).toUpper() + " but json has flash address of 0x" + QString::number(flashaddress,16).toUpper() +
                                                 ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                        return;
                    }
                    if (flashpage != m_memoryInfoMap[locationid].flashpage)
                    {
                        QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " flash page mismatch. Tuner flash page is 0x" +
                                                 QString::number(m_memoryInfoMap[locationid].flashpage,16).toUpper() + " but json has flash page of 0x" + QString::number(flashpage,16).toUpper() +
                                                 ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                        return;
                    }
                }
                else
                {
                    QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " flash mismatch. Tuner isFlash value is " +
                                             (m_memoryInfoMap[locationid].isFlash ? "true" : "false") + " but json has isFlash value of " + (isFlash ? "true" : "false") +
                                             ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                    return;
                }
                if (isRam == m_memoryInfoMap[locationid].isRam && isRam)
                {
                    if (ramaddress != m_memoryInfoMap[locationid].ramaddress)
                    {
                        QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " ram address mismatch. Tuner ram address is 0x" +
                                                 QString::number(m_memoryInfoMap[locationid].ramaddress,16).toUpper() + " but json has ram address of 0x" + QString::number(ramaddress,16).toUpper() +
                                                 ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                        return;
                    }
                    if (rampage != m_memoryInfoMap[locationid].rampage)
                    {
                        QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " ram page mismatch. Tuner ram page is 0x" +
                                                 QString::number(m_memoryInfoMap[locationid].rampage,16).toUpper() + " but json has ram page of 0x" + QString::number(rampage,16).toUpper() +
                                                 ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                        return;
                    }
                }
                else
                {
                    QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " ram mismatch. Tuner isRam value is " +
                                             (m_memoryInfoMap[locationid].isRam ? "true" : "false") + " but json has isRam value of " + (isRam ? "true" : "false") +
                                             ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                    return;
                }
                if (hasParent == m_memoryInfoMap[locationid].hasParent)
                {
                     if (parent != m_memoryInfoMap[locationid].parent)
                    {
                        QMessageBox::information(0,"Error","Error loading JSON. Location ID 0x" + QString::number(locationid,16).toUpper() + " parent mismatch. Tuner parent is 0x" +
                                                 QString::number(m_memoryInfoMap[locationid].parent,16).toUpper() + " but json has parent of 0x" + QString::number(parent,16).toUpper() +
                                                 ". Unable to finish loading JSON file. Please check your JSON file to ensure it is not corrupt");
                        return;
                    }
                }
            }
            i++;
        }
        //If we get to here, then the metadata all matches and is happy.
	if (QMessageBox::question(0,"Question","You are about to totally wipe out your FLASH data and replace it with the data in this JSON file. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) != QMessageBox::Yes)
	{
	    return;
	}
        i = flashMap.constBegin();
        while (i != flashMap.constEnd())
        {
            bool ok = false;
            //qDebug() << "Flash location" << i.key().toInt(&ok,16);
            QString val = i.value().toString();
            QStringList valsplit = val.split(",");
            QByteArray bytes;
            for (int j=0;j<valsplit.size();j++)
            {
                bytes.append(valsplit[j].toInt(&ok,16));
            }
            //interrogateFlashBlockRetrieved(i.key().toInt(&ok,16),QByteArray(),bytes);
            emsData->flashBlockUpdate(i.key().toInt(&ok,16),QByteArray(),bytes);
	    emsComms->updateBlockInFlash(i.key().toInt(&ok,16),0,0,bytes);
            i++;
        }
	if (QMessageBox::question(0,"Question","Flash memory has been replaced. Would you like to reset your ECU now to apply the changes to ram?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
	    emsComms->hardReset();
	}
        return;
    }
    i = metaMap.constBegin();
	while (i != metaMap.constEnd())
	{

		bool ok;

		MemoryLocationInfo info;
		//MemoryLocation *loc = new MemoryLocation();
		//loc->locationid = i.key().toInt();
		info.locationid = i.key().toInt(&ok,16);
		info.hasParent = i.value().toMap()["hasparent"].toBool();
		info.isFlash = i.value().toMap()["isflash"].toBool();
		info.isRam = i.value().toMap()["isram"].toBool();
		//m_tempMemoryList.append(loc);
		unsigned short flags =i.value().toMap()["flags"].toString().toInt(&ok,16);
		unsigned char rampage = i.value().toMap()["rampage"].toString().toInt(&ok,16);
		unsigned short ramaddress = i.value().toMap()["ramaddress"].toString().toInt(&ok,16);
		unsigned char flashpage = i.value().toMap()["flashpage"].toString().toInt(&ok,16);
		unsigned short flashaddress = i.value().toMap()["flags"].toString().toInt(&ok,16);
		unsigned short parent = i.value().toMap()["parent"].toString().toInt(&ok,16);
		unsigned short size = i.value().toMap()["size"].toString().toInt(&ok,16);
		unsigned short id = i.key().toInt(&ok,16);
		QString type = i.value().toMap()["type"].toString();

		info.rampage = rampage;
		info.parent = parent;
		info.flashpage = flashpage;
		info.ramaddress = ramaddress;
		info.flashaddress = flashaddress;
		info.size = size;
		info.rawflags = flags;
		if (type == "DATA_TABLE_2D")
		{
			info.type = DATA_TABLE_2D;
		}
		else if (type == "DATA_TABLE_3D")
		{
			info.type = DATA_TABLE_3D;
		}
		else if (type == "DATA_TABLE_LOOKUP")
		{
			info.type = DATA_TABLE_LOOKUP;
		}
		else if (type == "DATA_CONFIG")
		{
			info.type = DATA_CONFIG;
		}
		else if (type == "DATA_UNDEFINED")
		{
			info.type = DATA_UNDEFINED;
		}
		else
		{
			info.type = DATA_UNDEFINED;
		}
		QVariantMap properties = i.value().toMap()["properties"].toMap();
		for (QVariantMap::const_iterator j=properties.constBegin();j!=properties.constEnd();j++)
		{
			info.propertymap.append(QPair<QString,QString>(j.key(),j.value().toString()));
		}
		if (info.isFlash)
		{
			locationIdInfo(id,info);
		}

		//locationIdInfo(id,flags,nflags,parent,rampage,flashpage,ramaddress,flashaddress,size);
		i++;
	}
	populateParentLists();

	emsData->setInterrogation(true);

	i = ramMap.constBegin();
	while (i != ramMap.constEnd())
	{
		bool ok = false;
		//ramBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload)
		QString val = i.value().toString();
		QStringList valsplit = val.split(",");
		QByteArray bytes;
		for (int j=0;j<valsplit.size();j++)
		{
			bytes.append(valsplit[j].toInt(&ok,16));
		}
		//interrogateRamBlockRetrieved(i.key().toInt(&ok,16),QByteArray(),bytes);
		i++;
	}

	i = flashMap.constBegin();
	while (i != flashMap.constEnd())
	{
		bool ok = false;
		//qDebug() << "Flash location" << i.key().toInt(&ok,16);
		QString val = i.value().toString();
		QStringList valsplit = val.split(",");
		QByteArray bytes;
		for (int j=0;j<valsplit.size();j++)
		{
			bytes.append(valsplit[j].toInt(&ok,16));
		}
		if (m_memoryInfoMap[i.key().toInt(&ok,16)].isRam)
		{
			//interrogateRamBlockRetrieved(i.key().toInt(&ok,16),QByteArray(),bytes);
			emsData->ramBlockUpdate(i.key().toInt(&ok,16),QByteArray(),bytes);
		}
		//interrogateFlashBlockRetrieved(i.key().toInt(&ok,16),QByteArray(),bytes);
		emsData->flashBlockUpdate(i.key().toInt(&ok,16),QByteArray(),bytes);
		i++;
	}
	checkRamFlashSync();
	emsMdiWindow->show();

	emsData->setInterrogation(false);
}
void MainWindow::emsCommsSilence()
{
	//This is called when the ems has been silent for 5 seconds, when it was previously talking.
	qDebug() << "EMS HAS GONE SILENT";
}

void MainWindow::emsCommsSilenceBroken()
{
	//This is called when ems had previously been talking, gone silent, then started talking again.
	qDebug() << "EMS HAS GONE NOISEY";
}

void MainWindow::emsCommsDisconnected()
{

	emsComms->stop();
	emsComms->disconnect();
	emsComms->terminate();
	emsComms->wait(250); //Join it, fixes a race condition where the thread deletes before it's finished.
	emsComms->deleteLater();
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
	emsComms = 0;
	qDebug() << "emsCommsDisconnected, resetting and reloading plugin";

	//Need to reset everything here.
	pluginLoader->unload();
	//pluginLoader->deleteLater();
	delete pluginLoader;
	pluginLoader = 0;
	pluginLoader = new QPluginLoader(this);
	pluginLoader->setFileName(m_pluginFileName);
	if (!pluginLoader->load())
	{
		qDebug() << "Unable to load plugin. " << m_pluginFileName << "error:" << pluginLoader->errorString();
		exit(-1);
	}


	emsComms = qobject_cast<EmsComms*>(pluginLoader->instance());
	if (!emsComms)
	{
		qDebug() << "Unable to instantiate plugin:" << m_pluginFileName << pluginLoader->instance();
		qDebug() << pluginLoader->errorString();
		exit(-1);
	}

	dataPacketDecoder = emsComms->getDecoder();
	connect(dataPacketDecoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	QString filestr = "";
	if (QFile::exists(m_settingsDir + "/" + "definitions/freeems.config.json"))
	{
		filestr = m_settingsDir + "/" + "definitions/freeems.config.json";
	}
	else if (QFile::exists(m_defaultsDir + "/definitions/freeems.config.json"))
	{
		filestr = m_defaultsDir + "/definitions/freeems.config.json";
	}
	else if (QFile::exists("freeems.config.json"))
	{
		filestr = "freeems.config.json";
	}
	else
	{
		QMessageBox::information(0,"Error","Error: No freeems.config.json file found!");
	}
	m_memoryMetaData = emsComms->getMetaParser();
	m_memoryMetaData->loadMetaDataFromFile(filestr);
	emsData->setMetaData(m_memoryMetaData);
	qDebug() << m_memoryMetaData->errorMap().keys().size() << "Error Keys Loaded";
	qDebug() << m_memoryMetaData->table3DMetaData().size() << "3D Tables Loaded";
	qDebug() << m_memoryMetaData->table2DMetaData().size() << "2D Tables Loaded";


	m_logFileName = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");
	emsComms->setLogFileName(m_logFileName);
	emsComms->setLogDirectory(m_logDirectory);
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(emsSilenceStarted()),this,SLOT(emsCommsSilence()));
	connect(emsComms,SIGNAL(emsSilenceBroken()),this,SLOT(emsCommsSilenceBroken()));
	connect(emsComms,SIGNAL(error(QString)),this,SLOT(error(QString)));
	connect(emsComms,SIGNAL(error(SerialPortStatus,QString)),this,SLOT(error(SerialPortStatus,QString)));

	connect(emsComms,SIGNAL(disconnected()),this,SLOT(emsCommsDisconnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(firmwareVersion(QString)),this,SLOT(firmwareVersion(QString)));
	connect(emsComms,SIGNAL(decoderName(QString)),this,SLOT(emsDecoderName(QString)));
	connect(emsComms,SIGNAL(compilerVersion(QString)),this,SLOT(emsCompilerVersion(QString)));
	connect(emsComms,SIGNAL(interfaceVersion(QString)),this,SLOT(interfaceVersion(QString)));
	connect(emsComms,SIGNAL(operatingSystem(QString)),this,SLOT(emsOperatingSystem(QString)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)),Qt::QueuedConnection);
	connect(emsComms,SIGNAL(firmwareBuild(QString)),this,SLOT(emsFirmwareBuildDate(QString)));
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandTimedOut(int)),this,SLOT(commandTimedOut(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	//connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),this,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,MemoryLocationInfo)),this,SLOT(locationIdInfo(unsigned short,MemoryLocationInfo)));
	connect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(ramBlockUpdate(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(flashBlockUpdate(unsigned short,QByteArray,QByteArray)));
	emsData->setInterrogation(true);
	connect(emsComms,SIGNAL(packetSent(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketSent(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetAcked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketAck(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray,unsigned short)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray,unsigned short)));
	connect(emsComms,SIGNAL(decoderFailure(QByteArray)),packetStatus,SLOT(passDecoderFailure(QByteArray)));
	emsComms->setBaud(m_comBaud);
	emsComms->setPort(m_comPort);
	emsComms->setLogsEnabled(m_saveLogs);
	emsComms->setInterByteSendDelay(m_comInterByte);
	emsComms->setlogsDebugEnabled(m_debugLogs);

	ui.actionConnect->setEnabled(true);
	m_offlineMode = true;
}
void MainWindow::setPlugin(QString plugin)
{
	m_pluginFileName = plugin;
	if (emsComms)
	{
	emsComms->stop();
	emsComms->terminate();
	emsComms->wait(250); //Join it, fixes a race condition where the thread deletes before it's finished.
	emsComms->deleteLater();
	}
	pluginLoader->unload();
	pluginLoader->deleteLater();
	pluginLoader=0;
	pluginLoader = new QPluginLoader(this);
	pluginLoader->setFileName(m_pluginFileName);
	if (!pluginLoader->load())
	{
		qDebug() << "Unable to load plugin. error:" << pluginLoader->errorString();
		exit(-1);
	}
	emsComms = qobject_cast<EmsComms*>(pluginLoader->instance());
	if (!emsComms)
	{
		qDebug() << "Unable to load plugin!!!";
		qDebug() << pluginLoader->errorString();
		exit(-1);
	}
	QString filestr = "";
	if (QFile::exists(m_settingsDir + "/" + "definitions/freeems.config.json"))
	{
		filestr = m_settingsDir + "/" + "definitions/freeems.config.json";
	}
	else if (QFile::exists(m_defaultsDir + "/definitions/freeems.config.json"))
	{
		filestr = m_defaultsDir + "/definitions/freeems.config.json";
	}
	else if (QFile::exists("freeems.config.json"))
	{
		filestr = "freeems.config.json";
	}
	else
	{
		QMessageBox::information(0,"Error","Error: No freeems.config.json file found!");
	}
	m_memoryMetaData = emsComms->getMetaParser();
	m_memoryMetaData->loadMetaDataFromFile(filestr);
	emsData->setMetaData(m_memoryMetaData);
	qDebug() << m_memoryMetaData->errorMap().keys().size() << "Error Keys Loaded";
	qDebug() << m_memoryMetaData->table3DMetaData().size() << "3D Tables Loaded";
	qDebug() << m_memoryMetaData->table2DMetaData().size() << "2D Tables Loaded";
	dataPacketDecoder = emsComms->getDecoder();
	connect(dataPacketDecoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	dataTables->passDecoder(dataPacketDecoder);
	m_logFileName = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");
	emsComms->setLogFileName(m_logFileName);
	emsComms->setLogDirectory(m_logDirectory);
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(emsSilenceStarted()),this,SLOT(emsCommsSilence()));
	connect(emsComms,SIGNAL(emsSilenceBroken()),this,SLOT(emsCommsSilenceBroken()));
	connect(emsComms,SIGNAL(error(QString)),this,SLOT(error(QString)));
	connect(emsComms,SIGNAL(error(SerialPortStatus,QString)),this,SLOT(error(SerialPortStatus,QString)));

	connect(emsComms,SIGNAL(disconnected()),this,SLOT(emsCommsDisconnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(firmwareVersion(QString)),this,SLOT(firmwareVersion(QString)));
	connect(emsComms,SIGNAL(decoderName(QString)),this,SLOT(emsDecoderName(QString)));
	connect(emsComms,SIGNAL(compilerVersion(QString)),this,SLOT(emsCompilerVersion(QString)));
	connect(emsComms,SIGNAL(interfaceVersion(QString)),this,SLOT(interfaceVersion(QString)));
	connect(emsComms,SIGNAL(operatingSystem(QString)),this,SLOT(emsOperatingSystem(QString)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)),Qt::QueuedConnection);
	connect(emsComms,SIGNAL(firmwareBuild(QString)),this,SLOT(emsFirmwareBuildDate(QString)));
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandTimedOut(int)),this,SLOT(commandTimedOut(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	//connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),this,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,MemoryLocationInfo)),this,SLOT(locationIdInfo(unsigned short,MemoryLocationInfo)));
	connect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(ramBlockUpdate(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),emsData,SLOT(flashBlockUpdate(unsigned short,QByteArray,QByteArray)));
	emsData->setInterrogation(true);
	connect(emsComms,SIGNAL(packetSent(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketSent(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetAcked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketAck(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray,unsigned short)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray,unsigned short)));
	connect(emsComms,SIGNAL(decoderFailure(QByteArray)),packetStatus,SLOT(passDecoderFailure(QByteArray)));
	emsComms->setBaud(m_comBaud);
	emsComms->setPort(m_comPort);
	emsComms->setLogsEnabled(m_saveLogs);
	emsComms->setInterByteSendDelay(m_comInterByte);
	emsComms->setlogsDebugEnabled(m_debugLogs);
}
void MainWindow::menu_enableDatalogsClicked()
{
	if (emsComms)
	{
		emsComms->enableDatalogStream();
	}
}

void MainWindow::menu_disableDatalogsClicked()
{
	if (emsComms)
	{
		emsComms->disableDatalogStream();
	}
}

void MainWindow::setDevice(QString dev)
{
	m_comPort = dev;
	if (emsComms)
	{
		emsComms->setPort(dev);
	}
}

void MainWindow::connectToEms()
{
	menu_connectClicked();
}
void MainWindow::tableview3d_reloadTableData(unsigned short locationid,bool ram)
{
	if (ram)
	{
		emsComms->retrieveBlockFromRam(locationid,0,0);
	}
	else
	{
		if (emsData->hasLocalFlashBlock(locationid))
		{
			TableView3D *table = qobject_cast<TableView3D*>(sender());
			if (table)
			{
				table->setData(locationid,emsData->getLocalFlashBlock(locationid),emsComms->getNew3DTableData());
				emsData->setLocalRamBlock(locationid,emsData->getLocalFlashBlock(locationid));
				emsComms->updateBlockInRam(locationid,0,emsData->getLocalFlashBlock(locationid).size(),emsData->getLocalFlashBlock(locationid));
				emsData->setDeviceRamBlock(locationid,emsData->getLocalFlashBlock(locationid));
			}
			else
			{
				qDebug() << "Unable to find TableView3D to pass data to";
			}
		}
	}
}
void MainWindow::reloadDataFromDevice(unsigned short locationid,bool isram)
{
	if (isram)
	{
		if (m_offlineMode)
		{
			QMessageBox::information(0,"Error","Unable to retrieve from ram during offline mode. Please connect to the ECU for this functionality");
		}
		else
		{
			emsComms->retrieveBlockFromRam(locationid,0,0);
		}
	}
	else
	{
		DataView *view = qobject_cast<DataView*>(sender());
		if (!view)
		{
			qDebug() << "Error, unable to cast sender of reloadDataFromDevice() to DataView*";
			return;
		}
		if (emsData->hasLocalRamBlock(locationid))
		{
			if (m_memoryMetaData->has2DMetaData(locationid))
			{
				view->setData(locationid,emsData->getLocalFlashBlock(locationid),emsComms->getNew2DTableData());
			}
			else if (m_memoryMetaData->has3DMetaData(locationid))
			{
				view->setData(locationid,emsData->getLocalFlashBlock(locationid),emsComms->getNew3DTableData());
			}
			else
			{
				qDebug() << "ERROR! Table location id update requested with no valid 2d/3d meta data";
			}
			if (!m_offlineMode)
			{
				emsComms->updateBlockInRam(locationid,0,emsData->getLocalFlashBlock(locationid).size(),emsData->getLocalFlashBlock(locationid));
				emsComms->retrieveBlockFromFlash(locationid,0,0);
			}
			emsData->setLocalRamBlock(locationid,emsData->getLocalFlashBlock(locationid));
		}
		else if (emsData->hasLocalFlashBlock(locationid))
		{
			if (m_offlineMode)
			{
				QMessageBox::information(0,"Error","Unable to retrieve block from flash during offline mode. Please connect to the ECU for this functionality");
			}
			else
			{
				emsComms->retrieveBlockFromFlash(locationid,0,0);
			}
		}
		else
		{
			qDebug() << "Local flash block does not exist! This should never happen";
			return;
		}
	}
}

void MainWindow::tableview2d_reloadTableData(unsigned short locationid,bool isram)
{
	if (isram)
	{
		emsComms->retrieveBlockFromRam(locationid,0,0);
	}
	else
	{
		if (emsData->hasLocalFlashBlock(locationid))
		{
			TableView2D *table = qobject_cast<TableView2D*>(sender());
			if (table)
			{
				table->setData(locationid,emsData->getLocalFlashBlock(locationid),emsComms->getNew2DTableData());
				emsComms->updateBlockInRam(locationid,0,emsData->getLocalFlashBlock(locationid).size(),emsData->getLocalFlashBlock(locationid));
				emsData->setLocalRamBlock(locationid,emsData->getLocalFlashBlock(locationid));
				emsData->setDeviceRamBlock(locationid,emsData->getLocalFlashBlock(locationid));
			}
		}
	}
}

void MainWindow::dataViewSaveLocation(unsigned short locationid,QByteArray data,int physicallocation)
{
	if (physicallocation == 0)
	{
		//RAM
		emsComms->updateBlockInRam(locationid,0,data.size(),data);
		emsData->setLocalRamBlock(locationid,data);
		/*for (int i=0;i<m_ramMemoryList.size();i++)
		{
			if (m_ramMemoryList[i]->locationid == locationid)
			{
				m_ramMemoryList[i]->setData(data);
			}
		}*/
	}
	else if (physicallocation == 1)
	{
		//FLASH
		emsComms->updateBlockInFlash(locationid,0,data.size(),data);
		emsData->setLocalFlashBlock(locationid,data);
	}
}
void MainWindow::menu_aboutClicked()
{
	aboutMdiWindow->show();
	QApplication::postEvent(aboutMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(aboutMdiWindow, new QEvent(QEvent::WindowActivate));
}
void MainWindow::menu_windows_PacketStatusClicked()
{
	packetStatusMdiWindow->show();
	QApplication::postEvent(packetStatusMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(packetStatusMdiWindow, new QEvent(QEvent::WindowActivate));
}
void MainWindow::updateView(unsigned short locid,QObject *view,QByteArray data,DataType type)
{
	Q_UNUSED(type)
	DataView *dview = qobject_cast<DataView*>(view);
	dview->setData(locid,data,0);
	m_rawDataView[locid]->show();
	m_rawDataView[locid]->raise();
	QApplication::postEvent(m_rawDataView[locid], new QEvent(QEvent::Show));
	QApplication::postEvent(m_rawDataView[locid], new QEvent(QEvent::WindowActivate));

}
void MainWindow::createView(unsigned short locid,QByteArray data,DataType type,bool isram, bool isflash)
{
	qDebug() << "Table Type:" << type;
	if (type == DATA_TABLE_2D)
	{
		qDebug() << "Creating new table view for location: 0x" << QString::number(locid,16).toUpper();
		TableView2D *view = new TableView2D(isram,isflash);
		QString title;
		Table2DMetaData metadata = m_memoryMetaData->get2DMetaData(locid);
		if (metadata.valid)
		{
			if (!view->setData(locid,data,metadata,emsComms->getNew2DTableData()))
			{
				view->deleteLater();
				QMessageBox::information(0,"Error","Table view contains invalid data! Please check your firmware");
				return;
			}
			title = metadata.tableTitle;
		}
		else
		{
			if (!view->setData(locid,data,emsComms->getNew2DTableData()))
			{
				QMessageBox::information(0,"Error","Table view contains invalid data! Please check your firmware");
				view->deleteLater();
				return;
			}
		}
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		//connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
		connect(view,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SLOT(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
		connect(view,SIGNAL(saveToFlash(unsigned short)),this,SLOT(saveFlashLocationId(unsigned short)));
		connect(view,SIGNAL(reloadTableData(unsigned short,bool)),this,SLOT(tableview2d_reloadTableData(unsigned short,bool)));
		connect(view,SIGNAL(reloadTableData(unsigned short,bool)),this,SLOT(reloadDataFromDevice(unsigned short,bool)));
		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper() + " " + title);
		win->setGeometry(view->geometry());
		m_rawDataView[locid] = view;
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	}
	else if (type == DATA_TABLE_3D)
	{
		TableView3D *view = new TableView3D(isram,isflash);
		connect(view,SIGNAL(show3DTable(unsigned short,Table3DData*)),this,SLOT(tableview3d_show3DTable(unsigned short,Table3DData*)));
		QString title;
		Table3DMetaData metadata = m_memoryMetaData->get3DMetaData(locid);
		if (metadata.valid)
		{
			Table3DData *new3ddata = emsComms->getNew3DTableData();
			if (!new3ddata)
			{
				qDebug() << "Something wrong here...";
			}
			TableData *castdata = new3ddata;
			if (!view->setData(locid,data,metadata,castdata))
			{
				QMessageBox::information(0,"Error","Table view contains invalid data! Please check your firmware");
				view->deleteLater();
				return;
			}
			title = metadata.tableTitle;

		}
		else
		{
			if (!view->setData(locid,data,emsComms->getNew3DTableData()))
			{
				QMessageBox::information(0,"Error","Table view contains invalid data! Please check your firmware");
				view->deleteLater();
				return;
			}
		}
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		//connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
		connect(view,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SLOT(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
		connect(view,SIGNAL(saveToFlash(unsigned short)),this,SLOT(saveFlashLocationId(unsigned short)));
		//connect(view,SIGNAL(reloadTableData(unsigned short,bool)),this,SLOT(tableview3d_reloadTableData(unsigned short,bool)));
		connect(view,SIGNAL(reloadTableData(unsigned short,bool)),this,SLOT(reloadDataFromDevice(unsigned short,bool)));
		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper() + " " + title);
		win->setGeometry(view->geometry());
		m_rawDataView[locid] = view;
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	}
	else
	{
		/*if (m_readOnlyMetaDataMap.contains(locid))
		{
			int length=0;
			for (int j=0;j<m_readOnlyMetaDataMap[locid].m_ramData.size();j++)
			{
				length += m_readOnlyMetaDataMap[locid].m_ramData[j].size;
			}
			if (data.size() != length)
			{
				//Wrong size!
				qDebug() << "Invalid meta data size for location id:" << "0x" + QString::number(locid,16).toUpper();
				qDebug() << "Expected:" << length << "Got:" << data.size();
				QMessageBox::information(this,"Error",QString("Meta data indicates this location ID should be ") + QString::number(length) + " however it is " + QString::number(data.size()) + ". Unable to load memory location. Please fix your config.json file");
				return;
			}
			//m_readOnlyMetaDataMap[locid]
			ReadOnlyRamView *view = new ReadOnlyRamView();
			view->passData(locid,data,m_readOnlyMetaDataMap[locid].m_ramData);
			connect(view,SIGNAL(readRamLocation(unsigned short)),this,SLOT(reloadLocationId(unsigned short)));
			connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
			QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
			win->setWindowTitle("Ram Location: 0x" + QString::number(locid,16).toUpper());
			win->setGeometry(view->geometry());
			m_rawDataView[locid] = view;
			win->show();
			QApplication::postEvent(win, new QEvent(QEvent::Show));
			QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
		}
		else
		{*/
		if (m_configBlockMap.contains(locid))
		{
			ConfigView *view = new ConfigView();
			view->passConfig(m_configBlockMap[locid],data);
			connect(view,SIGNAL(saveData(unsigned short,QByteArray)),this,SLOT(configViewSaveData(unsigned short,QByteArray)));
			connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
			QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
			win->setWindowTitle("Config location: 0x" + QString::number(locid,16).toUpper());
			win->setGeometry(view->geometry());
			m_configDataView[locid] = view;
			win->show();
			QApplication::postEvent(win, new QEvent(QEvent::Show));
			QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
		}
		else
		{
			RawDataView *view = new RawDataView(isram,isflash);
			view->setData(locid,data);
			connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
			connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
			connect(view,SIGNAL(reloadData(unsigned short,bool)),this,SLOT(reloadDataFromDevice(unsigned short,bool)));
			QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
			win->setWindowTitle("Ram Location: 0x" + QString::number(locid,16).toUpper());
			win->setGeometry(view->geometry());
			m_rawDataView[locid] = view;
			win->show();
			QApplication::postEvent(win, new QEvent(QEvent::Show));
			QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
		}
		//}
	}
}

void MainWindow::emsInfoDisplayLocationId(int locid,bool isram,DataType type)
{
	Q_UNUSED(type)
	Q_UNUSED(isram)
	if (emsData->hasLocalRamBlock(locid))
	{
		if (m_rawDataView.contains(locid))
		{
			updateView(locid,m_rawDataView[locid],emsData->getLocalRamBlock(locid),type);
		}
		else
		{
			createView(locid,emsData->getLocalRamBlock(locid),type,true,emsData->hasLocalFlashBlock(locid));
		}
	}
	/*else if (m_configBlockMap.contains(locid))
	{
		ConfigView *view = new ConfigView();
		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper());
		win->setGeometry(view->geometry());
		m_rawDataView[locid] = view;
		view->passConfig(m_configBlockMap[locid],emsData->getLocalFlashBlock(locid));
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	}*/
	else if (emsData->hasLocalFlashBlock(locid))
	{
		if (m_rawDataView.contains(locid))
		{
			updateView(locid,m_rawDataView[locid],emsData->getLocalFlashBlock(locid),type);
		}
		else
		{
			createView(locid,emsData->getLocalFlashBlock(locid),type,emsData->hasLocalRamBlock(locid),true);
		}
	}
}
void MainWindow::configViewSaveData(unsigned short locationid,QByteArray data)
{
	qDebug() << "config view save data";
	if (emsData->hasLocalFlashBlock(locationid))
	{
		if (emsData->getLocalFlashBlock(locationid) == data)
		{
			qDebug() << "Data in application memory unchanged, no reason to send write";
			return;
		}
		emsData->setLocalFlashBlock(locationid,data);
		if (!m_offlineMode)
		{
			qDebug() << "Writing to ECU";
			m_currentFlashLocationId = locationid;
			m_waitingForFlashWriteConfirmation = true;
			emsComms->updateBlockInFlash(locationid,0,data.size(),data);
			emsComms->retrieveBlockFromFlash(locationid,0,0);
		}
	}
	else
	{
		qDebug() << "No local flash block... 0x" << QString::number(locationid).toUpper();
	}
}

void MainWindow::rawViewSaveData(unsigned short locationid,QByteArray data,int physicallocation)
{
	Q_UNUSED(physicallocation)
	markRamDirty();
	if (physicallocation==0)
	{
		if (emsData->hasLocalRamBlock(locationid))
		{
			if (emsData->getLocalRamBlock(locationid) == data)
			{
				qDebug() << "Data in application memory unchanged, no reason to send write";
				return;
			}
			emsData->setLocalRamBlock(locationid,data);

		}
		else
		{
			qDebug() << "Attempted to save data for location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Ram list.";
		}
		qDebug() << "Requesting to update ram location:" << "0x" + QString::number(locationid,16).toUpper() << "data size:" << data.size();
		if (!m_offlineMode)
		{
			m_currentRamLocationId = locationid;
			m_waitingForRamWriteConfirmation=true;
			emsComms->updateBlockInRam(locationid,0,data.size(),data);
		}
	}
	else if (physicallocation == 1)
	{
		if (emsData->hasLocalFlashBlock(locationid))
		{
			if (emsData->getLocalFlashBlock(locationid) == data)
			{
				qDebug() << "Data in application memory unchanged, no reason to send write";
				return;
			}
			emsData->setLocalFlashBlock(locationid,data);
		}
		else
		{
			qDebug() << "Attempted to save data for location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Flash list.";
		}
		qDebug() << "Requesting to update flash location:" << "0x" + QString::number(locationid,16).toUpper() << "data size:" << data.size();
		m_currentFlashLocationId = locationid;
		m_waitingForFlashWriteConfirmation=true;
		emsComms->updateBlockInFlash(locationid,0,data.size(),data);
	}


}
void MainWindow::interrogateProgressViewDestroyed(QObject *object)
{
	Q_UNUSED(object);
	if (m_interrogationInProgress)
	{
		m_interrogationInProgress = false;
		interrogateProgressViewCancelClicked();
	}
	QMdiSubWindow *win = qobject_cast<QMdiSubWindow*>(object->parent());
	if (!win)
	{
		//qDebug() << "Error "
		return;
	}
	win->hide();


}

void MainWindow::rawDataViewDestroyed(QObject *object)
{
	QMap<unsigned short,QWidget*>::const_iterator i = m_rawDataView.constBegin();
	while( i != m_rawDataView.constEnd())
	{
		if (i.value() == object)
		{
			//This is the one that needs to be removed.
			m_rawDataView.remove(i.key());
			QMdiSubWindow *win = qobject_cast<QMdiSubWindow*>(object->parent());
			if (!win)
			{
				//qDebug() << "Raw Data View without a QMdiSubWindow parent!!";
				return;
			}
			win->hide();
			ui.mdiArea->removeSubWindow(win);
			return;
		}
		i++;
	}
}
void MainWindow::markRamDirty()
{
	m_localRamDirty = true;
	emsInfo->setLocalRam(true);
}
void MainWindow::markDeviceFlashDirty()
{
	m_deviceFlashDirty = true;
	emsInfo->setDeviceFlash(true);
}
void MainWindow::markRamClean()
{
	m_localRamDirty = false;
	emsInfo->setLocalRam(false);
}
void MainWindow::markDeviceFlashClean()
{
	m_deviceFlashDirty = false;
	emsInfo->setDeviceFlash(false);
}


void MainWindow::ui_saveDataButtonClicked()
{

}

void MainWindow::menu_settingsClicked()
{
	ComSettings *settings = new ComSettings();
	settings->setComPort(m_comPort);
	settings->setBaud(m_comBaud);
	settings->setSaveDataLogs(m_saveLogs);
	settings->setClearDataLogs(m_clearLogs);
	settings->setNumLogsToSave(m_logsToKeep);
	settings->setDataLogDir(m_logDirectory);
	settings->setInterByteDelay(m_comInterByte);
	//m_saveLogs = settings.value("savelogs",true).toBool();
	//m_clearLogs = settings.value("clearlogs",false).toBool();
	//m_logsToKeep = settings.value("logstokeep",0).toInt();
	//m_logDirectory = settings.value("logdir",".").toString();
	connect(settings,SIGNAL(saveClicked()),this,SLOT(settingsSaveClicked()));
	connect(settings,SIGNAL(cancelClicked()),this,SLOT(settingsCancelClicked()));
	QMdiSubWindow *win = ui.mdiArea->addSubWindow(settings);
	win->setWindowTitle(settings->windowTitle());
	win->setGeometry(settings->geometry());
	win->show();
	settings->show();
}

void MainWindow::menu_connectClicked()
{
	if (!m_offlineMode)
	{
		emsData->clearAllMemory();
	}
	emsInfo->clear();
	ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(true);
	m_interrogationInProgress = true;

	m_tempMemoryList.clear();
	interrogationSequenceList.clear();
	m_locIdMsgList.clear();
	m_locIdInfoMsgList.clear();
	emsMdiWindow->hide();
	for (QMap<unsigned short,QWidget*>::const_iterator i= m_rawDataView.constBegin();i != m_rawDataView.constEnd();i++)
	{
		QMdiSubWindow *win = qobject_cast<QMdiSubWindow*>((*i)->parent());
		if (win)
		{
			ui.mdiArea->removeSubWindow(win);
			delete win;
		}
		else
		{
			//Something is wrong here
			qDebug() << "QWidget pointer found in m_rawDataView that is NOT a QMdiSubWindow!!!";
		}

		//delete (*i);
	}
	m_rawDataView.clear();
	if (!emsComms)
	{
		qDebug() << "No EMSCOMMS!!!";
	}
	qDebug() << "Starting emsComms:" << emsComms;
	emsComms->start();
	emsComms->connectSerial(m_comPort,m_comBaud);
}

void MainWindow::menu_disconnectClicked()
{
	emsComms->disconnectSerial();
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
}

void MainWindow::timerTick()
{
	ui.ppsLabel->setText("PPS: " + QString::number(pidcount));
	pidcount = 0;
}
void MainWindow::settingsSaveClicked()
{
	ComSettings *comSettingsWidget = qobject_cast<ComSettings*>(sender());
	m_comBaud = comSettingsWidget->getBaud();
	m_comPort = comSettingsWidget->getComPort();
	m_comInterByte = comSettingsWidget->getInterByteDelay();
	m_saveLogs = comSettingsWidget->getSaveDataLogs();
	m_clearLogs = comSettingsWidget->getClearDataLogs();
	m_logsToKeep = comSettingsWidget->getNumLogsToSave();
	m_logDirectory = comSettingsWidget->getDataLogDir();

	comSettingsWidget->hide();
	QSettings settings(m_settingsFile,QSettings::IniFormat);
	settings.beginGroup("comms");
	settings.setValue("port",m_comPort);
	settings.setValue("baud",m_comBaud);
	settings.setValue("interbytedelay",m_comInterByte);
	settings.setValue("savelogs",m_saveLogs);
	settings.setValue("clearlogs",m_clearLogs);
	settings.setValue("logstokeep",m_logsToKeep);
	settings.setValue("logdir",m_logDirectory);
	settings.setValue("debuglogs",m_debugLogs);
	settings.endGroup();
	QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(comSettingsWidget->parent());
	ui.mdiArea->removeSubWindow(subwin);
	comSettingsWidget->deleteLater();

}
void MainWindow::locationIdInfo(unsigned short locationid,MemoryLocationInfo info)
{
	if (m_memoryInfoMap.contains(locationid))
	{
		qDebug() << "Duplicate location ID recieved from ECU:" << "0x" + QString::number(locationid,16).toUpper();
		//return;
	}
	m_memoryInfoMap[locationid] = info;
	QString title = "";
	if (m_memoryMetaData->has2DMetaData(locationid))
	{
		title = m_memoryMetaData->get2DMetaData(locationid).tableTitle;
		if (m_memoryMetaData->get2DMetaData(locationid).size != info.size)
		{
			interrogateProgressViewCancelClicked();
			QMessageBox::information(0,"Interrogate Error","Error: Meta data for table location 0x" + QString::number(locationid,16).toUpper() + " is not valid for actual table. Size: " + QString::number(info.size) + " expected: " + QString::number(m_memoryMetaData->get2DMetaData(locationid).size));
		}
	}
	if (m_memoryMetaData->has3DMetaData(locationid))
	{
		title = m_memoryMetaData->get3DMetaData(locationid).tableTitle;
		if (m_memoryMetaData->get3DMetaData(locationid).size != info.size)
		{
			interrogateProgressViewCancelClicked();
			QMessageBox::information(0,"Interrogate Error","Error: Meta data for table location 0x" + QString::number(locationid,16).toUpper() + " is not valid for actual table. Size: " + QString::number(info.size) + " expected: " + QString::number(m_memoryMetaData->get3DMetaData(locationid).size));
		}
	}
	if (m_memoryMetaData->hasRORMetaData(locationid))
	{
		title = m_memoryMetaData->getRORMetaData(locationid).dataTitle;
		//m_readOnlyMetaDataMap[locationid]
	}
	if (m_memoryMetaData->hasLookupMetaData(locationid))
	{
		title = m_memoryMetaData->getLookupMetaData(locationid).title;
	}
	//emsInfo->locationIdInfo(locationid,title,rawFlags,flags,parent,rampage,flashpage,ramaddress,flashaddress,size);
	emsInfo->locationIdInfo(locationid,title,info);
	emsData->passLocationInfo(locationid,info);

	//We don't care about ram only locations, since they're not available in offline mode anyway.
	if (info.isFlash)
	{
		checkEmsData->passLocationInfo(locationid,info);
	}
}

void MainWindow::settingsCancelClicked()
{
	//comSettings->hide();
	ComSettings *comSettingsWidget = qobject_cast<ComSettings*>(sender());
	comSettingsWidget->hide();
	QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(comSettingsWidget->parent());
	ui.mdiArea->removeSubWindow(subwin);
	comSettingsWidget->deleteLater();
}
void MainWindow::menu_windows_EmsStatusClicked()
{
	statusView->show();
}

void MainWindow::menu_windows_GaugesClicked()
{
	gaugesMdiWindow->show();
	QApplication::postEvent(gaugesMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(gaugesMdiWindow, new QEvent(QEvent::WindowActivate));
}

void MainWindow::menu_windows_EmsInfoClicked()
{
	emsMdiWindow->show();
	QApplication::postEvent(emsMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(emsMdiWindow, new QEvent(QEvent::WindowActivate));
}

void MainWindow::menu_windows_TablesClicked()
{
	tablesMdiWindow->show();
	QApplication::postEvent(tablesMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(tablesMdiWindow, new QEvent(QEvent::WindowActivate));
}
void MainWindow::menu_windows_FlagsClicked()
{
	flagsMdiWindow->show();
	QApplication::postEvent(flagsMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(flagsMdiWindow, new QEvent(QEvent::WindowActivate));
}

void MainWindow::unknownPacket(QByteArray header,QByteArray payload)
{
	QString result = "";
	for (int i=0;i<header.size();i++)
	{
		result += (((unsigned char)header[i] < (char)0xF) ? "0" : "") + QString::number((unsigned char)header[i],16).toUpper();
	}
	for (int i=0;i<payload.size();i++)
	{
		result += (((unsigned char)payload[i] < (char)0xF) ? "0" : "") + QString::number((unsigned char)payload[i],16).toUpper();
	}
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
			//logLoader->loadFile(filename);
			emsComms->loadLog(filename);

		}
	}
}
void MainWindow::interByteDelayChanged(int num)
{
	emsComms->setInterByteSendDelay(num);
}

void MainWindow::logFinished()
{
	ui.statusLabel->setText("Status: File loaded and log finished");
}

void MainWindow::playLogButtonClicked()
{
	//logLoader->start();
	emsComms->playLog();
	ui.statusLabel->setText("Status: File loaded and playing");
}
void MainWindow::locationIdList(QList<unsigned short> idlist)
{
	qDebug() << "Location ID List!";
	for (int i=0;i<idlist.size();i++)
	{
	//ui/listWidget->addItem(QString::number(idlist[i]));0000000000000000000000000000000000000000
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = idlist[i];
		m_tempMemoryList.append(loc);
		int seq = emsComms->getLocationIdInfo(idlist[i]);
		qDebug() << "LocationIdInfoReq:" << seq;
		if (progressView) progressView->setMaximum(progressView->maximum()+1);
		if (progressView) progressView->addTask("Getting Location ID Info for 0x" + QString::number(idlist[i],16).toUpper(),seq,1);
		m_locIdMsgList.append(seq);
		interrogationSequenceList.append(seq);
	}
}
void MainWindow::blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload)
{
	Q_UNUSED(sequencenumber)
	Q_UNUSED(header)
	Q_UNUSED(payload)
}
void MainWindow::dataLogPayloadReceived(QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	Q_UNUSED(payload)
    dataPacketDecoder->decodePayload(payload);

}
void MainWindow::interfaceVersion(QString version)
{
	//ui.interfaceVersionLineEdit->setText(version);
	m_interfaceVersion = version;
	if (emsInfo)
	{
		emsInfo->setInterfaceVersion(version);

	}
	emsinfo.interfaceVersion = version;
}
void MainWindow::firmwareVersion(QString version)
{
	//ui.firmwareVersionLineEdit->setText(version);
	m_firmwareVersion = version;
	this->setWindowTitle(QString("EMStudio ") + QString(define2string(GIT_COMMIT)) + " Firmware: " + version);
	if (emsInfo)
	{
		emsInfo->setFirmwareVersion(version);
	}
	emsinfo.firmwareVersion = version;
}
void MainWindow::error(SerialPortStatus error,QString msg)
{
	Q_UNUSED(error); //We don't actually use the error here, since we treat every error the same.

	switch (QMessageBox::information(0,"",msg,"Ok","Retry","Load Offline Data"))
	{
	case 0:		//Ok
		//We do nothing here.
		break;
	case 1:		//Retry
		//Delay of 2 seconds to allow for this function to return, and the emscomms loop to be destroyed
		if (error == LOADER_MODE)
		{
			//We need to send a SM reset, this is not supported yet.
		}
		QTimer::singleShot(2000,this,SLOT(menu_connectClicked()));
		break;
	case 2:		//Load Offline Data
		//Delay here, to ensure that it goes into the event loop and isn't directly called.
		QTimer::singleShot(500,this,SLOT(menu_file_loadOfflineDataClicked()));
		break;
	default:
		qDebug() << "WEEEE";
		break;
	}
}

void MainWindow::error(QString msg)
{
	//Q_UNUSED(msg)
	QMessageBox::information(0,"Error",msg);
}
void MainWindow::interrogateProgressViewCancelClicked()
{
	m_offlineMode = true;
	emsComms->disconnectSerial();
	emsComms->wait(1000);
	emsComms->terminate();
	emsComms->wait(1000);
}
void MainWindow::emsCompilerVersion(QString version)
{
	emsinfo.compilerVersion = version;
}

void MainWindow::emsFirmwareBuildDate(QString date)
{
	emsinfo.firmwareBuildDate = date;
}

void MainWindow::emsDecoderName(QString name)
{
	emsinfo.decoderName = name;
}

void MainWindow::emsOperatingSystem(QString os)
{
	emsinfo.operatingSystem = os;
}

void MainWindow::emsCommsConnected()
{
	//New log and settings file here.
	if (m_memoryInfoMap.size() == 0)
	{
		m_offlineMode = false;
	}
	if (progressView)
	{
		progressView->reset();
	}
	else
	{
		ui.actionInterrogation_Progress->setEnabled(true);
		progressView = new InterrogateProgressView();
		connect(progressView,SIGNAL(destroyed(QObject*)),this,SLOT(interrogateProgressViewDestroyed(QObject*)));
		interrogateProgressMdiWindow = ui.mdiArea->addSubWindow(progressView);
		interrogateProgressMdiWindow->setGeometry(progressView->geometry());
		connect(progressView,SIGNAL(cancelClicked()),this,SLOT(interrogateProgressViewCancelClicked()));
		progressView->setMaximum(0);
	}
	interrogationSequenceList.clear();
	//disconnect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,0);
	//disconnect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,0);
	//connect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,SLOT(interrogateRamBlockRetrieved(unsigned short,QByteArray,QByteArray)));
	//connect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,SLOT(interrogateFlashBlockRetrieved(unsigned short,QByteArray,QByteArray)));
	emsData->setInterrogation(true);
	progressView->show();
	interrogateProgressMdiWindow->show();
	progressView->addOutput("Connected to EMS");
	//this->setEnabled(false);
	int seq = emsComms->getFirmwareVersion();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Firmware Version",seq,0);

	seq = emsComms->getInterfaceVersion();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Interface Version",seq,0);

	seq = emsComms->getLocationIdList(0x00,0x00);
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Location ID List",seq,0);

	seq = emsComms->getCompilerVersion();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Compiler Version",seq,0);

	seq = emsComms->getDecoderName();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Decoder Name",seq,0);

	seq = emsComms->getFirmwareBuildDate();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Firmware Build Date",seq,0);

	seq = emsComms->getMaxPacketSize();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Max Packet Size",seq,0);

	seq = emsComms->getOperatingSystem();
	interrogationSequenceList.append(seq);
	progressView->addTask("Get Operating System",seq,0);


	progressView->setMaximum(8);
	//progressView->setMax(progressView->max()+1);

    //LogLoader *loader = new LogLoader();
    //connect(loader,SIGNAL(payloadReceived(QByteArray,QByteArray)),this,SLOT(dataLogPayloadReceived(QByteArray,QByteArray)));
    //loader->loadFile("/home/michael/Downloads/2013.05.01-19.50.30.bin");
    //loader->start();

}
void MainWindow::checkSyncRequest()
{
	emsComms->getLocationIdList(0,0);
}
void MainWindow::tableview3d_show3DTable(unsigned short locationid,Table3DData *data)
{
	if (m_table3DMapViewMap.contains(locationid))
	{
		m_table3DMapViewMap[locationid]->show();
		QApplication::postEvent(m_table3DMapViewMap[locationid], new QEvent(QEvent::Show));
		QApplication::postEvent(m_table3DMapViewMap[locationid], new QEvent(QEvent::WindowActivate));
		return;
	}

	TableMap3D *m_tableMap = new TableMap3D();

	m_table3DMapViewWidgetMap[locationid] = m_tableMap;
	m_tableMap->passData(data);
	QMdiSubWindow *win = ui.mdiArea->addSubWindow(m_tableMap);
	connect(win,SIGNAL(destroyed(QObject*)),this,SLOT(tableMap3DDestroyed(QObject*)));
	win->setGeometry(m_tableMap->geometry());
	win->setWindowTitle("0x" + QString::number(locationid,16).toUpper());
	win->show();
	QApplication::postEvent(win, new QEvent(QEvent::Show));
	QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	m_table3DMapViewMap[locationid] = win;

}
void MainWindow::tableMap3DDestroyed(QObject *object)
{
	Q_UNUSED(object)
	for (QMap<unsigned short,QMdiSubWindow*>::const_iterator i = m_table3DMapViewMap.constBegin(); i != m_table3DMapViewMap.constEnd();i++)
	{
		if (i.value() == sender())
		{
			m_table3DMapViewWidgetMap.remove(i.key());
			m_table3DMapViewMap.remove(i.key());
			return;
		}
	}
}

void MainWindow::emsStatusHardResetRequested()
{
	if (QMessageBox::information(0,"Warning","Hard resetting the ECU will erase all changes currently in RAM, but not saved to FLASH, and restart the ECU. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Attempting hard reset:" << emsComms->hardReset();
	}
}

void MainWindow::emsStatusSoftResetRequested()
{
	if (QMessageBox::information(0,"Warning","Soft resetting the ECU will erase all changes currently in RAM, but not saved to FLASH, and restart the ECU. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "Attempting soft reset:" << emsComms->softReset();
	}
	//ui.mdiArea->removeSubWindow(emsStatusMdiWindow);
	//this->addDockWidget(Qt::RightDockWidgetArea,statusView);
}

void MainWindow::updateRamLocation(unsigned short locationid)
{
	bool hasparent = false;
	unsigned short tempRamParentId=0;
	bool isparent = false;
	QList<unsigned short> childlist;

	childlist = emsData->getChildrenOfLocalRamLocation(locationid);
	if (childlist.size() > 0)
	{
		isparent = true;
	}
	if (emsData->getParentOfLocalRamLocation(locationid).size() > 0)
	{
		tempRamParentId = emsData->getParentOfLocalRamLocation(locationid)[0];
		hasparent = true;
	}
	emsData->setDeviceRamBlock(locationid,emsData->getLocalRamBlock(locationid));
	//Find all windows that use that location id
	if (hasparent && isparent)
	{
		//This should never happen.
		qDebug() << "Found a memory location that is parent AND child!!! This should not happen.";
		qDebug() << "Parent:" << "0x" + QString::number(tempRamParentId);
		qDebug() << "Current:" << "0x" + QString::number(locationid);
		QString children;
		for (int i=0;i<childlist.size();i++)
		{
			children += "0x" + QString::number(childlist[i],16).toUpper() + " ";
		}
		qDebug() << "Children" << children;
	}
	else if (hasparent)
	{
		//qDebug() << "No children, is a child for:" << "0x" + QString::number(m_currentRamLocationId,16).toUpper();
		updateDataWindows(tempRamParentId);
	}
	else if (isparent)
	{
		for (int i=0;i<childlist.size();i++)
		{
			updateDataWindows(childlist[i]);
		}

	}
	//qDebug() << "No children for:" << "0x" + QString::number(m_currentRamLocationId,16).toUpper();
	updateDataWindows(locationid);
}
void MainWindow::commandTimedOut(int sequencenumber)
{
	qDebug() << "Command timed out:" << QString::number(sequencenumber);
	if (m_waitingForRamWriteConfirmation)
	{
		m_waitingForRamWriteConfirmation = false;
		if (emsData->hasLocalRamBlock(m_currentRamLocationId))
		{
			if (emsData->hasDeviceRamBlock(m_currentRamLocationId))
			{
				qDebug() << "Data reverting for location id 0x" + QString::number(m_currentRamLocationId,16);
				if (emsData->getLocalRamBlock(m_currentRamLocationId) == emsData->getDeviceRamBlock(m_currentRamLocationId))
				{
					qDebug() << "Data valid. No need for a revert.";
				}
				else
				{
					qDebug() << "Invalid data, reverting...";
					emsData->setLocalRamBlock(m_currentRamLocationId,emsData->getDeviceRamBlock(m_currentRamLocationId));
					/*if (m_ramMemoryList[i]->data() != m_deviceRamMemoryList[j]->data())
					{
						qDebug() << "Failed to revert!!!";
					}*/
					updateRamLocation(m_currentRamLocationId);
				}
			}
		}
		else
		{
			qDebug() << "Unable to find memory location " << QString::number(m_currentRamLocationId,16) << "in local or device memory!";
		}
		//Find all windows that use that location id
		m_currentRamLocationId = 0;
		//checkRamFlashSync();
	}
	else
	{
		//qDebug() << "Error reverting! " << QString::number(m_currentRamLocationId,16) << "Location not found!";
	}
	if (m_waitingForFlashWriteConfirmation)
	{
		m_waitingForFlashWriteConfirmation = false;
		if (emsData->hasLocalFlashBlock(m_currentFlashLocationId))
		{
			if (emsData->hasDeviceFlashBlock(m_currentFlashLocationId))
			{
				qDebug() << "Data reverting for location id 0x" + QString::number(m_currentFlashLocationId,16);
				if (emsData->getLocalFlashBlock(m_currentFlashLocationId) == emsData->getDeviceFlashBlock(m_currentFlashLocationId))
				{
					qDebug() << "Data valid. No need for a revert.";
				}
				else
				{
					qDebug() << "Invalid data, reverting...";
					//m_flashMemoryList[i]->setData(m_deviceFlashMemoryList[j]->data());
					emsData->setLocalFlashBlock(m_currentFlashLocationId,emsData->getDeviceFlashBlock(m_currentFlashLocationId));
					/*if (m_flashMemoryList[i]->data() != m_deviceFlashMemoryList[j]->data())
					{
						qDebug() << "Failed to revert!!!";
					}*/
					updateRamLocation(m_currentFlashLocationId);
				}
			}
		}
		else
		{
			qDebug() << "Unable to find memory location " << QString::number(m_currentFlashLocationId,16) << "in local or device memory!";
		}
		//Find all windows that use that location id
		m_currentFlashLocationId = 0;
		//checkRamFlashSync();
		return;
	}
	else
	{
		//qDebug() << "Error reverting! " << QString::number(m_currentFlashLocationId,16) << "Location not found!";
	}
	if (m_interrogationInProgress)
	{
		progressView->taskFail(sequencenumber);
		//If interrogation is in progress, we need to stop, since something has gone
		//horribly wrong.
		interrogateProgressViewCancelClicked();
		QMessageBox::information(0,"Error","Something has gone serious wrong, one of the commands timed out during interrogation. This should be properly investigated before continuing");
	}

}
void MainWindow::commandSuccessful(int sequencenumber)
{
	qDebug() << "Command succesful:" << QString::number(sequencenumber);
	if (m_interrogationInProgress)
	{
		if (progressView) progressView->taskSucceed(sequencenumber);
	}
	if (m_waitingForRamWriteConfirmation)
	{
		m_waitingForRamWriteConfirmation = false;
		updateRamLocation(m_currentRamLocationId);
		checkRamFlashSync();
		m_currentRamLocationId=0;
		return;
	}
	if (m_waitingForFlashWriteConfirmation)
	{
		m_waitingForFlashWriteConfirmation = false;
		m_currentFlashLocationId=0;
		return;
	}
	checkMessageCounters(sequencenumber);

}
void MainWindow::checkMessageCounters(int sequencenumber)
{
	qDebug() << "Checking message counters:" << sequencenumber << m_locIdInfoMsgList.size() << m_locIdMsgList.size() << interrogationSequenceList.size();
	if (m_locIdInfoMsgList.contains(sequencenumber))
	{
		m_locIdInfoMsgList.removeOne(sequencenumber);
		if (m_locIdInfoMsgList.size() == 0)
		{
			if (m_offlineMode && m_checkEmsDataInUse)
			{
				//We were offline. Let's check.
				QList<unsigned short> ramlocs = checkEmsData->getTopLevelDeviceRamLocations();
				QList<unsigned short> flashlocs = checkEmsData->getTopLevelDeviceFlashLocations();
				bool ramdirty = false;
				bool flashdirty = false;
				for (int i=0;i<ramlocs.size();i++)
				{
					if (!(emsData->getDeviceRamBlock(ramlocs[i])==checkEmsData->getDeviceRamBlock(ramlocs[i])))
					{
						qDebug() << "Ram Location id:" << "0x" + QString::number(ramlocs[i],16).toUpper() << "is different!";
						ramdirty = true;
					}
				}
				for (int i=0;i<flashlocs.size();i++)
				{
					if (!(emsData->getDeviceFlashBlock(flashlocs[i]) == checkEmsData->getDeviceFlashBlock(flashlocs[i])))
					{
						qDebug() << "Flash Location id:" << "0x" + QString::number(flashlocs[i],16).toUpper() << "is different!";
						flashdirty = true;
					}
				}
				if (ramdirty)
				{
					if (QMessageBox::information(0,"Warning","RAM from offline mode does not equal ram on device! Do you wish to write offline RAM data to the device?",QMessageBox::Yes,QMessageBox::No) != QMessageBox::Yes)
					{
						for (int i=0;i<ramlocs.size();i++)
						{
							emsData->setDeviceRamBlock(ramlocs[i],checkEmsData->getDeviceRamBlock(ramlocs[i]));
							emsData->setLocalRamBlock(ramlocs[i],checkEmsData->getDeviceRamBlock(ramlocs[i]));
						}
					}
					else
					{
						for (int i=0;i<ramlocs.size();i++)
						{
							emsComms->updateBlockInRam(ramlocs[i],0,emsData->getDeviceRamBlock(ramlocs[i]).size(),emsData->getDeviceRamBlock(ramlocs[i]));
						}
					}
				}
				if (flashdirty)
				{
					if (QMessageBox::information(0,"Warning","Flash from offline mode does not equal flash on device! Do you wish to write offline flash data to the device?",QMessageBox::Yes,QMessageBox::No) != QMessageBox::Yes)
					{
						for (int i=0;i<flashlocs.size();i++)
						{
							emsData->setDeviceFlashBlock(flashlocs[i],checkEmsData->getDeviceFlashBlock(flashlocs[i]));
							emsData->setLocalFlashBlock(flashlocs[i],checkEmsData->getDeviceFlashBlock(flashlocs[i]));
						}
					}
					else
					{
						for (int i=0;i<flashlocs.size();i++)
						{
							emsComms->updateBlockInFlash(flashlocs[i],0,emsData->getDeviceFlashBlock(flashlocs[i]).size(),emsData->getDeviceFlashBlock(flashlocs[i]));
						}
					}

				}
				m_offlineMode = false;
			}
			qDebug() << "All Ram and Flash locations updated";
			//End of the location ID information messages.
			checkRamFlashSync();
		}
	}
	else
	{
		qDebug() << "Not in locidinfo";
	}
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			qDebug() << "All ID information recieved. Requesting Ram and Flash updates";
			populateParentLists();
			QList<unsigned short>  memorylist = emsData->getTopLevelDeviceFlashLocations();
			for (int i=0;i<memorylist.size();i++)
			{
				int seq = emsComms->retrieveBlockFromFlash(memorylist[i],0,0);
				if (progressView) progressView->addTask("Getting Location ID 0x" + QString::number(memorylist[i],16).toUpper(),seq,2);
				m_locIdInfoMsgList.append(seq);
				if (progressView) progressView->setMaximum(progressView->maximum()+1);
				interrogationSequenceList.append(seq);
			}
			memorylist = emsData->getTopLevelDeviceRamLocations();
			for (int i=0;i<memorylist.size();i++)
			{
				int seq = emsComms->retrieveBlockFromRam(memorylist[i],0,0);
				if (progressView) progressView->addTask("Getting Location ID 0x" + QString::number(memorylist[i],16).toUpper(),seq,2);
				m_locIdInfoMsgList.append(seq);
				if (progressView) progressView->setMaximum(progressView->maximum()+1);
				interrogationSequenceList.append(seq);
			}
		}
	}
	else
	{
		qDebug() << "Not in locidmsglist";
	}
	if (interrogationSequenceList.contains(sequencenumber))
	{
		qDebug() << "GOOD!";
		if (progressView)
		{
			progressView->setProgress(progressView->progress()+1);
		}
		interrogationSequenceList.removeOne(sequencenumber);
		if (interrogationSequenceList.size() == 0)
		{
			m_interrogationInProgress = false;
			if (progressView)
			{
				//progressView->hide();
				QMdiSubWindow *win = qobject_cast<QMdiSubWindow*>(progressView->parent());
				if (win)
				{
					win->hide();
				}
				progressView->done();
			}
			//this->setEnabled(true);
			qDebug() << "Interrogation complete";

			emsData->setInterrogation(false);


			//emsInfo->show();
			emsMdiWindow->show();
			//Write everything to the settings.
			QString json = "";
			json += "{";
			QJson::Serializer jsonSerializer;
			QVariantMap top;
			top["firmwareversion"] = emsinfo.firmwareVersion;
			top["interfaceversion"] = emsinfo.interfaceVersion;
			top["compilerversion"] = emsinfo.compilerVersion;
			top["firmwarebuilddate"] = emsinfo.firmwareBuildDate;
			top["decodername"] = emsinfo.decoderName;
			top["operatingsystem"] = emsinfo.operatingSystem;
			top["emstudiohash"] = emsinfo.emstudioHash;
			top["emstudiocommit"] = emsinfo.emstudioCommit;
			/*QVariantMap memorylocations;
			for (int i=0;i<m_deviceRamMemoryList.size();i++)
			{
				QVariantMap tmp;
				tmp["flashaddress"] =  m_deviceRamMemoryList[i]->flashAddress;
				tmp["flashpage"] = m_deviceRamMemoryList[i]->flashPage;
				tmp["rampage"] = m_deviceRamMemoryList[i]->ramPage;
				tmp["ramaddress"] = m_deviceRamMemoryList[i]->ramAddress;
				tmp["hasparent"] = (m_deviceRamMemoryList[i]->hasParent) ? "true" : "false";
				tmp["size"] = m_deviceRamMemoryList[i]->size;
				QString memory = "";
				for (int j=0;j<m_deviceRamMemoryList[i]->data().size();j++)
				{
					memory += QString("0x") + (((unsigned char)m_deviceRamMemoryList[i]->data()[j] <= 0xF) ? "0" : "") + QString::number((unsigned char)m_deviceRamMemoryList[i]->data()[j],16).toUpper() + ",";
				}
				memory = memory.mid(0,memory.length()-1);
				tmp["data"] = memory;
				//memorylocations["0x" + QString::number(m_deviceRamMemoryList[i]->locationid,16).toUpper()] = tmp;

			}
			for (int i=0;i<m_deviceFlashMemoryList.size();i++)
			{
				QVariantMap tmp;
				tmp["flashaddress"] =  m_deviceFlashMemoryList[i]->flashAddress;
				tmp["flashpage"] = m_deviceFlashMemoryList[i]->flashPage;
				tmp["rampage"] = m_deviceFlashMemoryList[i]->ramPage;
				tmp["ramaddress"] = m_deviceFlashMemoryList[i]->ramAddress;
				tmp["hasparent"] = (m_deviceFlashMemoryList[i]->hasParent) ? "true" : "false";
				tmp["size"] = m_deviceFlashMemoryList[i]->size;
				QString memory = "";
				for (int j=0;j<m_deviceFlashMemoryList[i]->data().size();j++)
				{
					memory += QString("0x") + (((unsigned char)m_deviceFlashMemoryList[i]->data()[j] <= 0xF) ? "0" : "") + QString::number((unsigned char)m_deviceFlashMemoryList[i]->data()[j],16).toUpper() + ",";
				}
				memory = memory.mid(0,memory.length()-1);
				tmp["data"] = memory;
				//memorylocations["0x" + QString::number(m_deviceFlashMemoryList[i]->locationid,16).toUpper()] = tmp;
			}*/
			//top["memory"] = memorylocations;
			if (m_saveLogs)
			{
				QFile *settingsFile = new QFile(m_localHomeDir + "/logs/" + m_logFileName + ".meta.json");
				settingsFile->open(QIODevice::ReadWrite);
				settingsFile->write(jsonSerializer.serialize(top));
				settingsFile->close();
			}
		}
		else
		{
			qDebug() << interrogationSequenceList.size() << "messages left to go. First one:" << interrogationSequenceList[0];
		}
	}
	else
	{
		qDebug() << "Not in interrogation list";
	}
}

void MainWindow::retrieveFlashLocationId(unsigned short locationid)
{
	emsComms->retrieveBlockFromFlash(locationid,0,0);
}

void MainWindow::retrieveRamLocationId(unsigned short locationid)
{
	emsComms->retrieveBlockFromRam(locationid,0,0);
}

void MainWindow::updateDataWindows(unsigned short locationid)
{
	if (m_rawDataView.contains(locationid))
	{
		DataView *dview = qobject_cast<DataView*>(m_rawDataView[locationid]);
		if (dview)
		{
			if (emsData->hasLocalRamBlock(locationid))
			{
				dview->setData(locationid,emsData->getLocalRamBlock(locationid));
			}
			else if (emsData->hasLocalFlashBlock(locationid))
			{
				dview->setData(locationid,emsData->getLocalFlashBlock(locationid));
			}
			else
			{
				qDebug() << "updateDataWindows called for location id" << "0x" + QString::number(locationid,16).toUpper() << "but no local ram or flash block exists!";
			}
			//return;
		}
	}
	if (m_table3DMapViewWidgetMap.contains(locationid))
	{
		qDebug() << "Updating...";
		m_table3DMapViewWidgetMap[locationid]->update();
		m_table3DMapViewWidgetMap[locationid]->updateGL();
	}
	if (m_configDataView.contains(locationid))
	{
		m_configDataView[locationid]->passConfig(m_configBlockMap[locationid],emsData->getLocalFlashBlock(locationid));
	}
	else
	{
		qDebug() << "Attempted to update a window that does not exist!" << "0x" + QString::number(locationid,16).toUpper();
	}
}

void MainWindow::checkRamFlashSync()
{
	emsData->populateLocalRamAndFlash();


}

void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	qDebug() << "Command failed:" << QString::number(sequencenumber) << "0x" + QString::number(errornum,16);
	if (!m_interrogationInProgress)
	{
		QMessageBox::information(0,"Command Failed","Command failed with error: " + m_memoryMetaData->getErrorString(errornum));
	}
	else
	{
		if (progressView) progressView->taskFail(sequencenumber);
	}
	//bool found = false;
	if (m_waitingForRamWriteConfirmation)
	{
		m_waitingForRamWriteConfirmation = false;
		if (emsData->hasLocalRamBlock(m_currentRamLocationId))
		{
			if (emsData->hasDeviceRamBlock(m_currentRamLocationId))
			{
				qDebug() << "Data reverting for location id 0x" + QString::number(m_currentRamLocationId,16);
				if (emsData->getLocalRamBlock(m_currentRamLocationId) == emsData->getDeviceRamBlock(m_currentRamLocationId))
				{
					qDebug() << "Data valid. No need for a revert.";
				}
				else
				{
					qDebug() << "Invalid data, reverting...";
					emsData->setLocalRamBlock(m_currentRamLocationId,emsData->getDeviceRamBlock(m_currentRamLocationId));
					/*if (m_ramMemoryList[i]->data() != m_deviceRamMemoryList[j]->data())
					{
						qDebug() << "Failed to revert!!!";
					}*/
					updateRamLocation(m_currentRamLocationId);
				}
			}
		}
		else
		{
			qDebug() << "Unable to find memory location " << QString::number(m_currentRamLocationId,16) << "in local or device memory!";
		}
		//Find all windows that use that location id
		m_currentRamLocationId = 0;
		//checkRamFlashSync();
	}
	else
	{
		//qDebug() << "Error reverting! " << QString::number(m_currentRamLocationId,16) << "Location not found!";
	}
	if (m_waitingForFlashWriteConfirmation)
	{
		m_waitingForFlashWriteConfirmation = false;
		if (emsData->hasLocalFlashBlock(m_currentFlashLocationId))
		{
			if (emsData->hasDeviceFlashBlock(m_currentFlashLocationId))
			{
				qDebug() << "Data reverting for location id 0x" + QString::number(m_currentFlashLocationId,16);
				if (emsData->getLocalFlashBlock(m_currentFlashLocationId) == emsData->getDeviceFlashBlock(m_currentFlashLocationId))
				{
					qDebug() << "Data valid. No need for a revert.";
				}
				else
				{
					qDebug() << "Invalid data, reverting...";
					//m_flashMemoryList[i]->setData(m_deviceFlashMemoryList[j]->data());
					emsData->setLocalFlashBlock(m_currentFlashLocationId,emsData->getDeviceFlashBlock(m_currentFlashLocationId));
					/*if (m_flashMemoryList[i]->data() != m_deviceFlashMemoryList[j]->data())
					{
						qDebug() << "Failed to revert!!!";
					}*/
					updateRamLocation(m_currentFlashLocationId);
				}
			}
		}
		else
		{
			qDebug() << "Unable to find memory location " << QString::number(m_currentFlashLocationId,16) << "in local or device memory!";
		}
		//Find all windows that use that location id
		m_currentFlashLocationId = 0;
		//checkRamFlashSync();
		return;
	}
	else
	{
		//qDebug() << "Error reverting! " << QString::number(m_currentFlashLocationId,16) << "Location not found!";
	}
	checkMessageCounters(sequencenumber);

}
void MainWindow::populateParentLists()
{
	//Need to get a list of all IDs here now.
	qDebug() << "Populating internal memory parent list.";
	/*qDebug() << m_deviceFlashMemoryList.size() << "Device flash locations";
	qDebug() << m_flashMemoryList.size() << "Application flash locations";
	qDebug() << m_deviceRamMemoryList.size() << "Device Ram locations";
	qDebug() << m_ramMemoryList.size() << "Application Ram locations";*/
	emsData->populateDeviceRamAndFlashParents();
	checkEmsData->populateDeviceRamAndFlashParents();
}

void MainWindow::pauseLogButtonClicked()
{

}
void MainWindow::saveFlashLocationIdBlock(unsigned short locationid,QByteArray data)
{
	qDebug() << "Burning flash block:" << "0x" + QString::number(locationid,16).toUpper();
	if (emsData->hasLocalFlashBlock(locationid))
	{
		emsData->setLocalFlashBlock(locationid,data);
	}
	if (m_offlineMode)
	{
		emsData->setDeviceFlashBlock(locationid,data);
	}
	else
	{
		emsComms->updateBlockInFlash(locationid,0,data.size(),data);
	}
}

void MainWindow::saveFlashLocationId(unsigned short locationid)
{
	qDebug() << "Burning block from ram to flash for locationid:" << "0x"+QString::number(locationid,16).toUpper();
	if (!m_offlineMode)
	{
		emsComms->burnBlockFromRamToFlash(locationid,0,0);
	}
	if (emsData->hasLocalFlashBlock(locationid))
	{
		if(emsData->hasDeviceRamBlock(locationid))
		{
			emsData->setLocalFlashBlock(locationid,emsData->getDeviceRamBlock(locationid));
			if (m_offlineMode)
			{
				emsData->setDeviceFlashBlock(locationid,emsData->getLocalFlashBlock(locationid));
			}
			//getLocalFlashBlock(locationid);
			//getDeviceRamBlock(locationid);
		}
	}
}

void MainWindow::saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size)
{
	if (m_offlineMode)
	{
		if (emsData->hasDeviceRamBlock(locationid))
		{
			if (emsData->getDeviceRamBlock(locationid).mid(offset,size) == data)
			{
				qDebug() << "Data in application memory unchanged, no reason to send write for single value";
				return;
			}
			emsData->setDeviceRamBlock(locationid,emsData->getDeviceRamBlock(locationid).replace(offset,size,data));
		}
		else
		{
			qDebug() << "Attempted to save data for single value at location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Ram list.";
		}
	}
	if (emsData->hasLocalRamBlock(locationid))
	{
		if (emsData->getLocalRamBlock(locationid).mid(offset,size) == data)
		{
			qDebug() << "Data in application memory unchanged, no reason to send write for single value";
			return;
		}
		emsData->setLocalRamBlock(locationid,emsData->getLocalRamBlock(locationid).replace(offset,size,data));
	}
	else
	{
		qDebug() << "Attempted to save data for single value at location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Ram list.";
	}
	if (m_offlineMode)
	{
		updateDataWindows(locationid); //This is required to update all windows during an offline edit.
	}
	qDebug() << "Requesting to update single value at ram location:" << "0x" + QString::number(locationid,16).toUpper() << "data size:" << data.size();
	qDebug() << "Offset:" << offset << "Size:" << size  <<  "Data:" << data;

	if (!m_offlineMode)
	{
		m_currentRamLocationId = locationid;
		m_waitingForRamWriteConfirmation = true;
		emsComms->updateBlockInRam(locationid,offset,size,data);
	}
}

void MainWindow::stopLogButtonClicked()
{

}
void MainWindow::connectButtonClicked()
{
	emsComms->connectSerial(m_comPort,m_comBaud);
}

void MainWindow::logProgress(qlonglong current,qlonglong total)
{
	Q_UNUSED(current)
	Q_UNUSED(total)
	//setWindowTitle(QString::number(current) + "/" + QString::number(total) + " - " + QString::number((float)current/(float)total));
}
void MainWindow::guiUpdateTimerTick()
{
}
void MainWindow::dataLogDecoded(QVariantMap data)
{
	//m_valueMap = data;
	if (dataTables)
	{
		dataTables->passData(data);
	}
	if (dataGauges)
	{
		dataGauges->passData(data);
	}
	if (dataFlags)
	{
		dataFlags->passData(data);
	}
	if (statusView)
	{
		statusView->passData(data);
	}
	for (QMap<unsigned short,QWidget*>::const_iterator i=m_rawDataView.constBegin();i!=m_rawDataView.constEnd();i++)
	{
		DataView *dview = qobject_cast<DataView*>(i.value());
		if (dview)
		{
			dview->passDatalog(data);
		}
	}
	if (data.contains("tempClock"))
	{
		int newval = data["tempClock"].toInt();
		if (m_currentEcuClock == -1)
		{
			m_currentEcuClock = newval;
		}
		else
		{
			if (newval == 0 && m_currentEcuClock != 255)
			{
				//We probably had a chip reset here. Ask, and verify.
				if (QMessageBox::question(0,"Warning","It seems that the ECU has reset. This means any un-flashed RAM changes may be lost. Would you like to re-send any RAM data in the tuner to ensure no changes are lost?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
				{
					QList<unsigned short> loclist = emsData->getTopLevelDeviceRamLocations();
					for (int i=0;i<loclist.size();i++)
					{
						if (!emsData->isReadOnlyRamBlock(loclist[i]))
						{
							emsComms->updateBlockInRam(loclist[i],0,emsData->getDeviceRamBlock(loclist[i]).size(),emsData->getDeviceRamBlock(loclist[i]));
						}
					}
				}

			}
			if (m_currentEcuClock == 255)
			{
				if (newval == 0)
				{
					//All is good
				}
				else
				{
					qDebug() << "A: Packets lost! At least:" << newval << "packets";
				}
			}
			else
			{
				if (m_currentEcuClock + 1 != newval)
				{
					if (m_currentEcuClock > newval)
					{
						qDebug() << "B: Packets lost! At least:" << (255 - m_currentEcuClock) + newval << "packets";
					}
					else
					{
						qDebug() << "C: Packets lost! At least:" << newval - m_currentEcuClock << "packets";
					}

				}
			}
			m_currentEcuClock = newval;
		}
	}
}
void MainWindow::logPayloadReceived(QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	pidcount++;
	if (payload.length() != 96)
	{
		//Wrong sized payload!
		//We should do something here or something...
		//return;
	}
    //If we are logging, disable this:
    dataPacketDecoder->decodePayload(payload);
	//guiUpdateTimerTick();

}

MainWindow::~MainWindow()
{
	emsComms->stop();
	emsComms->wait(1000);
	delete emsComms;
}
