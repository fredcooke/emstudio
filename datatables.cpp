#include "datatables.h"
#include <QMdiSubWindow>

DataTables::DataTables(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,50);
	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(250);

}
void DataTables::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void DataTables::passDecoder(DataPacketDecoder *decoder)
{
	dataPacketDecoder = decoder;
	ui.tableWidget->setRowCount(dataPacketDecoder->m_dataFieldList.size());
	for (int i=0;i<dataPacketDecoder->m_dataFieldList.size();i++)
	{
		m_nameToIndexMap[dataPacketDecoder->m_dataFieldList[i].name()] = i;
		ui.tableWidget->setItem(i,0,new QTableWidgetItem(dataPacketDecoder->m_dataFieldList[i].description()));
		ui.tableWidget->setItem(i,1,new QTableWidgetItem("0"));
	}
}

DataTables::~DataTables()
{

}
void DataTables::passData(QVariantMap data)
{
	m_valueMap = data;
}

void DataTables::guiUpdateTimerTick()
{
	QVariantMap::const_iterator i = m_valueMap.constBegin();
	while (i != m_valueMap.constEnd())
	{
		if (i.value().type() == QVariant::Double)
		{
			//widget->propertyMap.setProperty(i.key().toAscii(),QVariant::fromValue(i.value()));
			ui.tableWidget->item(m_nameToIndexMap[i.key()],1)->setText(i.value().toString());
		}
		//qDebug() << i.key() << m_nameToIndexMap[i.key()] << i.value();
		i++;
	}

}
