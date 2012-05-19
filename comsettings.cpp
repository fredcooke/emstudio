#include "comsettings.h"


ComSettings::ComSettings(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.cancelPushButton,SIGNAL(clicked()),this,SIGNAL(cancelClicked()));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SIGNAL(saveClicked()));
}

ComSettings::~ComSettings()
{
}
void ComSettings::setInterByteDelay(int delay)
{
	ui.interByteDelaySpinBox->setValue(delay);
}
void ComSettings::setBaud(int baud)
{
	ui.baudRateLineEdit->setText(QString::number(baud));
}
void ComSettings::setComPort(QString port)
{
	ui.portNameLineEdit->setText(port);
}
int ComSettings::getInterByteDelay()
{
	return ui.interByteDelaySpinBox->value();
}
int ComSettings::getBaud()
{
	return ui.baudRateLineEdit->text().toInt();
}
QString ComSettings::getComPort()
{
	return ui.portNameLineEdit->text();
}
