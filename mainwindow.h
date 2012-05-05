#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFile>
#include "datafield.h"
#include "logloader.h"
#include "freeemscomms.h"
#include "gaugewidget.h"
#include <QTimer>
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	GaugeWidget *widget;
	QList<DataField> m_dataFieldList;
	Ui::MainWindow ui;
	LogLoader *logLoader;
	FreeEmsComms *emsComms;
	int pidcount;
	QTimer *timer;
	QTimer *guiUpdateTimer;
	QMap<QString,double> m_valueMap;
private slots:
	void guiUpdateTimerTick();
	void timerTick();
	void connectButtonClicked();
	void logPayloadReceived(QByteArray header,QByteArray payload);
	void logProgress(qlonglong current,qlonglong total);
	void logFinished();
	void loadLogButtonClicked();
	void playLogButtonClicked();
	void pauseLogButtonClicked();
	void stopLogButtonClicked();
};

#endif // MAINWINDOW_H
