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
#include <QFileDialog>
//#include "datafield.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSettings>
#include <tableview2d.h>
#include <qjson/parser.h>
#include "logloader.h"
#include "QsLog.h"
#include "wizardview.h"

#define define2string_p(x) #x
#define define2string(x) define2string_p(x)

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	m_interrogationInProgress = false;
	m_offlineMode = false;
	m_checkEmsDataInUse = false;
	m_currentEcuClock = -1;
	m_EcuResetPopup = false;
	interrogateProgressMdiWindow=0;
	qRegisterMetaType<MemoryLocationInfo>("MemoryLocationInfo");
	qRegisterMetaType<DataType>("DataType");
	qRegisterMetaType<SerialPortStatus>("SerialPortStatus");
	QLOG_INFO() << "EMStudio commit:" << define2string(GIT_COMMIT);
	QLOG_INFO() << "Full hash:" << define2string(GIT_HASH);
	progressView=0;
	emsComms=0;
	m_interrogationInProgress = false;
	m_debugLogs = false;

//	emsData = new EmsData();
//	connect(emsData,SIGNAL(updateRequired(unsigned short)),this,SLOT(updateDataWindows(unsigned short)));

	//Create this, even though we only need it during offline to online transitions.
//	checkEmsData = new EmsData();

	m_settingsFile = "settings.ini";
	//TODO: Figure out proper directory names
#ifdef Q_OS_WIN
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
	//Settings file should ALWAYS be the one in the settings dir. No reason to have it anywhere else.
	m_settingsFile = m_settingsDir + "/EMStudio-config.ini";

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
		//decoderfilestr = "decodersettings.json";
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
			QLOG_ERROR() << "Error parsing json";
			QLOG_ERROR() << "Line number:" << decoderparser.errorLine() << "error text:" << decoderparser.errorString();
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
					QLOG_TRACE() << "XCalc:" << calcliststr[j].toMap()["type"].toString() << calcliststr[j].toMap()["value"].toDouble();
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
	connect(ui.actionParameter_View,SIGNAL(triggered()),this,SLOT(menu_windows_ParameterViewClicked()));
	connect(ui.actionFirmware_Metadata,SIGNAL(triggered()),this,SLOT(menu_windows_firmwareMetadataClicked()));
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
	QLOG_INFO() << "Loading plugin from:" << m_pluginFileName;
	//m_pluginFileName = "plugins/libmsplugin.so";
	//m_pluginFileName = "plugins/libo5eplugin.so";

	m_logFileName = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");

	emsInfo = new EmsInfoView();
	connect(emsInfo,SIGNAL(displayLocationId(int,bool,DataType)),this,SLOT(emsInfoDisplayLocationId(int,bool,DataType)));

	emsMdiWindow = ui.mdiArea->addSubWindow(emsInfo);
	emsMdiWindow->setGeometry(emsInfo->geometry());
	emsMdiWindow->hide();
	emsMdiWindow->setWindowTitle(emsInfo->windowTitle());

	parameterView = new ParameterView();
	parameterMdiWindow = ui.mdiArea->addSubWindow(parameterView);
	parameterMdiWindow->setGeometry(parameterView->geometry());
	parameterMdiWindow->hide();
	parameterMdiWindow->setWindowTitle(parameterView->windowTitle());

	aboutView = new AboutView();
	aboutView->setHash(define2string(GIT_HASH));
	aboutView->setCommit(define2string(GIT_COMMIT));
	aboutView->setDate(define2string(GIT_DATE));
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

	firmwareMetaData = new FirmwareMetaData();
	firmwareMetaMdiWindow = ui.mdiArea->addSubWindow(firmwareMetaData);
	firmwareMetaMdiWindow->setGeometry(firmwareMetaData->geometry());
	firmwareMetaMdiWindow->hide();
	firmwareMetaMdiWindow->setWindowTitle(firmwareMetaData->windowTitle());

	dataTables = new TableView();
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
	packetStatusMdiWindow = ui.mdiArea->addSubWindow(packetStatus);
	packetStatusMdiWindow->setGeometry(packetStatus->geometry());
	packetStatusMdiWindow->hide();
	packetStatusMdiWindow->setWindowTitle(packetStatus->windowTitle());

	//Load settings
	QLOG_INFO() << "Local settings file is:" << m_settingsFile;
	QSettings settings(m_settingsFile,QSettings::IniFormat);
	settings.beginGroup("comms");
#if defined(Q_OS_WIN)
	m_comPort = settings.value("port","COM4").toString();
#elif defined(Q_OS_LINUX)
	m_comPort = settings.value("port","/dev/ttyUSB0").toString();
#else
	m_comPort = settings.value("port","/dev/cu.usbmodem").toString();
