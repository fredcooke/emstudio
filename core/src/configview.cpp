#include "configview.h"
#include <QDebug>
#include <QTreeWidget>
ConfigView::ConfigView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	//itemChanged ( QTreeWidgetItem * item, int column )
	ui.treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	//ui.treeWidget->setColumnCount(2);
	connect(ui.treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(treeItemDoubleClicked(QTreeWidgetItem*,int)));
	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(treeCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}
void ConfigView::treeCurrentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *previous)
{
	Q_UNUSED(current)
	ui.treeWidget->closePersistentEditor(previous,0);
}

void ConfigView::treeItemDoubleClicked(QTreeWidgetItem* item,int column)
{
	if (item->parent() == 0)
	{
		//Item is a top level item, uneditable.
		qDebug() << "Top level item";
		return;
	}
	ui.treeWidget->openPersistentEditor(item,column);
}

void ConfigView::treeItemChanged(QTreeWidgetItem* item,int column)
{
	//Q_UNUSED(column)
	ui.treeWidget->closePersistentEditor(item,column);
	qDebug() << "Item changed:" << item->text(0);
	for (int i=0;i<ui.treeWidget->topLevelItemCount();i++)
	{
		if (ui.treeWidget->topLevelItem(i) == item)
		{
			int index = ui.treeWidget->topLevelItem(i)->indexOfChild(item);
			int totaloffset = m_configList[i].offset() + index * m_configList[i].elementSize();
			unsigned short newval = backConvertAxis(item->text(0).toDouble(),m_configList[i].calc());
			QByteArray retval;
			retval.append((char)((newval >> 8) & 0xFF));
			retval.append(newval & 0xFF);
			m_rawData.replace(totaloffset,m_configList[i].elementSize(),retval);
			emit saveData(m_configList[i].locationId(),m_rawData);
		}
	}
}
void ConfigView::passConfig(QList<ConfigBlock> config,QByteArray data)
{
	m_configList = config;
	m_rawData = data;
	for (int i=0;i<config.size();i++)
	{
		//ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
		//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem(config[i].name()));
		ui.treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << config[i].name()));

		if (config[i].type() == "value")
		{
			unsigned short val = 0;
			for (int j=0;j<config[i].size();j++)
			{
				val += ((unsigned char)data[config[i].offset() + j]) << (8 * (config[i].size() - (j+1)));
				qDebug() << j << (8 * (config[i].size() - (j+1))) << QString::number(data[config[i].offset() + j]);
			}
			ui.treeWidget->topLevelItem(ui.treeWidget->topLevelItemCount()-1)->addChild(new QTreeWidgetItem(QStringList() << QString::number(calcAxis(val,config[i].calc()))));
			//int count = ui.treeWidget->topLevelItemCount()-1;
			//ui.treeWidget->topLevelItem(count)->child(ui.treeWidget->topLevelItem(count)->childCount()-1)->setFlags(ui.treeWidget->topLevelItem(count)->child(ui.treeWidget->topLevelItem(count)->childCount()-1)->flags() & Qt::ItemIsEditable);
			//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,1,new QTableWidgetItem(QString::number(calcAxis(val,config[i].calc()))));

		}
		else if (config[i].type() == "array")
		{
			//QString text = "";
			for (int j=0;j<config[i].size();j++)
			{
				unsigned short val = 0;
				for (int k=0;k<config[i].elementSize();k++)
				{
					val += ((unsigned char)data[config[i].offset() + j + k]) << (8 * (config[i].elementSize() - (k+1)));
					//val += data[config[i].offset() + j+1] << 8;
				}
				ui.treeWidget->topLevelItem(ui.treeWidget->topLevelItemCount()-1)->addChild(new QTreeWidgetItem(QStringList() << QString::number(calcAxis(val,config[i].calc()))));
				//text += "{" + QString::number(calcAxis(val,config[i].calc())) + "} ";
				j+=config[i].elementSize()-1;
			}
			//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,1,new QTableWidgetItem(text));
			//ui.treeWidget->topLevelItem(ui.treeWidget->topLevelItemCount()-1)->addChild(new QTreeWidgetItem(QStringList() << text));
		}
	}
}
double ConfigView::calcAxis(unsigned short val,QList<QPair<QString,double> > metadata)
{
	if (metadata.size() == 0)
	{
		return val;
	}
	double newval = val;
	for (int j=0;j<metadata.size();j++)
	{
		if (metadata[j].first == "add")
		{
			newval += metadata[j].second;
		}
		else if (metadata[j].first == "sub")
		{
			newval -= metadata[j].second;
		}
		else if (metadata[j].first == "mult")
		{
			newval *= metadata[j].second;
		}
		else if (metadata[j].first == "div")
		{
			newval /= metadata[j].second;
		}
	}
	return newval;
}
unsigned short ConfigView::backConvertAxis(double val,QList<QPair<QString,double> > metadata)
{
	if (metadata.size() == 0)
	{
		return val;
	}
	double newval = val;
	for (int j=metadata.size()-1;j>=0;j--)
	{
		if (metadata[j].first == "add")
		{
			newval -= metadata[j].second;
		}
		else if (metadata[j].first == "sub")
		{
			newval += metadata[j].second;
		}
		else if (metadata[j].first == "mult")
		{
			newval /= metadata[j].second;
		}
		else if (metadata[j].first == "div")
		{
			newval *= metadata[j].second;
		}
	}
	return (unsigned short)newval;
}

ConfigView::~ConfigView()
{

}
