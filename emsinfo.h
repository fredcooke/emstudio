#ifndef EMSINFO_H
#define EMSINFO_H

#include <QWidget>
#include <QCloseEvent>
#include "ui_emsinfo.h"
#include "freeemscomms.h"
class EmsInfo : public QWidget
{
    Q_OBJECT

public:
	explicit EmsInfo(QWidget *parent = 0);
	~EmsInfo();
	void setInterfaceVersion(QString version);
	void setFirmwareVersion(QString firmware);
protected:
	void closeEvent(QCloseEvent *event);
public slots:
	void locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size);
private:
	Ui::EmsInfo ui;
};

#endif // EMSINFO_H
