#ifndef DATATABLES_H
#define DATATABLES_H

#include <QWidget>
#include <QTimer>
#include "ui_datatables.h"
#include "datapacketdecoder.h"
class DataTables : public QWidget
{
    Q_OBJECT

public:
	explicit DataTables(QWidget *parent = 0);
	~DataTables();
	void passData(QMap<QString,double> data);
	void passDecoder(DataPacketDecoder *decoder);
private:
	QTimer *guiUpdateTimer;
	DataPacketDecoder *dataPacketDecoder;
	Ui::DataTables ui;
	QMap<QString,double> m_valueMap;
	QMap<QString,int> m_nameToIndexMap;
private slots:
	void guiUpdateTimerTick();
};

#endif // DATATABLES_H
