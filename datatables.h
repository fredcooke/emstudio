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
	void passData(QVariantMap data);
	void passDecoder(DataPacketDecoder *decoder);
private:
	QTimer *guiUpdateTimer;
	DataPacketDecoder *dataPacketDecoder;
	Ui::DataTables ui;
	QVariantMap m_valueMap;
	QMap<QString,int> m_nameToIndexMap;
private slots:
	void guiUpdateTimerTick();
};

#endif // DATATABLES_H
