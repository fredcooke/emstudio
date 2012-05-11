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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFile>
#include "datafield.h"
//#include "logloader.h"
#include "freeemscomms.h"
#include "gaugewidget.h"
#include <QTimer>
#include "datapacketdecoder.h"
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	DataPacketDecoder *dataPacketDecoder;
	void populateDataFields();
	GaugeWidget *widget;
	Ui::MainWindow ui;
	//LogLoader *logLoader;
	FreeEmsComms *emsComms;
	int pidcount;
	QTimer *timer;
	QTimer *guiUpdateTimer;
	QMap<QString,double> m_valueMap;
	QMap<QString,int> m_nameToIndexMap;
private slots:
	void guiUpdateTimerTick();
	void timerTick();
	void connectButtonClicked();
	void dataLogDecoded(QMap<QString,double> data);
	void logPayloadReceived(QByteArray header,QByteArray payload);
	void logProgress(qlonglong current,qlonglong total);
	void logFinished();
	void loadLogButtonClicked();
	void playLogButtonClicked();
	void pauseLogButtonClicked();
	void stopLogButtonClicked();

	void locationIdList(QList<unsigned short> idlist);
	void locationIdInfo(QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size);
	void blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QByteArray version);
	void firmwareVersion(QString version);
	void error(QString msg);
	void commandSuccessful(int sequencenumber);
	void commandFailed(int sequencenumber,unsigned short errornum);

};

#endif // MAINWINDOW_H
