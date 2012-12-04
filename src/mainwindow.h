/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "tableview3d.h"
#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFile>
#include "datafield.h"
#include "configview.h"
#include "emsdata.h"
#include "memorymetadata.h"
//#include "logloader.h"
#include "freeemscomms.h"
#include "configblock.h"
#include <QTimer>
#include <qjson/serializer.h>
#include "headers.h"
#include "datapacketdecoder.h"
#include "comsettings.h"
#include "emsinfoview.h"
#include "tableview.h"
#include "rawdataview.h"
#include "gaugeview.h"
#include "flagview.h"
#include "packetstatusview.h"
#include "aboutview.h"
#include "memorylocation.h"
#include "interrogateprogressview.h"
#include "table2ddata.h"
#include "readonlyramview.h"
#include "emsstatus.h"
#include "tablemap3d.h"
//#include "datarawview.h"


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
	void connectToEms();
private:
	bool m_offlineMode;
	QMap<unsigned short,MemoryLocationInfo> m_memoryInfoMap;
	Interrogation emsinfo;
	unsigned short m_currentRamLocationId;
	bool m_waitingForRamWriteConfirmation;
	unsigned short m_currentFlashLocationId;
	bool m_waitingForFlashWriteConfirmation;
	//QList<RawDataBlock*> m_ramRawBlockList;
	//QList<RawDataBlock*> m_flashRawBlockList;
	//QList<RawDataBlock*> m_deviceRamRawBlockList;
	//QList<RawDataBlock*> m_deviceFlashRawBlockList;
	/*QList<MemoryLocation*> m_ramMemoryList;
	QList<MemoryLocation*> m_flashMemoryList;
	QList<MemoryLocation*> m_deviceFlashMemoryList;
	QList<MemoryLocation*> m_deviceRamMemoryList;*/
	QList<MemoryLocation*> m_tempMemoryList;
	QMap<unsigned short,QList<ConfigBlock> > m_configBlockMap;
	QMap<unsigned short,QMdiSubWindow*> m_table3DMapViewMap;
	QList<ConfigData> m_configMetaData;
	//QMap<unsigned short,QList<ReadOnlyRamData> > m_readOnlyMetaDataMap;
	//RawDataView *rawData;
	MemoryMetaData m_memoryMetaData;
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
	QMdiSubWindow *tablesMdiWindow;
	QMdiSubWindow *interrogateProgressMdiWindow;
	QMdiSubWindow *emsMdiWindow;
	QMdiSubWindow *flagsMdiWindow;
	QMdiSubWindow *gaugesMdiWindow;
	QMdiSubWindow *packetStatusMdiWindow;
	QMdiSubWindow *aboutMdiWindow;
	QMdiSubWindow *emsStatusMdiWindow;
	//QFile *settingsFile;
	void checkMessageCounters(int sequencenumber);
	//QMdiSubWindow *rawMdiWindow;
	//ComSettings *comSettings;
	void populateParentLists();
	DataPacketDecoder *dataPacketDecoder;
	void populateDataFields();
	void updateRamLocation(unsigned short locationid);
	void updateDataWindows(unsigned short locationid);
	Ui::MainWindow ui;
	//LogLoader *logLoader;
	FreeEmsComms *emsComms;
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
	QString m_logDirectory;
	QString m_firmwareVersion;
	QString m_interfaceVersion;
	QFile *logfile;
	void markRamDirty();
	void markDeviceFlashDirty();
	void markRamClean();
	void markDeviceFlashClean();
	bool m_localRamDirty;
	bool m_deviceFlashDirty;
	void updateView(unsigned short locid,QObject *view,QByteArray data,DataType type);
	void createView(unsigned short locid,QByteArray data,DataType type,bool isram, bool isflash);
	QList<int> m_locIdMsgList;
	QList<int> m_locIdInfoMsgList;
	void checkRamFlashSync();
	bool m_interrogationInProgress;
	EmsData *emsData;
	EmsData *checkEmsData;
private slots:
	void locationIdInfo(unsigned short locationid,MemoryLocationInfo info);
	void tableview3d_show3DTable(unsigned short locationid,Table3DData *data);
	void emsStatusHardResetRequested();
	void emsStatusSoftResetRequested();
	void tableMap3DDestroyed(QObject *object);
	void interrogateProgressViewDestroyed(QObject *object);
	void interrogateRamBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload);
	void interrogateFlashBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload);
	bool verifyMemoryBlock(unsigned short locationid,QByteArray header,QByteArray payload);
	void emsOperatingSystem(QString os);
	void emsDecoderName(QString name);
	void emsFirmwareBuildDate(QString date);
	void emsCommsDisconnected();
	void emsCompilerVersion(QString version);
	void checkSyncRequest();
	void rawViewSaveData(unsigned short locationid,QByteArray data,int physicallocation);
	void rawDataViewDestroyed(QObject *object);
	void emsInfoDisplayLocationId(int locid,bool isram,DataType type);
	void dataViewSaveLocation(unsigned short locationid,QByteArray data,int phyiscallocation);
	void menu_file_saveOfflineDataClicked();
	void menu_file_loadOfflineDataClicked();
	void menu_windows_GaugesClicked();
	void menu_windows_EmsStatusClicked();
	void menu_windows_EmsInfoClicked();
	void menu_windows_TablesClicked();
	void menu_windows_FlagsClicked();
	void menu_windows_PacketStatusClicked();
	void menu_windows_interrogateProgressViewClicked();
	void menu_settingsClicked();
	void menu_connectClicked();
	void menu_aboutClicked();
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
	void tableview3d_reloadTableData(unsigned short locationid,bool ram);
	void tableview2d_reloadTableData(unsigned short locationid,bool isram);
	void reloadDataFromDevice(unsigned short locationid,bool isram);
	void loadLogButtonClicked();
	void playLogButtonClicked();
	void pauseLogButtonClicked();
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	void saveFlashLocationId(unsigned short locationid);
	void stopLogButtonClicked();
	void emsCommsConnected();
	void unknownPacket(QByteArray header,QByteArray payload);
	void locationIdList(QList<unsigned short> idlist);
	//void locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size);
	void blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload);
	void ramBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload);
	void flashBlockRetrieved(unsigned short locationid,QByteArray header,QByteArray payload);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QString version);
	void firmwareVersion(QString version);
	void error(QString msg);
	void error(SerialError error,QString msg);
	void commandSuccessful(int sequencenumber);
	void commandFailed(int sequencenumber,unsigned short errornum);
	void commandTimedOut(int sequencenumber);
	void interByteDelayChanged(int num);
	void saveFlashLocationIdBlock(unsigned short locationid,QByteArray data);
	void retrieveRamLocationId(unsigned short locationid);
	void retrieveFlashLocationId(unsigned short locationid);

};

#endif // MAINWINDOW_H
