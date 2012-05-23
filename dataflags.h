#ifndef DATAFLAGS_H
#define DATAFLAGS_H

#include <QWidget>
#include "datapacketdecoder.h"
#include "ui_dataflags.h"
class DataFlags : public QWidget
{
	Q_OBJECT

public:
	explicit DataFlags(QWidget *parent = 0);
	~DataFlags();
	void passData(QVariantMap data);
	void passDecoder(DataPacketDecoder *decoder);

private:
	Ui::DataFlags ui;
};

#endif // DATAFLAGS_H
