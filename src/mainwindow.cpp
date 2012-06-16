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

#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "datafield.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSettings>
#include <tableview2d.h>

#define define2string_p(x) #x
#define define2string(x) define2string_p(x)
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

	m_currentRamLocationId=0;
	//populateDataFields();
	m_localRamDirty = false;
	m_deviceFlashDirty = false;
	ui.setupUi(this);
	this->setWindowTitle(QString("EMStudio ") + QString(define2string(GIT_COMMIT)));
	ui.actionDisconnect->setEnabled(false);
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_connectClicked()));
	connect(ui.actionDisconnect,SIGNAL(triggered()),this,SLOT(menu_disconnectClicked()));
	connect(ui.actionEMS_Info,SIGNAL(triggered()),this,SLOT(menu_windows_EmsInfoClicked()));
	connect(ui.actionGauges,SIGNAL(triggered()),this,SLOT(menu_windows_GaugesClicked()));
	connect(ui.actionTables,SIGNAL(triggered()),this,SLOT(menu_windows_TablesClicked()));
	connect(ui.actionFlags,SIGNAL(triggered()),this,SLOT(menu_windows_FlagsClicked()));
	connect(ui.actionExit_3,SIGNAL(triggered()),this,SLOT(close()));
	connect(ui.actionPacket_Status,SIGNAL(triggered()),this,SLOT(menu_windows_PacketStatusClicked()));
	connect(ui.actionAbout,SIGNAL(triggered()),this,SLOT(menu_aboutClicked()));
	//connect(ui.action_Raw_Data,SIGNAL(triggered()),this,SLOT(menu_window_rawDataClicked()));

	connect(ui.saveDataPushButton,SIGNAL(clicked()),this,SLOT(ui_saveDataButtonClicked()));

	emsInfo=0;
	dataTables=0;
	dataFlags=0;
	dataGauges=0;


	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
	connect(ui.loadLogPushButton,SIGNAL(clicked()),this,SLOT(loadLogButtonClicked()));
	connect(ui.playLogPushButton,SIGNAL(clicked()),this,SLOT(playLogButtonClicked()));
	connect(ui.pauseLogPushButton,SIGNAL(clicked()),this,SLOT(pauseLogButtonClicked()));
	connect(ui.stopLogPushButton,SIGNAL(clicked()),this,SLOT(stopLogButtonClicked()));


	//connect(ui.interByteDelaySpinBox,SIGNAL(valueChanged(int)),this,SLOT(interByteDelayChanged(int)));
	dataPacketDecoder = new DataPacketDecoder(this);
	connect(dataPacketDecoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(dataLogDecoded(QVariantMap)));
	//

	/*logLoader = new LogLoader(this);
	connect(logLoader,SIGNAL(endOfLog()),this,SLOT(logFinished()));
	connect(logLoader,SIGNAL(payloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(logLoader,SIGNAL(logProgress(qlonglong,qlonglong)),this,SLOT(logProgress(qlonglong,qlonglong)));
	*/
	emsComms = new FreeEmsComms(this);
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(firmwareVersion(QString)),this,SLOT(firmwareVersion(QString)));
	connect(emsComms,SIGNAL(interfaceVersion(QString)),this,SLOT(interfaceVersion(QString)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)));
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),this,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));
	connect(emsComms,SIGNAL(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,SLOT(ramBlockRetrieved(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)),this,SLOT(flashBlockRetrieved(unsigned short,QByteArray,QByteArray)));

	//connect(emsComms,SIGNAL(destroyed()),this,SLOT(dataTablesDestroyed()));

	emsInfo = new EmsInfoView();
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),emsInfo,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));

	emsInfo->setFirmwareVersion(m_firmwareVersion);
	emsInfo->setInterfaceVersion(m_interfaceVersion);
	connect(emsInfo,SIGNAL(displayLocationId(int,bool,int)),this,SLOT(emsInfoDisplayLocationId(int,bool,int)));


	emsMdiWindow = ui.mdiArea->addSubWindow(emsInfo);
	emsMdiWindow->setGeometry(emsInfo->geometry());
	emsMdiWindow->hide();
	emsMdiWindow->setWindowTitle("EMS Info");


	aboutView = new AboutView();
	aboutView->setHash(define2string(GIT_HASH));
	aboutView->setCommit(define2string(GIT_COMMIT));
	aboutMdiWindow = ui.mdiArea->addSubWindow(aboutView);
	aboutMdiWindow->setGeometry(aboutView->geometry());
	aboutMdiWindow->hide();
	aboutMdiWindow->setWindowTitle("About");

	dataGauges = new GaugeView();
	//connect(dataGauges,SIGNAL(destroyed()),this,SLOT(dataGaugesDestroyed()));
	gaugesMdiWindow = ui.mdiArea->addSubWindow(dataGauges);
	gaugesMdiWindow->setGeometry(dataGauges->geometry());
	gaugesMdiWindow->hide();
	gaugesMdiWindow->setWindowTitle("Gauges");

	dataTables = new TableView();
	//connect(dataTables,SIGNAL(destroyed()),this,SLOT(dataTablesDestroyed()));
	dataTables->passDecoder(dataPacketDecoder);
	tablesMdiWindow = ui.mdiArea->addSubWindow(dataTables);
	tablesMdiWindow->setGeometry(dataTables->geometry());
	tablesMdiWindow->hide();
	tablesMdiWindow->setWindowTitle("Data Tables");

	dataFlags = new FlagView();
	//connect(dataFlags,SIGNAL(destroyed()),this,SLOT(dataFlagsDestroyed()));
	dataFlags->passDecoder(dataPacketDecoder);
	flagsMdiWindow = ui.mdiArea->addSubWindow(dataFlags);
	flagsMdiWindow->setGeometry(dataFlags->geometry());
	flagsMdiWindow->hide();
	flagsMdiWindow->setWindowTitle("Flags");

	packetStatus = new PacketStatusView();
	connect(emsComms,SIGNAL(packetSent(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketSent(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetAcked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketAck(unsigned short,QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray,unsigned short)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray,unsigned short)));
	connect(emsComms,SIGNAL(decoderFailure(QByteArray)),packetStatus,SLOT(passDecoderFailure(QByteArray)));
	packetStatusMdiWindow = ui.mdiArea->addSubWindow(packetStatus);
	packetStatusMdiWindow->setGeometry(packetStatus->geometry());
	packetStatusMdiWindow->hide();
	packetStatusMdiWindow->setWindowTitle("Packet Status");



	//Load settings
	QSettings settings("freeems","freetune");
	settings.beginGroup("comms");
	m_comPort = settings.value("port","/dev/ttyUSB0").toString();
	m_comBaud = settings.value("baud",115200).toInt();
	m_comInterByte = settings.value("interbytedelay",0).toInt();
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
	emsComms->start();

	logfile = new QFile("myoutput.log");
	logfile->open(QIODevice::ReadWrite | QIODevice::Truncate);

	menu_connectClicked(); //Connect on start.

	QByteArray data;
	for (int i=0;i<16;i++)
	{

		data.append((char)(((i * 1000) >> 8) & 0xFF));
		data.append((i * 1000) & 0xFF);
	}

	for (int i=0;i<16;i++)
	{
		data.append((char)0x00);
		data.append((char)((unsigned char)rand()));
	}

	TableView2D *view = new TableView2D();
	view->passData(0xABCD,data,0);
	connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
	connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
	QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
	win->setWindowTitle("Ram Location 0x" + QString::number(0xABCD,16).toUpper());
	win->setGeometry(view->geometry());
	m_rawDataView[0xABCD] = view;
	win->show();
	win->raise();

}
void MainWindow::dataViewSaveLocation(unsigned short locationid,QByteArray data,int physicallocation)
{
	if (physicallocation == 0)
	{
		//RAM
		emsComms->updateBlockInRam(locationid,0,data.size(),data);
		for (int i=0;i<m_ramMemoryList.size();i++)
		{
			if (m_ramMemoryList[i]->locationid == locationid)
			{
				m_ramMemoryList[i]->setData(data);
			}
		}
	}
	else if (physicallocation == 1)
	{
		//FLASH
		emsComms->updateBlockInFlash(locationid,0,data.size(),data);
		for (int i=0;i<m_flashMemoryList.size();i++)
		{
			if (m_flashMemoryList[i]->locationid == locationid)
			{
				m_flashMemoryList[i]->setData(data);
			}
		}
	}
}
void MainWindow::menu_aboutClicked()
{
	if (aboutMdiWindow->isVisible())
	{
		aboutMdiWindow->hide();
	}
	else
	{
		aboutMdiWindow->show();
	}
}
void MainWindow::menu_windows_PacketStatusClicked()
{
	if (packetStatusMdiWindow->isVisible())
	{
		packetStatusMdiWindow->hide();
	}
	else
	{
		packetStatusMdiWindow->show();
	}
}

