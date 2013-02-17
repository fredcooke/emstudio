#include "emsstatus.h"
#include <QVariant>
#include <QDebug>
EmsStatus::EmsStatus(QWidget *parent) : QDockWidget(parent)
{
	ui.setupUi(this);
	connect(ui.hardResetPushButton,SIGNAL(clicked()),this,SIGNAL(hardResetRequest()));
	connect(ui.softResetPushButton,SIGNAL(clicked()),this,SIGNAL(softResetRequest()));
	setFlag(ui.combustionSyncLineEdit,false);
	setFlag(ui.crankSyncLineEdit,false);
	setFlag(ui.camSyncLineEdit,false);
	setFlag(ui.lastPeriodValidLineEdit,false);
	setFlag(ui.lastTimeValidLineEdit,false);
}
void EmsStatus::closeEvent(QCloseEvent *event)
{
	event->ignore();
	this->hide();
}
EmsStatus::~EmsStatus()
{
}
void EmsStatus::setFlag(QLineEdit *edit,bool green)
{
	if (green)
	{
		if (edit->palette().color(QPalette::Base).red() == 255)
		{
			QPalette pal = edit->palette();
			pal.setColor(QPalette::Base,QColor::fromRgb(100,255,100));
			edit->setPalette(pal);
		}
	}
	else
	{
		if (edit->palette().color(QPalette::Base).green() == 255)
		{
			QPalette pal = edit->palette();
			pal.setColor(QPalette::Base,QColor::fromRgb(255,50,50));
			edit->setPalette(pal);
		}
	}
}

void EmsStatus::passData(QVariantMap data)
{

	unsigned char decoderFlags = data["decoderFlags"].toInt();
	//qDebug() << "Sync:" << decoderFlags;
	setFlag(ui.combustionSyncLineEdit,decoderFlags & 0x01);
	setFlag(ui.crankSyncLineEdit,decoderFlags & 0x02);
	setFlag(ui.camSyncLineEdit,decoderFlags & 0x04);
	setFlag(ui.lastPeriodValidLineEdit,decoderFlags & 0x08);
	setFlag(ui.lastTimeValidLineEdit,decoderFlags & 0x010);

}
