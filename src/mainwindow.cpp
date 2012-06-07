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
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	//populateDataFields();
	m_localRamDirty = false;
	m_localFlashDirty = false;
	m_deviceFlashDirty = false;
	ui.setupUi(this);
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
	//connect(ui.action_Raw_Data,SIGNAL(triggered()),this,SLOT(menu_window_rawDataClicked()));

	connect(ui.saveDataPushButton,SIGNAL(clicked()),this,SLOT(ui_saveDataButtonClicked()));
	//comSettings = new ComSettings();
	//connect(comSettings,SIGNAL(saveClicked()),this,SLOT(settingsSaveClicked()));
	//connect(comSettings,SIGNAL(cancelClicked()),this,SLOT(settingsCancelClicked()));
	//QMdiSubWindow *win = ui.mdiArea->addSubWindow(comSettings);
	//win->setGeometry(comSettings->geometry());
	//win->show();

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
	connect(emsComms,SIGNAL(packetNaked(unsigned short,QByteArray,QByteArray)),packetStatus,SLOT(passPacketNak(unsigned short,QByteArray,QByteArray)));
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


}
void MainWindow::dataViewSaveLocation(unsigned short locationid,QByteArray data,int physicallocation)
{
	if (physicallocation == 0)
	{
		//RAM
		emsComms->updateBlockInRam(locationid,0,data.size(),data);
	}
	else if (physicallocation == 1)
	{
		//FLASH
		emsComms->updateBlockInFlash(locationid,0,data.size(),data);
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
	for (int i=0;i<m_ramRawBlockList.size();i++)
	{
		if (m_ramRawBlockList[i]->locationid == locid)
		{
			if (m_rawDataView.contains(locid))
			{
				m_rawDataView[locid]->show();
				m_rawDataView[locid]->setData(locid,m_ramRawBlockList[i]->data);
				m_rawDataView[locid]->show();
			}
			else
			{
				RawDataView *view = new RawDataView();
				view->setData(locid,m_ramRawBlockList[i]->data);
				connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
				connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
				QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
				win->setGeometry(view->geometry());
				m_rawDataView[locid] = view;
				win->show();
			}
			return;
		}
	}
	for (int i=0;i<m_flashRawBlockList.size();i++)
	{
		if (m_ramRawBlockList[i]->locationid == locid)
		{
			if (m_rawDataView.contains(locid))
			{
				m_rawDataView[locid]->show();
				m_rawDataView[locid]->setData(locid,m_ramRawBlockList[i]->data);
				m_rawDataView[locid]->show();
			}
			else
			{
				RawDataView *view = new RawDataView();
				view->setData(locid,m_flashRawBlockList[i]->data);
				connect(view,SIGNAL(destroyed(QObject*)),this,SLOT(rawDataViewDestroyed(QObject*)));
				connect(view,SIGNAL(saveData(unsigned short,QByteArray,int)),this,SLOT(rawViewSaveData(unsigned short,QByteArray,int)));
				QMdiSubWindow *win = ui.mdiArea->addSubWindow(view);
				win->setGeometry(view->geometry());
				m_rawDataView[locid] = view;
				win->show();
			}
			return;
		}
	}
}
void MainWindow::rawViewSaveData(unsigned short locationid,QByteArray data,int physicallocation)
{
	for (int i=0;i<m_flashRawBlockList.size();i++)
	{
		if (m_flashRawBlockList[i]->locationid == locationid)
		{
			if (m_flashRawBlockList[i]->data != data)
			{
				markFlashDirty();
			}
		}
	}
	emsComms->updateBlockInRam(locationid,0,data.size(),data);
}

void MainWindow::rawDataViewDestroyed(QObject *object)
{
	QMap<unsigned short,RawDataView*>::const_iterator i = m_rawDataView.constBegin();
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
void MainWindow::markFlashDirty()
{
	m_localFlashDirty = true;
	emsInfo->setLocalFlash(true);
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
void MainWindow::markFlashClean()
{
	m_localFlashDirty = false;
	emsInfo->setLocalFlash(false);
}
void MainWindow::markDeviceFlashClean()
{
	m_deviceFlashDirty = false;
	emsInfo->setDeviceFlash(false);
}
QByteArray MainWindow::getLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramRawBlockList.size();i++)
	{
		if (m_ramRawBlockList[i]->locationid == id)
		{
			return m_ramRawBlockList[i]->data;
		}
	}
	return QByteArray();
}

QByteArray MainWindow::getLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashRawBlockList.size();i++)
	{
		if (m_flashRawBlockList[i]->locationid == id)
		{
			return m_flashRawBlockList[i]->data;
		}
	}
	return QByteArray();
}
QByteArray MainWindow::getDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamRawBlockList.size();i++)
	{
		if (m_deviceRamRawBlockList[i]->locationid == id)
		{
			return m_deviceRamRawBlockList[i]->data;
		}
	}
	return QByteArray();
}