void MainWindow::emsInfoDisplayLocationId(int locid,bool isram,int type)
{
	Q_UNUSED(type)
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == locid)
		{
			if (m_rawDataView.contains(locid))
			{
				if (type != 1)
				{

					qobject_cast<RawDataView*>(m_rawDataView[locid])->setData(locid,m_ramMemoryList[i]->data());
					m_rawDataView[locid]->show();
					m_rawDataView[locid]->raise();
				}
			}
			else
			{
				if (type == 1)
				{
					qDebug() << "Creating new table view for location: 0x" << QString::number(locid,16).toUpper();
					TableView2D *view = new TableView2D();
					view->passData(locid,m_ramMemoryList[i]->data(),0);
					connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
					connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
					connect(view,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SLOT(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
					connect(view,SIGNAL(saveToFlash(unsigned short)),this,SLOT(saveFlashLocationId(unsigned short)));
					QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
					win->setWindowTitle("Ram Location 0x" + QString::number(locid,16).toUpper());
					win->setGeometry(view->geometry());
					m_rawDataView[locid] = view;
					win->show();
					win->raise();
				}
				else
				{
					RawDataView *view = new RawDataView();
					view->setData(locid,m_ramMemoryList[i]->data());
					connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
					connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
					QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
					win->setWindowTitle("Ram Location: 0x" + QString::number(locid,16).toUpper());
					win->setGeometry(view->geometry());
					m_rawDataView[locid] = view;
					win->show();
					win->raise();
				}
			}
			return;
		}
	}

	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == locid)
		{
			if (m_rawDataView.contains(locid))
			{
				if (type != -1)
				{
					qobject_cast<RawDataView*>(m_rawDataView[locid])->setData(locid,m_flashMemoryList[i]->data());
					//m_rawDataView[locid]->setData(locid,m_ramRawBlockList[i]->data);
					m_rawDataView[locid]->show();
					m_rawDataView[locid]->raise();
				}
			}
			else
			{
				if (type == 1)
				{
					TableView2D *view = new TableView2D();
					view->passData(locid,m_flashMemoryList[i]->data(),0);
					connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
					connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
					connect(view,SIGNAL(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)),this,SLOT(saveSingleData(unsigned short,QByteArray,unsigned short,unsigned short)));
					QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
					win->setWindowTitle("Flash Location 0x" + QString::number(locid,16).toUpper());
					win->setGeometry(view->geometry());
					m_rawDataView[locid] = view;
					win->show();
					win->raise();
				}
				else
				{
					RawDataView *view = new RawDataView();
					view->setData(locid,m_flashMemoryList[i]->data());
					connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
					connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
					QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
					win->setWindowTitle("Flash Location: 0x" + QString::number(locid,16).toUpper());
					win->setGeometry(view->geometry());
					m_rawDataView[locid] = view;
					win->show();
					win->raise();
				}
			}
			return;
		}
	}
}
void MainWindow::rawViewSaveData(unsigned short locationid,QByteArray data,int physicallocation)
{
	markRamDirty();
	bool found = false;
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == locationid)
		{
			if (m_ramMemoryList[i]->data() == data)
			{
				qDebug() << "Data in application memory unchanged, no reason to send write";
				return;
			}
			m_ramMemoryList[i]->setData(data);
			found = true;
		}
	}
	if (!found)
	{
		qDebug() << "Attempted to save data for location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Ram list. Ram list size:" << m_ramMemoryList.size();
	}
	qDebug() << "Requesting to update ram location:" << "0x" + QString::number(locationid,16).toUpper() << "data size:" << data.size();
	m_currentRamLocationId = locationid;
	emsComms->updateBlockInRam(locationid,0,data.size(),data);
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
QByteArray MainWindow::getLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return m_ramMemoryList[i]->data();
		}
	}
	return QByteArray();
}

