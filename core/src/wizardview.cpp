#include "wizardview.h"
#include <QDebug>
WizardView::WizardView(QWidget *parent) : QDeclarativeView(parent)
{

}
void WizardView::setFile(EmsComms *comms,QString filename)
{
	updateTimer = new QTimer(this);
	connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTick()));
	updateTimer->start(250);
	connect(comms->getDecoder(),SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(decoderPayloadDecoded(QVariantMap)));
	this->rootContext()->setContextProperty("decoder",this);
	this->rootContext()->setContextProperty("emscomms",comms);
	setSource(QUrl::fromLocalFile(filename));
	emscomms = comms;
}

void WizardView::updateTimerTick()
{
	emit payloadDecoded(m_savedPayload);
}
void WizardView::decoderPayloadDecoded(QVariantMap values)
{
	m_savedPayload = values;
}
void WizardView::setMemory(unsigned short locationid,unsigned short offset,unsigned short length,QVariantList array)
{
	QByteArray data;
	for (int i=0;i<length;i++)
	{
		data.append(array[i].toUInt());
	}
	emscomms->updateBlockInFlash(locationid,offset,length,data);
}
