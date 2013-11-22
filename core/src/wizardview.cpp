#include "wizardview.h"
#include <QDebug>
WizardView::WizardView(QWidget *parent) : QDeclarativeView(parent)
{

}
void WizardView::addConfigRequest(QString name)
{
    m_configRequestList.append(name);
}

void WizardView::setFile(EmsComms *comms,QString filename)
{
	updateTimer = new QTimer(this);
	connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTick()));
	updateTimer->start(250);
	connect(comms->getDecoder(),SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(decoderPayloadDecoded(QVariantMap)));
	this->rootContext()->setContextProperty("decoder",this);
	this->rootContext()->setContextProperty("emscomms",comms);

	this->rootContext()->setContextProperty("propretyMap",&m_configPropertyMap);
	setSource(QUrl::fromLocalFile(filename));
	emscomms = comms;
}
void WizardView::passConfig(QMap<QString,QList<ConfigBlock> > config)
{
	m_configBlock = config;
	for(QMap<QString,QList<ConfigBlock> >::const_iterator i=config.constBegin();i!=config.constEnd();i++)
	{
		for (int j=0;j<i.value().size();j++)
		{
			ConfigBlock b = i.value()[j];
			m_configPropertyMap[b.name()] = 0;
		}
	}
}
void WizardView::addConfig(QString name,ConfigData *config)
{
    if (!m_configRequestList.contains(name))
    {
        //Throw away, we don't care
        return;
    }
    //We don't actually need to hold on to this object (*config), it is garunteed to live for the life
    //of the application. Simply connect the signals/slots, and ignore it otherwise.
    connect(config,SIGNAL(update()),this,SLOT(configUpdate()));
}
void WizardView::configUpdate()
{
    //Config block got updated.
    //ConfigData *data = qobject_cast<ConfigData*>(sender());
    //if (!data)
    //{
        //Someone else fired off this signal!
    //    return;
    //}
    //this->rootContext()->setContextProperty(data->name(),data->value());

}

void WizardView::configRecieved(ConfigBlock block,QVariant value)
{
	m_configPropertyMap[block.name()] = value.toString();
	qDebug() << "Wizard Config recieved" << block.name() << value.toString();
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
void WizardView::showEvent(QShowEvent *event)
{
	Q_UNUSED(event)
	emit visibilityChanged(true);
}

void WizardView::hideEvent(QHideEvent *event)
{
	Q_UNUSED(event)
	emit visibilityChanged(false);
}