QByteArray MainWindow::getLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return m_flashMemoryList[i]->data();
		}
	}
	return QByteArray();
}
QByteArray MainWindow::getDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->locationid == id)
		{
			return m_deviceRamMemoryList[i]->data();
		}
	}
	return QByteArray();
}

bool MainWindow::hasDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
bool MainWindow::hasLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}

bool MainWindow::hasLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}
void MainWindow::ramBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	if (!hasDeviceRamBlock(locationid))
	{
		//This should not happen
		/*RawDataBlock *block = new RawDataBlock();
		block->locationid = locationid;
		block->header = header;
		block->data = payload;
		//m_flashRawBlockList.append(block);
		m_deviceRamRawBlockList.append(block);*/
	}
	else
	{
		for (int i=0;i<m_deviceRamMemoryList.size();i++)
		{
			if (m_deviceRamMemoryList[i]->locationid == locationid)
			{
				if (m_deviceRamMemoryList[i]->isEmpty)
				{
					//Initial retrieval.
					m_deviceRamMemoryList[i]->setData(payload);
				}
				else
				{
					if (getDeviceRamBlock(locationid) != payload)
					{
						qDebug() << "Ram block on device does not match ram block on tuner! This should not happen!";
						qDebug() << "Tuner ram size:" << m_deviceRamMemoryList[i]->data().size();
						m_deviceRamMemoryList[i]->setData(payload);
					}
				}

			}
		}

	}
	return;
}


