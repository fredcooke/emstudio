#include "datagauges.h"
#include <QMdiSubWindow>

DataGauges::DataGauges(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	widget = new GaugeWidget(this);
	widget->setGeometry(0,0,1200,600);
	widget->show();

	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(250);
}

DataGauges::~DataGauges()
{
}
void DataGauges::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void DataGauges::passData(QVariantMap data)
{
	m_valueMap = data;
}

void DataGauges::passDecoder(DataPacketDecoder *decoder)
{

}

void DataGauges::guiUpdateTimerTick()
{

	QVariantMap::const_iterator i = m_valueMap.constBegin();
	while (i != m_valueMap.constEnd())
	{
		widget->propertyMap.setProperty(i.key().toAscii(),i.value());
		//ui.tableWidget->item(m_nameToIndexMap[i.key()],1)->setText(QString::number(i.value()));
		//qDebug() << i.key() << m_nameToIndexMap[i.key()] << i.value();
		i++;
	}

}