QByteArray MainWindow::getDeviceFlashBlock(unsigned short id)
{
	for (int i=0;i<m_deviceFlashRawBlockList.size();i++)
	{
		if (m_deviceFlashRawBlockList[i]->locationid == id)
		{
			return m_deviceFlashRawBlockList[i]->data;
		}
	}
	return QByteArray();
}
bool MainWindow::hasDeviceRamBlock(unsigned short id)
{
	for (int i=0;i<m_deviceRamRawBlockList.size();i++)
	{
		if (m_deviceRamRawBlockList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}

bool MainWindow::hasDeviceFlashBlock(unsigned short id)
{
	for (int i=0;i<m_deviceFlashRawBlockList.size();i++)
	{
		if (m_deviceFlashRawBlockList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}

bool MainWindow::hasLocalRamBlock(unsigned short id)
{
	for (int i=0;i<m_ramRawBlockList.size();i++)
	{
		if (m_ramRawBlockList[i]->locationid == id)
		{
			return true;
		}
	}
	return false;
}

bool MainWindow::hasLocalFlashBlock(unsigned short id)
{
	for (int i=0;i<m_flashRawBlockList.size();i++)
	{
		if (m_flashRawBlockList[i]->locationid == id)
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
		RawDataBlock *block = new RawDataBlock();
		block->locationid = locationid;
		block->header = header;
		block->data = payload;
		//m_flashRawBlockList.append(block);
		m_deviceRamRawBlockList.append(block);
	}
	else
	{
		if (getDeviceRamBlock(locationid) != payload)
		{
			qDebug() << "Ram block on device does not match ram block on tuner! This should not happen!";
		}
	}
	return;
}


void MainWindow::flashBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	if (!hasDeviceFlashBlock(locationid))
	{
		RawDataBlock *block = new RawDataBlock();
		block->locationid = locationid;
		block->header = header;
		block->data = payload;
		//m_flashRawBlockList.append(block);
		m_deviceFlashRawBlockList.append(block);
	}
	else
	{
		if (getDeviceFlashBlock(locationid) != payload)
		{
			qDebug() << "Flash block on device does not match flash block on tuner! This should not happen!";
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
}

void MainWindow::menu_disconnectClicked()
{
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
	comSettingsWidget->hide();
	QSettings settings("freeems","freetune");
	settings.beginGroup("comms");
	settings.setValue("port",m_comPort);
	settings.setValue("baud",m_comBaud);
	settings.setValue("interbytedelay",m_comInterByte);
	settings.endGroup();
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
	if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		m_locIdMsgList.append(emsComms->retrieveBlockFromRam(locationid,0,0));
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		m_locIdMsgList.append(emsComms->retrieveBlockFromFlash(locationid,0,0));
	}
}

void MainWindow::settingsCancelClicked()
{
	//comSettings->hide();
	ComSettings *comSettingsWidget = qobject_cast<ComSettings*>(sender());
	comSettingsWidget->hide();
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
		emsComms->getLocationIdInfo(idlist[i]);
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

void MainWindow::commandSuccessful(int sequencenumber)
{
	qDebug() << "command succesful:" << QString::number(sequencenumber);
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			//End of the location ID information messages.
			checkRamFlashSync();
		}
	}
}
void MainWindow::checkRamFlashSync()
{
	if (m_ramRawBlockList.size() == 0 && m_flashRawBlockList.size() == 0)
	{
		//Inital check, populate.
		for (int i=0;i<m_deviceRamRawBlockList.size();i++)
		{
			m_ramRawBlockList.append(m_deviceRamRawBlockList[i]);
			if (hasDeviceFlashBlock(m_deviceRamRawBlockList[i]->locationid))
			{
				if (getDeviceFlashBlock(m_deviceRamRawBlockList[i]->locationid) != m_deviceRamRawBlockList[i]->data)
				{
					//Device ram is out of sync with device flash.
					markDeviceFlashDirty();
				}
			}
		}
		for (int i=0;i<m_deviceFlashRawBlockList.size();i++)
		{
			m_flashRawBlockList.append(m_deviceFlashRawBlockList[i]);
		}
	}
	else
	{
		bool localRamDirty = false;
		bool localFlashDirty = false;
		bool deviceFlashDirty = false;
		for (int i=0;i<m_ramRawBlockList.size();i++)
		{
			if (hasDeviceRamBlock(m_ramRawBlockList[i]->locationid))
			{
				if (getDeviceRamBlock(m_ramRawBlockList[i]->locationid) != m_ramRawBlockList[i]->data)
				{
					//Device ram is out of sync with local ram.
					//markDeviceFlashDirty();
					markRamDirty();
					localRamDirty=true;
				}
				else
				{
					if (hasDeviceFlashBlock(m_ramRawBlockList[i]->locationid))
					{
						if (getDeviceFlashBlock(m_ramRawBlockList[i]->locationid) != m_ramRawBlockList[i]->data)
						{
							markDeviceFlashDirty();
							deviceFlashDirty=true;
						}
					}
				}
			}
			else
			{
				//Device ram does not have a location that local ram has. This should never happen.
				qDebug() << "Error: Device ram does not have the location found in local ram!" << m_ramRawBlockList[i]->locationid;
			}
			if (hasLocalFlashBlock(m_ramRawBlockList[i]->locationid))
			{
				if (getLocalFlashBlock(m_ramRawBlockList[i]->locationid) != m_ramRawBlockList[i]->data)
				{
					markFlashDirty();
					localFlashDirty = true;
				}
			}
			else
			{
				//Local does not have a flash location to match this ram location. This is normal.
			}
		}
		if (!localFlashDirty)
		{
			markFlashClean();
		}
		if (!localRamDirty)
		{
			markRamClean();
		}
		if (!deviceFlashDirty)
		{
			markDeviceFlashClean();
		}
	}
}

void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	qDebug() << "command failed:" << QString::number(sequencenumber) << QString::number(errornum,16);
	if (m_locIdMsgList.contains(sequencenumber))
	{
		m_locIdMsgList.removeOne(sequencenumber);
		if (m_locIdMsgList.size() == 0)
		{
			//End of the location ID information messages.
			checkRamFlashSync();
		}
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