void MainWindow::flashBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->locationid == locationid)
		{
			if (m_flashMemoryList[i]->isEmpty)
			{
				m_flashMemoryList[i]->setData(payload);
			}
			else
			{
				if (getLocalFlashBlock(locationid) != payload)
				{
					qDebug() << "Flash block in memory does not match flash block on tuner! This should not happen!";
					qDebug() << "Flash size:" << m_flashMemoryList[i]->data().size();
					m_flashMemoryList[i]->setData(payload);
				}
			}
		}
	}
	return;
}

void MainWindow::ui_saveDataButtonClicked()
{

}

void MainWindow::menu_settingsClicked()
{
	ComSettings *settings = new ComSettings();
	settings->setComPort(m_comPort);
	settings->setBaud(m_comBaud);
	connect(settings,SIGNAL(saveClicked()),this,SLOT(settingsSaveClicked()));
	connect(settings,SIGNAL(cancelClicked()),this,SLOT(settingsCancelClicked()));
	QMdiSubWindow *win = ui.mdiArea->addSubWindow(settings);
	win->setGeometry(settings->geometry());
	win->show();
	settings->show();
}

void MainWindow::menu_connectClicked()
{
	ui.actionConnect->setEnabled(false);
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
	/*if (!subwin)
	{
		subwin->deleteLater();
	}*/
	comSettingsWidget->hide();
	QSettings settings("freeems","freetune");
	settings.beginGroup("comms");
	settings.setValue("port",m_comPort);
	settings.setValue("baud",m_comBaud);
	settings.setValue("interbytedelay",m_comInterByte);
	settings.endGroup();
	QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow*>(comSettingsWidget->parent());
	ui.mdiArea->removeSubWindow(subwin);
	comSettingsWidget->deleteLater();

}
void MainWindow::locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size)
{
	Q_UNUSED(size)
	Q_UNUSED(rawFlags)
	Q_UNUSED(parent)
	Q_UNUSED(rampage)
	Q_UNUSED(flashpage)
	Q_UNUSED(ramaddress)
	Q_UNUSED(flashaddress)

	if (flags.contains(FreeEmsComms::BLOCK_IS_RAM) && flags.contains((FreeEmsComms::BLOCK_IS_FLASH)))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isRam = true;
		loc->isFlash = true;
		loc->ramAddress = ramaddress;
		loc->ramPage = rampage;
		loc->flashAddress = flashaddress;
		loc->flashPage = flashpage;
		m_deviceRamMemoryList.append(loc);
		m_flashMemoryList.append(new MemoryLocation(*loc));
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isFlash = true;
		loc->isRam = false;
		loc->flashAddress = flashaddress;
		loc->flashPage = flashpage;
		m_flashMemoryList.append(loc);
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = locationid;
		loc->size = size;
		if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
		{
			loc->parent = parent;
			loc->hasParent = true;
		}
		loc->isRam = true;
		loc->isFlash = false;
		loc->ramAddress = ramaddress;
		loc->ramPage = rampage;
		m_deviceRamMemoryList.append(loc);
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
void MainWindow::menu_windows_GaugesClicked()
{
	if (gaugesMdiWindow->isVisible())
	{
		gaugesMdiWindow->hide();
	}
	else
	{
		gaugesMdiWindow->show();
	}
}

void MainWindow::menu_windows_EmsInfoClicked()
{
	if (emsMdiWindow->isVisible())
	{
		emsMdiWindow->hide();
	}
	else
	{
		emsMdiWindow->show();
	}
}

void MainWindow::menu_windows_TablesClicked()
{
	if (tablesMdiWindow->isVisible())
	{
		tablesMdiWindow->hide();
	}
	else
	{
		tablesMdiWindow->show();
	}
}
void MainWindow::menu_windows_FlagsClicked()
{
	if (flagsMdiWindow->isVisible())
	{
		flagsMdiWindow->hide();
	}
	else
	{
		flagsMdiWindow->show();
	}
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
	for (int i=0;i<idlist.size();i++)
	{
		//ui/listWidget->addItem(QString::number(idlist[i]));
		MemoryLocation *loc = new MemoryLocation();
		loc->locationid = idlist[i];
		m_tempMemoryList.append(loc);
		m_locIdMsgList.append(emsComms->getLocationIdInfo(idlist[i]));
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
}
void MainWindow::interfaceVersion(QString version)
{
	//ui.interfaceVersionLineEdit->setText(version);
	m_interfaceVersion = version;
	if (emsInfo)
	{
		emsInfo->setInterfaceVersion(version);
	}
}
void MainWindow::firmwareVersion(QString version)
{
	//ui.firmwareVersionLineEdit->setText(version);
	m_firmwareVersion = version;
	if (emsInfo)
	{
		emsInfo->setFirmwareVersion(version);
	}
}
void MainWindow::error(QString msg)
{
	Q_UNUSED(msg)
}
void MainWindow::emsCommsConnected()
{
	emsComms->getFirmwareVersion();
	emsComms->getInterfaceVersion();
	emsComms->getLocationIdList(0x00,0x00);
}
void MainWindow::checkSyncRequest()
{
	emsComms->getLocationIdList(0,0);
}
void MainWindow::updateRamLocation(unsigned short locationid)
{
	bool hasparent = false;
	unsigned short tempRamParentId;
	bool isparent = false;
	QList<unsigned short> childlist;
	for (int j=0;j<m_ramMemoryList.size();j++)
	{
		if (m_ramMemoryList[j]->parent == locationid)
		{
			//qDebug() << "Child of:" << "0x" + QString::number(m_currentRamLocationId,16).toUpper() << "is" << "0x" + QString::number(m_ramMemoryList[j]->locationid,16).toUpper();
			childlist.append(m_ramMemoryList[j]->locationid);
			isparent = true;
		}
		if (m_ramMemoryList[j]->locationid == locationid)
		{
			if (m_ramMemoryList[j]->hasParent)
			{
				hasparent = true;
				tempRamParentId = m_ramMemoryList[j]->parent;
			}
			for (int i=0;i<m_deviceRamMemoryList.size();i++)
			{
				if (m_deviceRamMemoryList[i]->locationid == locationid)
				{
					m_deviceRamMemoryList[i]->setData(m_ramMemoryList[j]->data());
				}
			}
		}
	}
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
	else
	{
		//qDebug() << "No children for:" << "0x" + QString::number(m_currentRamLocationId,16).toUpper();
		updateDataWindows(locationid);
	}
}

void MainWindow::commandSuccessful(int sequencenumber)
{
	qDebug() << "Command succesful:" << QString::number(sequencenumber);
	if (m_currentRamLocationId != 0)
	{
		updateRamLocation(m_currentRamLocationId);
		checkRamFlashSync();
		m_currentRamLocationId=0;
		return;
	}
	if (m_locIdInfoMsgList.contains(sequencenumber))
	{
		m_locIdInfoMsgList.removeOne(sequencenumber);
		if (m_locIdInfoMsgList.size() == 0)
		{
			//End of the location ID information messages.
			checkRamFlashSync();
		}
	}
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			populateParentLists();
			for (int i=0;i<m_flashMemoryList.size();i++)
			{
				if (!m_flashMemoryList[i]->hasParent)
				{
					m_locIdInfoMsgList.append(emsComms->retrieveBlockFromFlash(m_flashMemoryList[i]->locationid,0,0));
				}
			}
			for (int i=0;i<m_deviceRamMemoryList.size();i++)
			{
				if (!m_deviceRamMemoryList[i]->hasParent)
				{
					m_locIdInfoMsgList.append(emsComms->retrieveBlockFromRam(m_deviceRamMemoryList[i]->locationid,0,0));
				}
			}
		}
	}
}
void MainWindow::updateDataWindows(unsigned short locationid)
{
	if (m_rawDataView.contains(locationid))
	{
		RawDataView *rawview = qobject_cast<RawDataView*>(m_rawDataView[locationid]);
		if (rawview)
		{
			rawview->setData(locationid,getLocalRamBlock(locationid));
		}
		else
		{
			TableView2D *tableview = qobject_cast<TableView2D*>(m_rawDataView[locationid]);
			if (tableview)
			{
				tableview->passData(locationid,getLocalRamBlock(locationid),0);
			}
			else
			{
				qDebug() << "GUI Window open with memory location, but no valid window type found!";
			}
		}
	}
	else
	{
		//qDebug() << "Attempted to update a window that does not exist!" << "0x" + QString::number(locationid,16).toUpper();
	}
}

