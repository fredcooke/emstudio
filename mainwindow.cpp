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
	ui.setupUi(this);
	ui.actionDisconnect->setEnabled(false);
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_connectClicked()));
	connect(ui.actionDisconnect,SIGNAL(triggered()),this,SLOT(menu_disconnectClicked()));
	connect(ui.actionEMS_Info,SIGNAL(triggered()),this,SLOT(menu_windows_EmsInfoClicked()));
	connect(ui.actionGauges,SIGNAL(triggered()),this,SLOT(menu_windows_GaugesClicked()));
	connect(ui.actionTables,SIGNAL(triggered()),this,SLOT(menu_windows_TablesClicked()));
	connect(ui.actionFlags,SIGNAL(triggered()),this,SLOT(menu_windows_FlagsClicked()));
	//comSettings = new ComSettings();
	//connect(comSettings,SIGNAL(saveClicked()),this,SLOT(settingsSaveClicked()));
	//connect(comSettings,SIGNAL(cancelClicked()),this,SLOT(settingsCancelClicked()));
	//QMdiSubWindow *win = ui.mdiArea->addSubWindow(comSettings);
	//win->setGeometry(comSettings->geometry());
	//win->show();

	emsInfo=0;

	dataTables=0;
	dataFlags=0;

	/*dataGauges = new DataGauges();
	QMdiSubWindow *win4 = ui.mdiArea->addSubWindow(dataGauges);
	win4->setGeometry(dataGauges->geometry());
	win4->show();*/
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

void MainWindow::settingsCancelClicked()
{
	//comSettings->hide();
	ComSettings *comSettingsWidget = qobject_cast<ComSettings*>(sender());
	comSettingsWidget->hide();
	comSettingsWidget->deleteLater();
}
void MainWindow::menu_windows_GaugesClicked()
{
	if (dataGauges)
	{
		//QMdiSubWindow *win;
		//win->widget()
		for (int i=0;i<ui.mdiArea->subWindowList().size();i++)
		{
			if (ui.mdiArea->subWindowList()[i]->widget() == dataGauges)
			{
				ui.mdiArea->removeSubWindow(ui.mdiArea->subWindowList()[i]);
				break;
			}
		}
		dataGauges->hide();
		dataGauges->deleteLater();
		dataGauges = 0;
	}
	else
	{
		dataGauges = new DataGauges();
		connect(dataGauges,SIGNAL(destroyed()),this,SLOT(dataGaugesDestroyed()));
		QMdiSubWindow *win = ui.mdiArea->addSubWindow(dataGauges);
		win->setGeometry(dataGauges->geometry());
		win->show();
	}
}
void MainWindow::dataTablesDestroyed()
{
	dataTables=0;
}

void MainWindow::dataGaugesDestroyed()
{
	dataGauges=0;
}
void MainWindow::dataFlagsDestroyed()
{
	dataFlags = 0;
}

void MainWindow::menu_windows_EmsInfoClicked()
{
	if (emsInfo)
	{
		//QMdiSubWindow *win;
		//win->widget()
		for (int i=0;i<ui.mdiArea->subWindowList().size();i++)
		{
			if (ui.mdiArea->subWindowList()[i]->widget() == emsInfo)
			{
				ui.mdiArea->removeSubWindow(ui.mdiArea->subWindowList()[i]);
				break;
			}
		}
		emsInfo->hide();
		emsInfo->deleteLater();
		emsInfo = 0;
	}
	else
	{
		emsInfo = new EmsInfo();
		emsInfo->setFirmwareVersion(m_firmwareVersion);
		emsInfo->setInterfaceVersion(m_interfaceVersion);
		connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),emsInfo,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));
		connect(emsComms,SIGNAL(destroyed()),this,SLOT(dataTablesDestroyed()));
		QMdiSubWindow *win2 = ui.mdiArea->addSubWindow(emsInfo);
		win2->setGeometry(emsInfo->geometry());
		win2->show();
	}
}

void MainWindow::menu_windows_TablesClicked()
{
	if (dataTables)
	{
		//QMdiSubWindow *win;
		//win->widget()
		for (int i=0;i<ui.mdiArea->subWindowList().size();i++)
		{
			if (ui.mdiArea->subWindowList()[i]->widget() == dataTables)
			{
				ui.mdiArea->removeSubWindow(ui.mdiArea->subWindowList()[i]);
				break;
			}
		}
		dataTables->hide();
		dataTables->deleteLater();
		dataTables = 0;
	}
	else
	{
		dataTables = new DataTables();
		connect(dataTables,SIGNAL(destroyed()),this,SLOT(dataTablesDestroyed()));
		dataTables->passDecoder(dataPacketDecoder);
		QMdiSubWindow *win3 = ui.mdiArea->addSubWindow(dataTables);
		win3->setGeometry(dataTables->geometry());
		win3->show();
	}
}
void MainWindow::menu_windows_FlagsClicked()
{
	if (dataFlags)
	{
		//QMdiSubWindow *win;
		//win->widget()
		for (int i=0;i<ui.mdiArea->subWindowList().size();i++)
		{
			if (ui.mdiArea->subWindowList()[i]->widget() == dataTables)
			{
				ui.mdiArea->removeSubWindow(ui.mdiArea->subWindowList()[i]);
				break;
			}
		}
		dataFlags->hide();
		dataFlags->deleteLater();
		dataFlags = 0;
	}
	else
	{
		dataFlags = new DataFlags();
		connect(dataFlags,SIGNAL(destroyed()),this,SLOT(dataFlagsDestroyed()));
		dataFlags->passDecoder(dataPacketDecoder);
		QMdiSubWindow *win3 = ui.mdiArea->addSubWindow(dataFlags);
		win3->setGeometry(dataFlags->geometry());
		win3->show();
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
	int firmwareseq = emsComms->getFirmwareVersion();
	int ifaceseq = emsComms->getInterfaceVersion();
	int locidseq = emsComms->getLocationIdList(0x00,0x00);
	/*ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(firmwareseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getFirmwareVersion"));

	ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(ifaceseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getInterfaceVersion"));

	ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(locidseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getLocationIdList"));
	*/
}

void MainWindow::commandSuccessful(int sequencenumber)
{
	qDebug() << "command succesful:" << QString::number(sequencenumber);
	/*for (int i=0;i<ui.sendCommandTableWidget->rowCount();i++)
	{
		if (ui.sendCommandTableWidget->item(i,0)->text().toInt() == sequencenumber)
		{
			ui.sendCommandTableWidget->item(i,1)->setText("Success");
		}
	}*/
}
void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	qDebug() << "command failed:" << QString::number(sequencenumber) << QString::number(errornum,16);
	/*for (int i=0;i<ui.sendCommandTableWidget->rowCount();i++)
	{
		if (ui.sendCommandTableWidget->item(i,0)->text().toInt() == sequencenumber)
		{
			ui.sendCommandTableWidget->item(i,1)->setText("Failed");
			ui.sendCommandTableWidget->item(i,2)->setText(QString::number(errornum));
		}
	}*/
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