#endif
	m_comBaud = settings.value("baud",115200).toInt();
	m_comInterByte = settings.value("interbytedelay",0).toInt();
	m_saveLogs = settings.value("savelogs",true).toBool();
	m_clearLogs = settings.value("clearlogs",false).toBool();
	m_logsToKeep = settings.value("logstokeep",0).toInt();
	m_logDirectory = settings.value("logdir",m_localHomeDir + "/logs").toString();
	m_debugLogs = settings.value("debuglogs",false).toBool();
	settings.endGroup();

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


	//ui.menuWizards
	connect(ui.mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(subMdiWindowActivated(QMdiSubWindow*)));
	QSettings windowsettings;
	windowsettings.beginGroup("general");
	this->restoreGeometry(windowsettings.value("location").toByteArray());
	if (windowsettings.value("isMaximized",false).toBool())
	{
		this->showMaximized();
	}

	windowsettings.endGroup();


}
void MainWindow::menu_windows_interrogateProgressViewClicked()
{
	interrogateProgressMdiWindow->show();
	QApplication::postEvent(interrogateProgressMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(interrogateProgressMdiWindow, new QEvent(QEvent::WindowActivate));
}
void MainWindow::menu_windows_ParameterViewClicked()
{
	parameterMdiWindow->show();
	QApplication::postEvent(parameterMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(parameterMdiWindow, new QEvent(QEvent::WindowActivate));
}
void MainWindow::menu_windows_firmwareMetadataClicked()
{
	firmwareMetaMdiWindow->show();
	QApplication::postEvent(firmwareMetaMdiWindow, new QEvent(QEvent::Show));
	QApplication::postEvent(firmwareMetaMdiWindow, new QEvent(QEvent::WindowActivate));
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
	QVariantMap datatable2d;
	QVariantMap datatable3d;
	QVariantMap dataraw;
	if (!emsComms)
	{
		return;
	}
	for (int i=0;i<m_locationIdList.size();i++)
	{

		if (emsComms->get2DTableData(m_locationIdList[i]))
		{
			QVariantMap current;
			QVariantList axis;
			QVariantList data;
			for (int j=0;j<emsComms->get2DTableData(m_locationIdList[i])->axis().size();j++)
			{
				axis.append(emsComms->get2DTableData(m_locationIdList[i])->axis()[j]);
			}
			for (int j=0;j<emsComms->get2DTableData(m_locationIdList[i])->values().size();j++)
			{
				data.append(emsComms->get2DTableData(m_locationIdList[i])->values()[j]);
			}
			QLOG_DEBUG() << "2D Table Saved";
			QLOG_DEBUG() << "Axis Size:" << axis.size();
			QLOG_DEBUG() << "Data Size:" << data.size();
			current["axis"] = axis;
			current["data"] = data;
			current["title"] = emsComms->getMetaParser()->get2DMetaData(m_locationIdList[i]).tableTitle;
			current["xtitle"] = emsComms->getMetaParser()->get2DMetaData(m_locationIdList[i]).xAxisTitle;
			current["ytitle"] = emsComms->getMetaParser()->get2DMetaData(m_locationIdList[i]).yAxisTitle;
			current["ram"] = emsComms->get2DTableData(m_locationIdList[i])->isRam();
			datatable2d[QString::number(m_locationIdList[i],16).toUpper()] = current;
		}
		if (emsComms->get3DTableData(m_locationIdList[i]))
		{
			QVariantMap current;
			QVariantList xlist;
			QVariantList ylist;
			QVariantList zlist;
			for (int j=0;j<emsComms->get3DTableData(m_locationIdList[i])->xAxis().size();j++)
			{
				xlist.append(emsComms->get3DTableData(m_locationIdList[i])->xAxis()[j]);
			}
			for (int j=0;j<emsComms->get3DTableData(m_locationIdList[i])->yAxis().size();j++)
			{
				ylist.append(emsComms->get3DTableData(m_locationIdList[i])->yAxis()[j]);
			}
			for (int j=0;j<emsComms->get3DTableData(m_locationIdList[i])->values().size();j++)
			{
				QVariantList zrow;
				for (int k=0;k<emsComms->get3DTableData(m_locationIdList[i])->values()[j].size();k++)
				{
					zrow.append(emsComms->get3DTableData(m_locationIdList[i])->values()[j][k]);
				}
				zlist.append((QVariant)zrow);
			}
			current["x"] = xlist;
			current["y"] = ylist;
			current["z"] = zlist;
			current["title"] = emsComms->getMetaParser()->get3DMetaData(m_locationIdList[i]).tableTitle;
			current["xtitle"] = emsComms->getMetaParser()->get3DMetaData(m_locationIdList[i]).xAxisTitle;
			current["ytitle"] = emsComms->getMetaParser()->get3DMetaData(m_locationIdList[i]).yAxisTitle;
			current["ztitle"] = emsComms->getMetaParser()->get3DMetaData(m_locationIdList[i]).zAxisTitle;
			current["ram"] = emsComms->get3DTableData(m_locationIdList[i])->isRam();
			datatable3d[QString::number(m_locationIdList[i],16).toUpper()] = current;
		}
		if (emsComms->getRawData(m_locationIdList[i]))
		{
			QVariantList raw;
			for (int j=0;j<emsComms->getRawData(m_locationIdList[i])->data().size();j++)
			{
				raw.append((unsigned char)emsComms->getRawData(m_locationIdList[i])->data()[j]);
			}
			dataraw[QString::number(m_locationIdList[i],16).toUpper()] = raw;
		}
	}
	top["2D"] = datatable2d;
	top["3D"] = datatable3d;
	top["RAW"] = dataraw;
	QJson::Serializer serializer2;
	QByteArray out2 = serializer2.serialize(top);
	QFile outfile2(filename);
	outfile2.open(QIODevice::ReadWrite | QIODevice::Truncate);
	outfile2.write(out2);
	outfile2.flush();
	outfile2.close();
}

void MainWindow::menu_file_loadOfflineDataClicked()
{

	QString filename = QFileDialog::getOpenFileName(this,"Load Offline File",".","Offline JSON Files (*.json)");
	if (filename == "")
	{
		QLOG_ERROR() << "No offline file selected!";
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
		QLOG_ERROR() << "Error parsing json:" << parser.errorString();
		return;
	}

	/*
	top["2D"] = datatable2d;
	top["3D"] = datatable3d;
	top["RAW"] = dataraw;
	*/
	QVariantMap top = outputvar.toMap();
	QVariantMap datatable2d = top["2D"].toMap();
	QVariantMap datatable3d = top["3D"].toMap();
	QVariantMap dataraw = top["RAW"].toMap();

	for(QVariantMap::const_iterator i=datatable2d.constBegin();i!=datatable2d.constEnd();i++)
	{
		bool ok = false;
		unsigned short locid = i.key().toInt(&ok,16);
		if (!ok)
		{
			QMessageBox::information(0,"Error","Error parsing json");
			return;
		}
		QVariantMap tablemap = i.value().toMap();
		QVariantList axislist = tablemap["axis"].toList();
		QVariantList datalist = tablemap["data"].toList();
		Table2DData *data = emsComms->get2DTableData(locid);
		data->setWritesEnabled(false);
		for (int j=0;j<axislist.size();j++)
		{
			data->setCell(0,j,axislist[j].toDouble());

		}
		for (int j=0;j<datalist.size();j++)
		{
			data->setCell(1,j,datalist[j].toDouble());
		}
		data->setWritesEnabled(true);
		if (tablemap["ram"].toBool())
		{
			data->writeWholeLocation(true);
		}
		else
		{
			data->writeWholeLocation(false);
		}
	}
	for (QVariantMap::const_iterator i=datatable3d.constBegin();i!=datatable3d.constEnd();i++)
	{
		bool ok = false;
		unsigned short locid = i.key().toInt(&ok,16);
		if (!ok)
		{
			QMessageBox::information(0,"Error","Error parsing json");
			return;
		}
		QVariantMap current = i.value().toMap();
		QVariantList xlist = current["x"].toList();
		QVariantList ylist = current["y"].toList();
		QVariantList zlist = current["z"].toList();
		Table3DData *data = emsComms->get3DTableData(locid);
		data->setWritesEnabled(false);
		for (int j=0;j<xlist.size();j++)
		{
			data->setCell(ylist.size()-1,j,xlist[j].toDouble());
		}
		for (int j=0;j<ylist.size();j++)
		{
			data->setCell(j,0,ylist[j].toDouble());
		}
		for (int j=0;j<zlist.size();j++)
		{
			QVariantList z2list = zlist[j].toList();
			for (int k=0;k<z2list.size();k++)
			{
				data->setCell(j,k,z2list[k].toDouble());
			}

		}
		data->setWritesEnabled(true);
		if (current["ram"].toBool())
		{
			data->writeWholeLocation(true);
		}
		else
		{
			data->writeWholeLocation(false);
		}

	}
	for (QVariantMap::const_iterator i=dataraw.constBegin();i!=datatable3d.constEnd();i++)
	{
		bool ok = false;
		unsigned short locid = i.key().toInt(&ok,16);
		if (!ok)
		{
			QMessageBox::information(0,"Error","Error parsing json");
			return;
		}
		QVariantList bytes = i.value().toList();
		RawData *data = emsComms->getRawData(locid);
		QByteArray bytearray;
		for (int j=0;j<bytes.size();j++)
		{
			bytearray.append(bytes[j].toUInt());
		}
		data->setData(locid,true,bytearray);
	}
}
void MainWindow::emsCommsSilence()
{
	//This is called when the ems has been silent for 5 seconds, when it was previously talking.
	QLOG_WARN() << "EMS HAS GONE SILENT";
}

void MainWindow::emsCommsSilenceBroken()
{
	//This is called when ems had previously been talking, gone silent, then started talking again.
	QLOG_WARN() << "EMS HAS GONE NOISEY";
}

void MainWindow::emsCommsDisconnected()
{
	ui.actionSave_Offline_Data->setEnabled(false);
	ui.actionLoad_Offline_Data->setEnabled(false);

	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
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
	QLOG_INFO() << "Attempting to load plugin:" << m_pluginFileName;
	if (!pluginLoader->load())
	{

		QLOG_ERROR() << "Unable to load plugin. error:" << pluginLoader->errorString();
		exit(-1);
	}
	emsComms = qobject_cast<EmsComms*>(pluginLoader->instance());
	if (!emsComms)
	{
		QLOG_ERROR() << "Unable to load plugin!!!";
		QLOG_ERROR() << pluginLoader->errorString();
		exit(-1);
	}
	emsComms->passLogger(&QsLogging::Logger::instance());

	QStringList searchpaths;
	searchpaths.append(m_settingsDir + "/" + "definitions");
	searchpaths.append(m_defaultsDir + "/definitions");
	searchpaths.append("."); //Local
	searchpaths.append("../../.."); //OSX local
	m_memoryMetaData = emsComms->getMetaParser();
	m_memoryMetaData->loadMetaDataFromFile(searchpaths); //Changed to trigger a load from a file found internally.
	//emsData->setMetaData(m_memoryMetaData);
	parameterView->passConfigBlockList(m_memoryMetaData->configMetaData());
	parameterView->passMenuList(m_memoryMetaData->menuMetaData());
	QLOG_INFO() << m_memoryMetaData->errorMap().keys().size() << "Error Keys Loaded";
	QLOG_INFO() << m_memoryMetaData->table3DMetaData().size() << "3D Tables Loaded";
	QLOG_INFO() << m_memoryMetaData->table2DMetaData().size() << "2D Tables Loaded";
	dataPacketDecoder = emsComms->getDecoder();
	//connect(dataPacketDecoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	connect(emsComms,SIGNAL(dataLogPayloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	dataTables->passDecoder(dataPacketDecoder);
	m_logFileName = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");
	emsComms->setLogFileName(m_logFileName);
	emsComms->setLogDirectory(m_logDirectory);
	connect(emsComms,SIGNAL(resetDetected(int)),this,SLOT(ecuResetDetected(int)));
	connect(emsComms,SIGNAL(dataLogPayloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	connect(emsComms,SIGNAL(interrogationProgress(int,int)),this,SLOT(interrogationProgress(int,int)));
	connect(emsComms,SIGNAL(interrogationComplete()),this,SLOT(interrogationComplete()));
	connect(emsComms,SIGNAL(interrogateTaskStart(QString,int)),this,SLOT(interrogateTaskStart(QString,int)));
	connect(emsComms,SIGNAL(interrogateTaskSucceed(int)),this,SLOT(interrogateTaskSucceed(int)));
	connect(emsComms,SIGNAL(interrogateTaskFail(int)),this,SLOT(interrogateTaskFail(int)));
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(emsSilenceStarted()),this,SLOT(emsCommsSilence()));
	connect(emsComms,SIGNAL(emsSilenceBroken()),this,SLOT(emsCommsSilenceBroken()));
	connect(emsComms,SIGNAL(error(QString)),this,SLOT(error(QString)));
	connect(emsComms,SIGNAL(error(SerialPortStatus,QString)),this,SLOT(error(SerialPortStatus,QString)));

	connect(emsComms,SIGNAL(disconnected()),this,SLOT(emsCommsDisconnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)),Qt::QueuedConnection);
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandTimedOut(int)),this,SLOT(commandTimedOut(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,MemoryLocationInfo)),this,SLOT(locationIdInfo(unsigned short,MemoryLocationInfo)));
	connect(emsComms,SIGNAL(packetSent(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketSent(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetAcked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketAck(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray,unsigned short)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray,unsigned short)));
	connect(emsComms,SIGNAL(decoderFailure(QByteArray)),packetStatus,SLOT(passDecoderFailure(QByteArray)));
	connect(emsComms,SIGNAL(interrogationData(QMap<QString,QString>)),this,SLOT(interrogationData(QMap<QString,QString>)));
	emsComms->setBaud(m_comBaud);
	emsComms->setPort(m_comPort);
	emsComms->setLogsEnabled(m_saveLogs);
	emsComms->setInterByteSendDelay(m_comInterByte);
	emsComms->setlogsDebugEnabled(m_debugLogs);
	emsComms->start();
}

void MainWindow::locationIdList(QList<unsigned short> idlist)
{
	m_locationIdList = idlist;
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

void MainWindow::createView(unsigned short locid,DataType type)
{
	if (type == DATA_TABLE_3D)
	{
		Table3DData *data = emsComms->get3DTableData(locid);
		TableView3D *view = new TableView3D();
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		connect(view,SIGNAL(show3DTable(unsigned short,Table3DData*)),this,SLOT(tableview3d_show3DTable(unsigned short,Table3DData*)));
		QString title;
		Table3DMetaData metadata = m_memoryMetaData->get3DMetaData(locid);
		view->setMetaData(metadata);
		DataBlock *block = dynamic_cast<DataBlock*>(data);
		if (!view->setData(locid,block))
		{
			QMessageBox::information(0,"Error","Table view contains invalid data! Please check your firmware");
			view->deleteLater();
			return;
		}
		title = metadata.tableTitle;
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));

		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper() + " " + title);
		win->setGeometry(0,0,((view->width() < this->width()-160) ? view->width() : this->width()-160),((view->height() < this->height()-100) ? view->height() : this->height()-100));
		m_rawDataView[locid] = view;
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	}
	else if (type == DATA_TABLE_2D)
	{
		Table2DData *data = emsComms->get2DTableData(locid);
		TableView2D *view = new TableView2D();
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		QString title;
		Table2DMetaData metadata = m_memoryMetaData->get2DMetaData(locid);
		view->setMetaData(metadata);
		DataBlock *block = dynamic_cast<DataBlock*>(data);
		if (!view->setData(locid,block))
		{
			return;
		}
		title = metadata.tableTitle;
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));

		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper() + " " + title);
		win->setGeometry(0,0,((view->width() < this->width()-160) ? view->width() : this->width()-160),((view->height() < this->height()-100) ? view->height() : this->height()-100));
		m_rawDataView[locid] = view;
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));
	}
	else
	{
		//Unhandled data type. Show it as a hex view.
		RawData *data = emsComms->getRawData(locid);
		RawDataView *view = new RawDataView(!data->isFlashOnly(),true);
		connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		view->setData(locid,data);

		QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
		connect(win,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
		win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper());
		win->setGeometry(0,0,((view->width() < this->width()-160) ? view->width() : this->width()-160),((view->height() < this->height()-100) ? view->height() : this->height()-100));
		m_rawDataView[locid] = view;
		win->show();
		QApplication::postEvent(win, new QEvent(QEvent::Show));
		QApplication::postEvent(win, new QEvent(QEvent::WindowActivate));

	}
}