void MainWindow::checkRamFlashSync()
{
	if (m_ramMemoryList.size() == 0)
	{
		//Internal ram list is empty. Let's fill it.
		for (int i=0;i<m_deviceRamMemoryList.size();i++)
		{
			m_ramMemoryList.append(new MemoryLocation(*m_deviceRamMemoryList[i]));
		}
		//We have to do something special here, since m_ramMemoryList's parents point to m_deviceRamMemoryList.
		for (int i=0;i<m_ramMemoryList.size();i++)
		{
			if (m_ramMemoryList[i]->hasParent)
			{
				for (int j=0;j<m_ramMemoryList.size();j++)
				{
					if (m_ramMemoryList[i]->parent== m_ramMemoryList[j]->locationid)
					{
						m_ramMemoryList[i]->setParent(m_ramMemoryList[j]);
					}
				}
			}
		}
		return;
	}

}

void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	qDebug() << "Command failed:" << QString::number(sequencenumber) << "0x" + QString::number(errornum,16);
	if (m_currentRamLocationId != 0)
	{
		for (int i=0;i<m_ramMemoryList.size();i++)
		{
			if (m_ramMemoryList[i]->locationid == m_currentRamLocationId)
			{
				for (int j=0;j<m_deviceRamMemoryList.size();j++)
				{
					if (m_deviceRamMemoryList[j]->locationid == m_currentRamLocationId)
					{
						qDebug() << "Data reverting for location id 0x" + QString::number(m_ramMemoryList[i]->locationid,16);
						if (m_ramMemoryList[i]->data() == m_deviceRamMemoryList[j]->data())
						{
							if (m_ramMemoryList[i] == m_deviceRamMemoryList[j])
							{
								qDebug() << "Ram memory list and Device memory list are using the same pointer! This should NOT happen!!";
							}
							qDebug() << "Data valid. No need for a revert.";
						}
						else
						{
							qDebug() << "Invalid data, reverting...";
							m_ramMemoryList[i]->setData(m_deviceRamMemoryList[j]->data());
							if (m_ramMemoryList[i]->data() != m_deviceRamMemoryList[j]->data())
							{
								qDebug() << "Failed to revert!!!";
							}
							updateRamLocation(m_currentRamLocationId);
						}
						break;
					}
				}
				break;
			}
		}
		//Find all windows that use that location id
		m_currentRamLocationId = 0;
		//checkRamFlashSync();
	}
	if (m_locIdInfoMsgList.contains(sequencenumber))
	{
		m_locIdInfoMsgList.removeOne(sequencenumber);
		if (m_locIdInfoMsgList.size() == 0)
		{
			qDebug() << "All Ram and Flash locations updated";
			//End of the location ID information messages.
			checkRamFlashSync();
		}
	}
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			qDebug() << "All ID information recieved. Requesting Ram and Flash updates";
			populateParentLists();
			for (int i=0;i<m_flashMemoryList.size();i++)
			{
				if (!m_flashMemoryList[i]->hasParent)
				{
					m_locIdInfoMsgList.append(emsComms->retrieveBlockFromFlash(m_flashMemoryList[i]->locationid,0,0));
				}
			}
			for (int i=0;i<m_deviceRamMemoryList.size();i++)
			{
				if (!m_deviceRamMemoryList[i]->hasParent)
				{
					m_locIdInfoMsgList.append(emsComms->retrieveBlockFromRam(m_deviceRamMemoryList[i]->locationid,0,0));
				}
			}
		}
	}

}
void MainWindow::populateParentLists()
{
	//Need to get a list of all IDs here now.
	qDebug() << "Populating internal memory parent list.";
	qDebug() << m_flashMemoryList.size() << "flash locations";
	qDebug() << m_deviceRamMemoryList.size() << "Device Ram locations";
	qDebug() << m_ramMemoryList.size() << "Application Ram locations";
	for (int i=0;i<m_flashMemoryList.size();i++)
	{
		if (m_flashMemoryList[i]->hasParent && m_flashMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_flashMemoryList.size();j++)
			{
				if (m_flashMemoryList[i]->parent== m_flashMemoryList[j]->locationid)
				{
					m_flashMemoryList[i]->setParent(m_flashMemoryList[j]);
				}
			}
		}
	}
	for (int i=0;i<m_deviceRamMemoryList.size();i++)
	{
		if (m_deviceRamMemoryList[i]->hasParent && m_deviceRamMemoryList[i]->getParent() == 0)
		{
			for (int j=0;j<m_deviceRamMemoryList.size();j++)
			{
				if (m_deviceRamMemoryList[i]->parent== m_deviceRamMemoryList[j]->locationid)
				{
					m_deviceRamMemoryList[i]->setParent(m_deviceRamMemoryList[j]);
				}
			}
		}
	}



}

