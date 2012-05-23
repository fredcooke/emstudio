#include "dataflags.h"
#include <QMdiSubWindow>
#include <QDebug>
DataFlags::DataFlags(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    /*m_dataFieldList.append(DataField("coreStatusA","Duplicate",60,1,1.0));
 m_dataFieldList.append(DataField("decoderFlags","Various decoder state flags",61,1,1.0));
 m_dataFieldList.append(DataField("flaggableFlags","Flags to go with flaggables",62,2,1.0));*/
	ui.flagsTableWidget->setColumnCount(2);
	ui.flagsTableWidget->setColumnWidth(0,300);
	ui.flagsTableWidget->setColumnWidth(1,50);
}

DataFlags::~DataFlags()
{
}
void DataFlags::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
void DataFlags::passData(QVariantMap data)
{
	QVariantMap::const_iterator i = data.constBegin();
	while (i != data.constEnd())
	{
		if (i.value().type() == QVariant::Bool)
		{
			qDebug() << "Flag value:" << i.key() << i.value();
			//Flag
			bool found = false;
			for (int j=0;j<ui.flagsTableWidget->rowCount();j++)
			{
				if (ui.flagsTableWidget->item(j,0)->text() == i.key())
				{
					found = true;
					ui.flagsTableWidget->item(j,1)->setText(i.value().toBool() ? "1" : "0");
				}
			}
			if (!found)
			{
				ui.flagsTableWidget->setRowCount(ui.flagsTableWidget->rowCount()+1);
				ui.flagsTableWidget->setItem(ui.flagsTableWidget->rowCount()-1,0,new QTableWidgetItem(i.key()));
				ui.flagsTableWidget->setItem(ui.flagsTableWidget->rowCount()-1,1,new QTableWidgetItem(i.value().toBool() ? "1" : "0"));
			}

		}
		else if (i.value().type() == QVariant::Double)
		{
			//Value
			qDebug() << "Double value:" << i.key() << i.value();
		}
		else
		{
			qDebug() << "unknown value type:" << i.value().typeName();
		}
		i++;
	}
}

void DataFlags::passDecoder(DataPacketDecoder *decoder)
{
}
