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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "tableview3d.h"
#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFile>
//#include "datafield.h"
#include "configview.h"
//#include "emsdata.h"
#include "memorymetadata.h"
#include "emscomms.h"
#include "configblock.h"
#include <QTimer>
#include <qjson/serializer.h>
#include "datapacketdecoder.h"
#include "comsettings.h"
#include "emsinfoview.h"
#include "tableview.h"
#include "rawdataview.h"
#include "gaugeview.h"
#include "flagview.h"
#include "packetstatusview.h"
#include "aboutview.h"
//#include "memorylocation.h"
#include "interrogateprogressview.h"
#include "table2ddata.h"
#include "readonlyramview.h"
#include "emsstatus.h"
#include "tablemap3d.h"
#include "rawdata.h"
#include "serialportstatus.h"
#include <QPluginLoader>
#include "parameterview.h"
class RawDataBlock
{
public:
	unsigned short locationid;
	QByteArray header;
	QByteArray data;
};

class Interrogation
{
public:
	QString firmwareVersion;
	QString interfaceVersion;
	QString compilerVersion;
	QString firmwareBuildDate;
	QString decoderName;
	QString maxPacketSize;
	QString operatingSystem;
	QString emstudioBuilDate;
	QString emstudioCommit;
	QString emstudioHash;
};


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setDevice(QString dev);
	void setPlugin(QString plugin);
	void connectToEms();
private:
	QList<unsigned short> m_locationIdList;
	bool m_checkEmsDataInUse;
	bool m_offlineMode;
	QMap<unsigned short,MemoryLocationInfo> m_memoryInfoMap;
	Interrogation emsinfo;
	unsigned short m_currentRamLocationId;
	bool m_waitingForRamWriteConfirmation;
	unsigned short m_currentFlashLocationId;
	bool m_waitingForFlashWriteConfirmation;
//	QList<MemoryLocation*> m_tempMemoryList;
	QMap<unsigned short,QList<ConfigBlock> > m_configBlockMap;
	QMap<unsigned short,QMdiSubWindow*> m_table3DMapViewMap;
	QMap<unsigned short,TableMap3D*> m_table3DMapViewWidgetMap;
//	QList<ConfigData> m_configMetaData;
	MemoryMetaData *m_memoryMetaData;
	TableView *dataTables;
	GaugeView *dataGauges;
	EmsInfoView *emsInfo;
	FlagView *dataFlags;
	QString m_logFileName;
	PacketStatusView *packetStatus;
	AboutView *aboutView;
	EmsStatus *statusView;
	InterrogateProgressView *progressView;
	QList<int> interrogationSequenceList;
	QMap<unsigned short,QWidget*> m_rawDataView;
	QMap<unsigned short,ConfigView*> m_configDataView;
	QMdiSubWindow *tablesMdiWindow;
	QMdiSubWindow *interrogateProgressMdiWindow;
	QMdiSubWindow *emsMdiWindow;
	QMdiSubWindow *flagsMdiWindow;
	QMdiSubWindow *gaugesMdiWindow;
	QMdiSubWindow *packetStatusMdiWindow;
	QMdiSubWindow *aboutMdiWindow;
	QMdiSubWindow *emsStatusMdiWindow;

	ParameterView *parameterView;
	QMdiSubWindow *parameterMdiWindow;

	//QFile *settingsFile;
	void checkMessageCounters(int sequencenumber);
	DataPacketDecoder *dataPacketDecoder;
	void populateDataFields();

	Ui::MainWindow ui;
	QString m_pluginFileName;
	QPluginLoader *pluginLoader;
	EmsComms *emsComms;
	int pidcount;
	QTimer *timer;
	QTimer *guiUpdateTimer;
	QString m_comPort;
	int m_comBaud;
	int m_comInterByte;
	bool m_saveLogs;
	bool m_clearLogs;
	int m_logsToKeep;
	QString m_settingsFile;
	QString m_settingsDir;
	QString m_defaultsDir;
	QString m_localHomeDir;
	QString m_logDirectory;
	QString m_firmwareVersion;
	QString m_interfaceVersion;
	QFile *logfile;
	void markRamDirty();
	void markDeviceFlashDirty();
	void markRamClean();
	void markDeviceFlashClean();
	bool m_localRamDirty;
	bool m_debugLogs;
	bool m_deviceFlashDirty;
	void createView(unsigned short locid,DataType type);
	QList<int> m_locIdMsgList;
	QList<int> m_locIdInfoMsgList;
	void checkRamFlashSync();
	bool m_interrogationInProgress;
	//EmsData *emsData;
	//EmsData *checkEmsData;
	int m_currentEcuClock;
private slots:
	void interrogationData(QMap<QString,QString> datamap);
	void emsCommsSilence();
	void emsCommsSilenceBroken();
	void locationIdInfo(unsigned short locationid,MemoryLocationInfo info);
	void tableview3d_show3DTable(unsigned short locationid,Table3DData *data);
	void emsStatusHardResetRequested();
	void emsStatusSoftResetRequested();
	void tableMap3DDestroyed(QObject *object);
	void interrogateProgressViewDestroyed(QObject *object);
	void emsOperatingSystem(QString os);
	void emsDecoderName(QString name);
	void emsFirmwareBuildDate(QString date);
	void emsCommsDisconnected();
	void emsCompilerVersion(QString version);
	void checkSyncRequest();
	void rawDataViewDestroyed(QObject *object);
	void emsInfoDisplayLocationId(int locid,bool isram,DataType type);
	void menu_file_saveOfflineDataClicked();
	void menu_file_loadOfflineDataClicked();
	void menu_windows_GaugesClicked();
	void menu_windows_EmsStatusClicked();
	void menu_windows_EmsInfoClicked();
	void menu_windows_TablesClicked();
	void menu_windows_FlagsClicked();
	void menu_windows_PacketStatusClicked();
	void menu_windows_interrogateProgressViewClicked();
	void menu_windows_ParameterViewClicked();
	void menu_settingsClicked();
	void menu_connectClicked();
	void menu_aboutClicked();
	void menu_enableDatalogsClicked();
	void menu_disableDatalogsClicked();
	void interrogateProgressViewCancelClicked();
	void ui_saveDataButtonClicked();
	void menu_disconnectClicked();
	void settingsSaveClicked();
	void settingsCancelClicked();
	void guiUpdateTimerTick();
	void timerTick();
	void connectButtonClicked();
	void dataLogDecoded(QVariantMap data);
	void logPayloadReceived(QByteArray header,QByteArray payload);
	void logProgress(qlonglong current,qlonglong total);
	void logFinished();
	void loadLogButtonClicked();
	void playLogButtonClicked();
	void pauseLogButtonClicked();
	void stopLogButtonClicked();
	void emsCommsConnected();
	void unknownPacket(QByteArray header,QByteArray payload);
	void blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QString version);
	void firmwareVersion(QString version);
	void error(QString msg);
	void error(SerialPortStatus error,QString msg);
	void commandSuccessful(int sequencenumber);
	void commandFailed(int sequencenumber,unsigned short errornum);
	void commandTimedOut(int sequencenumber);
	void interByteDelayChanged(int num);
	void retrieveRamLocationId(unsigned short locationid);
	void retrieveFlashLocationId(unsigned short locationid);
	void interrogationProgress(int current, int total);
	void interrogationComplete();
	void interrogateTaskStart(QString task, int sequence);
	void interrogateTaskSucceed(int sequence);
	void interrogateTaskFail(int sequence);
	void locationIdList(QList<unsigned short> idlist);

};

#endif // MAINWINDOW_H