void MainWindow::pauseLogButtonClicked()
{

}
void MainWindow::saveFlashLocationId(unsigned short locationid)
{
	qDebug() << "Burning block from ram to flash for locationid:" << "0x"+QString::number(locationid,16).toUpper();
	emsComms->burnBlockFromRamToFlash(locationid,0,0);
}

void MainWindow::saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size)
{
	bool found = false;
	for (int i=0;i<m_ramMemoryList.size();i++)
	{
		if (m_ramMemoryList[i]->locationid == locationid)
		{
			if (m_ramMemoryList[i]->data().mid(offset,size) == data)
			{
				qDebug() << "Data in application memory unchanged, no reason to send write for single value";
				return;
			}

			m_ramMemoryList[i]->setData(m_ramMemoryList[i]->data().replace(offset,size,data));
			found = true;
		}
	}
	if (!found)
	{
		qDebug() << "Attempted to save data for single value at location id:" << "0x" + QString::number(locationid,16) << "but no valid location found in Ram list. Ram list size:" << m_ramMemoryList.size();
	}
	qDebug() << "Requesting to update single value at ram location:" << "0x" + QString::number(locationid,16).toUpper() << "data size:" << data.size();
	m_currentRamLocationId = locationid;
	emsComms->updateBlockInRam(locationid,offset,size,data);
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
	dataPacketDecoder->decodePayload(payload);
	//guiUpdateTimerTick();

}

MainWindow::~MainWindow()
{
	emsComms->terminate();
	emsComms->wait(1000);
	delete emsComms;
}