void MainWindow::emsInfoDisplayLocationId(int locid,bool isram,DataType type)
{
	Q_UNUSED(isram)
	if (!m_rawDataView.contains(locid))
	{
		createView(locid,type);
	}
	else
	{
		QApplication::postEvent(m_rawDataView[locid]->parent(), new QEvent(QEvent::Show));
		QApplication::postEvent(m_rawDataView[locid]->parent(), new QEvent(QEvent::WindowActivate));
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
				win = qobject_cast<QMdiSubWindow*>(object);
				if (!win)
				{
					return;
				}
			}
			ui.menuOpen_Windows->removeAction(m_mdiSubWindowToActionMap[win]);
			m_mdiSubWindowToActionMap.remove(win);
			win->hide();
			ui.mdiArea->removeSubWindow(win);
			return;
		}
		i++;
	}
}

void MainWindow::ui_saveDataButtonClicked()
{

}

void MainWindow::menu_settingsClicked()
{
	ComSettings *settings = new ComSettings();
	//connect(settings,SIGNAL(windowHiding(QMdiSubWindow*)),this,SLOT(windowHidden(QMdiSubWindow*)));
	settings->setComPort(m_comPort);
	settings->setBaud(m_comBaud);
	settings->setSaveDataLogs(m_saveLogs);
	settings->setClearDataLogs(m_clearLogs);
	settings->setNumLogsToSave(m_logsToKeep);
	settings->setDataLogDir(m_logDirectory);
	settings->setInterByteDelay(m_comInterByte);
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
	if (interrogateProgressMdiWindow)
	{
		delete interrogateProgressMdiWindow;
		progressView = 0;
		interrogateProgressMdiWindow = 0;
	}

	if (!m_offlineMode)
	{
		//emsData->clearAllMemory();
	}
	emsInfo->clear();
	ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(true);
	m_interrogationInProgress = true;

	//m_tempMemoryList.clear();
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
			QLOG_ERROR() << "QWidget pointer found in m_rawDataView that is NOT a QMdiSubWindow!!!";
		}

		//delete (*i);
	}
	m_rawDataView.clear();
	if (!emsComms)
	{
		QLOG_ERROR() << "No EMSCOMMS!!!";
	}
	QLOG_INFO() << "Starting emsComms:" << emsComms;
	//emsComms->start();
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
	comSettingsWidget->close();
	comSettingsWidget->deleteLater();
	if (emsComms)
	{
		emsComms->setInterByteSendDelay(m_comInterByte);
		emsComms->setlogsDebugEnabled(m_debugLogs);
		emsComms->setLogDirectory(m_logDirectory);
	}

}
void MainWindow::locationIdInfo(unsigned short locationid,MemoryLocationInfo info)
{
	if (m_memoryInfoMap.contains(locationid))
	{
		QLOG_WARN() << "Duplicate location ID recieved from ECU:" << "0x" + QString::number(locationid,16).toUpper();
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
	//emsData->passLocationInfo(locationid,info);

	//We don't care about ram only locations, since they're not available in offline mode anyway.
	if (info.isFlash)
	{
		//checkEmsData->passLocationInfo(locationid,info);
	}
}

void MainWindow::settingsCancelClicked()
{
	//comSettings->hide();
	ComSettings *comSettingsWidget = qobject_cast<ComSettings*>(sender());
	comSettingsWidget->hide();
	QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(comSettingsWidget->parent());
	ui.mdiArea->removeSubWindow(subwin);
	comSettingsWidget->close();
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
void MainWindow::interrogationData(QMap<QString,QString> datamap)
{
	firmwareMetaData->setInterrogationData(datamap);
	//emsInfo->setInterrogationData(datamap);
}

void MainWindow::interfaceVersion(QString version)
{
	//ui.interfaceVersionLineEdit->setText(version);
	m_interfaceVersion = version;
	if (emsInfo)
	{

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
		if (error == SM_MODE)
		{
			//We need to send a SM reset, this is not supported yet.
		}
		QTimer::singleShot(500,this,SLOT(menu_connectClicked()));
		break;
	case 2:		//Load Offline Data
		//Delay here, to ensure that it goes into the event loop and isn't directly called.
		QTimer::singleShot(500,this,SLOT(menu_file_loadOfflineDataClicked()));
		break;
	default:
		QLOG_FATAL() << "WEEEE";
		break;
	}
}

void MainWindow::error(QString msg)
{
	QMessageBox::information(0,"Error",msg);
}
void MainWindow::interrogateProgressViewCancelClicked()
{
	m_offlineMode = true;
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
void MainWindow::loadWizards(QString dir)
{
	QDir wizards(dir);
	foreach (QString file,wizards.entryList(QDir::Files | QDir::NoDotAndDotDot))
	{
		if (file.endsWith(".qml"))
		{
			WizardView *view = new WizardView();
			connect(emsComms,SIGNAL(configRecieved(ConfigBlock,QVariant)),view,SLOT(configRecieved(ConfigBlock,QVariant)));
			m_wizardList.append(view);
			for (int i=0;i<emsComms->getConfigList().size();i++)
			{
				view->addConfig(emsComms->getConfigList()[i],emsComms->getConfigData(emsComms->getConfigList()[i]));
			}
			view->setFile(emsComms,wizards.absoluteFilePath(file));
			view->passConfig(m_memoryMetaData->configMetaData());
			view->setGeometry(0,0,800,600);
			QAction *action = new QAction(this);
			action->setText(file.mid(0,file.lastIndexOf(".")));
			action->setCheckable(true);
			ui.menuWizards->addAction(action);
			connect(action,SIGNAL(triggered(bool)),view,SLOT(setVisible(bool)));
			connect(view,SIGNAL(visibilityChanged(bool)),action,SLOT(setChecked(bool)));
		}
	}
}

void MainWindow::emsCommsConnected()
{
	emsComms->startInterrogation();
	m_interrogationFailureCount = 0;
	ui.actionSave_Offline_Data->setEnabled(true);
	ui.actionLoad_Offline_Data->setEnabled(true);
	while (ui.menuWizards->actions().size() > 0)
	{
		QAction *action = ui.menuWizards->actions().takeFirst();
		ui.menuWizards->removeAction(action);
		delete action;
	}
	for (int i=0;i<m_wizardList.size();i++)
	{
		delete m_wizardList[i];
	}
	m_wizardList.clear();
	//m_defaultsDir
	QDir defaultsdir(m_defaultsDir);
	loadWizards(defaultsdir.absolutePath() + "/wizards");
	loadWizards("wizards");
	loadWizards(m_localHomeDir + "/wizards");
	for (int i=0;i<emsComms->getConfigList().size();i++)
	{
		parameterView->addConfig(emsComms->getConfigList()[i],emsComms->getConfigData(emsComms->getConfigList()[i]));
		/*for (int j=0;j<m_wizardList.size();j++)
		{
			m_wizardList[j]->addConfig(emsComms->getConfigList()[i],emsComms->getConfigData(emsComms->getConfigList()[i]));
		}*/
	}
	//New log and settings file here.
	if (m_memoryInfoMap.size() == 0)
	{
		m_offlineMode = false;
	}
	if (progressView)
	{
		progressView->reset();
		ui.actionInterrogation_Progress->setEnabled(true);
	}
	else
	{
		ui.actionInterrogation_Progress->setEnabled(true);
		progressView = new InterrogateProgressView();
		connect(progressView,SIGNAL(destroyed(QObject*)),this,SLOT(interrogateProgressViewDestroyed(QObject*)));
		interrogateProgressMdiWindow = ui.mdiArea->addSubWindow(progressView);
		interrogateProgressMdiWindow->setWindowTitle(progressView->windowTitle());
		interrogateProgressMdiWindow->setGeometry(progressView->geometry());
		connect(progressView,SIGNAL(cancelClicked()),this,SLOT(interrogateProgressViewCancelClicked()));
		progressView->setMaximum(0);
	}
	interrogationSequenceList.clear();
	progressView->show();
	interrogateProgressMdiWindow->show();
	progressView->addOutput("Connected to EMS");
}
void MainWindow::interrogationProgress(int current, int total)
{
	Q_UNUSED(current);
	Q_UNUSED(total);
}

void MainWindow::interrogationComplete()
{
	m_interrogationInProgress = false;
	if (progressView)
	{
		//progressView->hide();
		QMdiSubWindow *win = qobject_cast<QMdiSubWindow*>(progressView->parent());
		if (win && m_interrogationFailureCount == 0)
		{
			win->hide();
		}
		progressView->done();
	}
	QLOG_INFO() << "Interrogation complete";
	if (m_interrogationFailureCount > 0)
	{
		QMessageBox::information(0,"Error","Something has gone serious wrong, one of the commands timed out during interrogation. This should be properly investigated before continuing");
		emsComms->disconnectSerial();
	return;
	}
	else
	{
		emsMdiWindow->show();
	}
	QSettings windowsettings;
	int size = windowsettings.beginReadArray("rawwindows");
	for (int i=0;i<size;i++)
	{
		//createView()
		windowsettings.setArrayIndex(i);
		unsigned short locid = windowsettings.value("window").toInt();
		QString type = windowsettings.value("type").toString();
		if (type == "TableView2D")
		{
			createView(locid,DATA_TABLE_2D);
			QWidget *parent = (QWidget*)m_rawDataView[locid]->parent();
			parent->restoreGeometry(windowsettings.value("location").toByteArray());
		}
		else if (type == "TableView3D")
		{
			createView(locid,DATA_TABLE_3D);
			QWidget *parent = (QWidget*)m_rawDataView[locid]->parent();
			parent->restoreGeometry(windowsettings.value("location").toByteArray());
		}
		else if (type == "tablesMdiWindow")
		{
			tablesMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			tablesMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());

		}
		else if (type == "firmwareMetaMdiWindow")
		{
			firmwareMetaMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			firmwareMetaMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "interrogateProgressMdiWindow")
		{
			// interrogateProgressMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			// interrogateProgressMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "emsMdiWindow")
		{
			emsMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			emsMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "flagsMdiWindow")
		{
			flagsMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			flagsMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "gaugesMdiWindow")
		{
			gaugesMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			gaugesMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "packetStatusMdiWindow")
		{
			packetStatusMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			packetStatusMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "aboutMdiWindow")
		{
			aboutMdiWindow->restoreGeometry(windowsettings.value("location").toByteArray());
			aboutMdiWindow->setHidden(windowsettings.value("hidden",true).toBool());
		}
		else if (type == "emsStatusMdiWindow")
		{

		}
		else
		{
			qDebug() << "Unknown type:" << type;
		}

	}
	/*	QMdiSubWindow *tablesMdiWindow;
	QMdiSubWindow *firmwareMetaMdiWindow;
	QMdiSubWindow *interrogateProgressMdiWindow;
	QMdiSubWindow *emsMdiWindow;
	QMdiSubWindow *flagsMdiWindow;
	QMdiSubWindow *gaugesMdiWindow;
	QMdiSubWindow *packetStatusMdiWindow;
	QMdiSubWindow *aboutMdiWindow;
	QMdiSubWindow *emsStatusMdiWindow;*/
	windowsettings.endArray();
	/*for (QMap<unsigned short,QWidget*>::const_iterator i=m_rawDataView.constBegin();i!=m_rawDataView.constEnd();i++)
	{
		windowsettings.setArrayIndex(val++);
		windowsettings.value("window",i.key());
		QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(i.value()->parent());
		windowsettings.value("location",subwin->saveGeometry());
		windowsettings.value("hidden",subwin->isHidden());
		windowsettings.value("type",i.value()->metaObject()->className());

	}
	windowsettings.endArray();*/
}
void MainWindow::interrogateTaskStart(QString task, int sequence)
{
	if(task.contains("Location ID"))
	{
		progressView->addTask(task,sequence,1);
	}
	else if (task.contains("Ram Location") || task.contains("Flash Location"))
	{
		progressView->addTask(task,sequence,2);
	}
	else if (task.contains("Ecu Info"))
	{
		progressView->addTask(task,sequence,0);
	}
}

void MainWindow::interrogateTaskSucceed(int sequence)
{
	progressView->taskSucceed(sequence);
}

void MainWindow::interrogateTaskFail(int sequence)
{
	progressView->taskFail(sequence);
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
		QLOG_INFO() << "Attempting hard reset:" << emsComms->hardReset();
	}
}

void MainWindow::emsStatusSoftResetRequested()
{
	if (QMessageBox::information(0,"Warning","Soft resetting the ECU will erase all changes currently in RAM, but not saved to FLASH, and restart the ECU. Are you sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
	{
		QLOG_INFO() << "Attempting soft reset:" << emsComms->softReset();
	}
}

void MainWindow::commandTimedOut(int sequencenumber)
{
	QLOG_INFO() << "Command timed out:" << QString::number(sequencenumber);
	if (m_waitingForRamWriteConfirmation)
	{

	}
	else
	{
	}
	if (m_waitingForFlashWriteConfirmation)
	{
		m_waitingForFlashWriteConfirmation = false;

		//Find all windows that use that location id
		m_currentFlashLocationId = 0;
		//checkRamFlashSync();
		return;
	}
	else
	{
	}
	if (m_interrogationInProgress)
	{
		progressView->taskFail(sequencenumber);
		//If interrogation is in progress, we need to stop, since something has gone
		//horribly wrong.
		//interrogateProgressViewCancelClicked();

		m_interrogationFailureCount++;
	}

}
void MainWindow::commandSuccessful(int sequencenumber)
{
	QLOG_INFO() << "Command succesful:" << QString::number(sequencenumber);
	if (m_interrogationInProgress)
	{
		//if (progressView) progressView->taskSucceed(sequencenumber);
	}

}
void MainWindow::checkMessageCounters(int sequencenumber)
{
	QLOG_INFO() << "Checking message counters:" << sequencenumber << m_locIdInfoMsgList.size() << m_locIdMsgList.size() << interrogationSequenceList.size();
	if (m_locIdInfoMsgList.contains(sequencenumber))
	{
		m_locIdInfoMsgList.removeOne(sequencenumber);
		if (m_locIdInfoMsgList.size() == 0)
		{
			if (m_offlineMode && m_checkEmsDataInUse)
			{
				//We were offline. Let's check.

				m_offlineMode = false;
			}
			QLOG_INFO() << "All Ram and Flash locations updated";
			//End of the location ID information messages.
			checkRamFlashSync();
		}
	}
	else
	{
		QLOG_INFO() << "Not in locidinfo";
	}
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			QLOG_INFO() << "All ID information recieved. Requesting Ram and Flash updates";
			//populateParentLists();
			/*TODO
			 *QList<unsigned short>  memorylist = emsData->getTopLevelDeviceFlashLocations();
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
			}*/
		}
	}
	else
	{
		QLOG_INFO() << "Not in locidmsglist";
	}
	if (interrogationSequenceList.contains(sequencenumber))
	{
		QLOG_INFO() << "GOOD!";
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
			QLOG_INFO() << "Interrogation complete";

//			emsData->setInterrogation(false);


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

			if (m_saveLogs)
			{
				QFile *settingsFile = new QFile(m_logDirectory + "/" + m_logFileName + ".meta.json");
				settingsFile->open(QIODevice::ReadWrite);
				settingsFile->write(jsonSerializer.serialize(top));
				settingsFile->close();
			}
		}
		else
		{
			QLOG_INFO() << interrogationSequenceList.size() << "messages left to go. First one:" << interrogationSequenceList[0];
		}
	}
	else
	{
		QLOG_INFO() << "Not in interrogation list";
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



void MainWindow::checkRamFlashSync()
{
	//emsData->populateLocalRamAndFlash();


}

void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	QLOG_INFO() << "Command failed:" << QString::number(sequencenumber) << "0x" + QString::number(errornum,16);
	if (!m_interrogationInProgress)
	{
		QMessageBox::information(0,"Command Failed","Command failed with error: " + m_memoryMetaData->getErrorString(errornum));
	}
	else
	{
		if (progressView) progressView->taskFail(sequencenumber);
	}
}


void MainWindow::pauseLogButtonClicked()
{

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
void MainWindow::ecuResetDetected(int missedpackets)
{
	if (!m_interrogationInProgress && !m_EcuResetPopup)
	{
		m_EcuResetPopup = true;
		if (QMessageBox::question(this,"Error","ECU Reset detected with " + QString::number(missedpackets) + " missed packets! Would you like to reflash data? If you do not, then EMStudio will continue with an INVALID idea of ECU memory, and you will lose any changes made",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
		{
			emsComms->writeAllRamToRam();
		}
		else
		{
			QMessageBox::information(this,"Warning","EMStudio will now continue with a corrupt version of ECU memory. The death of your engine is on your head now");
		}
		m_EcuResetPopup = false;
	}
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
		DataView *dview = dynamic_cast<DataView*>(i.value());
		if (dview)
		{
			dview->passDatalog(data);
		}
	}
}
void MainWindow::logPayloadReceived(QByteArray header,QByteArray payload)
{
	//All we're doing here is counting the incoming packets.
	Q_UNUSED(header)
	Q_UNUSED(payload)
	pidcount++;
}
void MainWindow::windowHidden(QMdiSubWindow* window)
{
	if (window && m_mdiSubWindowToActionMap.contains(window))
	{
		ui.menuOpen_Windows->removeAction(m_mdiSubWindowToActionMap[window]);
		m_mdiSubWindowToActionMap.remove(window);
	}
}
void MainWindow::windowDestroyed(QObject *window)
{
	if (window && m_mdiSubWindowToActionMap.contains((QMdiSubWindow*)window))
	{
		ui.menuOpen_Windows->removeAction(m_mdiSubWindowToActionMap[(QMdiSubWindow*)window]);
		m_mdiSubWindowToActionMap.remove((QMdiSubWindow*)window);
	}
}

void MainWindow::bringToFrontAndShow()
{
	for (QMap<QMdiSubWindow*,QAction*>::const_iterator i=m_mdiSubWindowToActionMap.constBegin();i!=m_mdiSubWindowToActionMap.constEnd();i++)
	{
		if (i.value() == sender())
		{
			i.key()->show();
			QApplication::postEvent(i.key(), new QEvent(QEvent::Show));
			QApplication::postEvent(i.key(), new QEvent(QEvent::WindowActivate));
		}

	}
}

void MainWindow::subMdiWindowActivated(QMdiSubWindow* window)
{
	if (window && !m_mdiSubWindowToActionMap.contains(window))
	{
		if (window->isVisible())
		{
		//connect(window,SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates))
		QAction *action = ui.menuOpen_Windows->addAction(window->windowTitle());
		connect(action,SIGNAL(triggered()),this,SLOT(bringToFrontAndShow()));
		connect(window->widget(),SIGNAL(windowHiding(QMdiSubWindow*)),this,SLOT(windowHidden(QMdiSubWindow*)));
		connect(window,SIGNAL(destroyed(QObject*)),this,SLOT(windowDestroyed(QObject*)));
		m_mdiSubWindowToActionMap[window] = action;
		}
		//connect(action,SIGNAL(triggered(bool)),window,SLOT(setVisible(bool)));
		//connect(action,SIGNAL(triggered()),this,SLOT(bringToFrontAndShow()));
		QLOG_DEBUG() << "Window Activated New:" << window->windowTitle();
	}
	else if (window)
	{
		QLOG_DEBUG() << "Window Activated Old:" << window->windowTitle();
	}
}

MainWindow::~MainWindow()
{
	//Save the window state.
	/*	QMap<unsigned short,QWidget*> m_rawDataView;
	QMap<unsigned short,ConfigView*> m_configDataView;
	QMdiSubWindow *tablesMdiWindow;
	QMdiSubWindow *firmwareMetaMdiWindow;
	QMdiSubWindow *interrogateProgressMdiWindow;
	QMdiSubWindow *emsMdiWindow;
	QMdiSubWindow *flagsMdiWindow;
	QMdiSubWindow *gaugesMdiWindow;
	QMdiSubWindow *packetStatusMdiWindow;
	QMdiSubWindow *aboutMdiWindow;
	QMdiSubWindow *emsStatusMdiWindow;

	ParameterView *parameterView;
	QMdiSubWindow *parameterMdiWindow;*/
	QSettings windowsettings;
	windowsettings.beginWriteArray("rawwindows");
	int val = 0;
	for (QMap<unsigned short,QWidget*>::const_iterator i=m_rawDataView.constBegin();i!=m_rawDataView.constEnd();i++)
	{
		windowsettings.setArrayIndex(val++);
		windowsettings.setValue("window",i.key());
		QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(i.value()->parent());
		windowsettings.setValue("location",subwin->saveGeometry());
		windowsettings.setValue("hidden",subwin->isHidden());
		windowsettings.setValue("type",i.value()->metaObject()->className());

	}

	//tablesMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",tablesMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",tablesMdiWindow->isHidden());
	windowsettings.setValue("type","tablesMdiWindow");

	//firmwareMetaMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",firmwareMetaMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",firmwareMetaMdiWindow->isHidden());
	windowsettings.setValue("type","firmwareMetaMdiWindow");

	//interrogateProgressMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",interrogateProgressMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",interrogateProgressMdiWindow->isHidden());
	windowsettings.setValue("type","interrogateProgressMdiWindow");

	//emsMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",emsMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",emsMdiWindow->isHidden());
	windowsettings.setValue("type","emsMdiWindow");

	//flagsMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",flagsMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",flagsMdiWindow->isHidden());
	windowsettings.setValue("type","flagsMdiWindow");

	//gaugesMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",gaugesMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",gaugesMdiWindow->isHidden());
	windowsettings.setValue("type","gaugesMdiWindow");

	//packetStatusMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",packetStatusMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",packetStatusMdiWindow->isHidden());
	windowsettings.setValue("type","packetStatusMdiWindow");

	//aboutMdiWindow
	windowsettings.setArrayIndex(val++);
	windowsettings.setValue("location",aboutMdiWindow->saveGeometry());
	windowsettings.setValue("hidden",aboutMdiWindow->isHidden());
	windowsettings.setValue("type","aboutMdiWindow");

	//emsStatusMdiWindow
	//windowsettings.setArrayIndex(val++);
	//windowsettings.setValue("location",emsStatusMdiWindow->saveGeometry());
	////windowsettings.setValue("hidden",emsStatusMdiWindow->isHidden());
	//windowsettings.setValue("type","emsStatusMdiWindow");


	windowsettings.endArray();
	windowsettings.beginGroup("general");
	windowsettings.setValue("location",this->saveGeometry());
	windowsettings.setValue("isMaximized",this->isMaximized());
	windowsettings.endGroup();
	windowsettings.sync();

	emsComms->stop();
	emsComms->wait(1000);
	delete emsComms;
}
