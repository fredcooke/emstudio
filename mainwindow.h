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
};

#endif // MAINWINDOW_H
