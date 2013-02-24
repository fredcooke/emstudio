#include "memorymetadata.h"
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <qjson/parser.h>
#include <QMessageBox>
#include <QVariant>
MemoryMetaData::MemoryMetaData()
{
}
bool MemoryMetaData::parseMetaData(QString json)
{
	QJson::Parser parser;
	QVariant top = parser.parse(json.toStdString().c_str());
	if (!top.isValid())
	{
		QString errormsg = QString("Error parsing JSON from config file on line number: ") + QString::number(parser.errorLine()) + " error text: " + parser.errorString();
		//QMessageBox::information(0,"Error",errormsg);
		qDebug() << "Error parsing JSON";
		qDebug() << "Line number:" << parser.errorLine() << "error text:" << parser.errorString();
		qDebug() << "Start Json";
		qDebug() << "Json:" << json;
		qDebug() << "End Json";
		return false;
	}
	QVariantMap topmap = top.toMap();
	QVariantMap errormap = topmap["errormap"].toMap();
	QVariantMap::iterator i = errormap.begin();
	while (i != errormap.end())
	{
		bool ok = false;
		m_errorMap[i.value().toString().mid(2).toInt(&ok,16)] = i.key();
		i++;
	}

	QVariantMap ramvars = topmap["ramvars"].toMap();
	i = ramvars.begin();
	while (i != ramvars.end())
	{
		bool ok = false;
		unsigned short locid = i.key().mid(2).toInt(&ok,16);
		m_readOnlyMetaDataMap[locid] = ReadOnlyRamBlock();
		QVariantMap locidlist = i.value().toMap();
		QString title = locidlist["title"].toString();
		m_readOnlyMetaDataMap[locid].title = title;
		QVariantList locidmap = locidlist["vars"].toList();
		int offset = 0;
		for (int j=0;j<locidmap.size();j++)
		{
			QVariantMap newlocidmap = locidmap[j].toMap();
			ReadOnlyRamData rdata;
			rdata.dataTitle = newlocidmap["name"].toString();
			rdata.dataDescription = newlocidmap["title"].toString();
			rdata.locationId = locid;
			rdata.offset = offset;
			rdata.size = newlocidmap["size"].toInt();
			offset += rdata.size;
			m_readOnlyMetaDataMap[locid].m_ramData.append(rdata);
			m_readOnlyMetaData.append(rdata);
			//m_readOnlyMetaDataMap[locid].append(rdata);

		}
		/*QVariantMap::iterator j = locidmap.begin();
		while (j != locidmap.end())
		{
			if (j.key() == "title")
			{
				QString title = j.value().toString();
				qDebug() << "Location title:" << title;
			}
			else
			{
				qDebug() << j.key();
				QVariantMap valuemap = j.value().toMap();
				if (valuemap.contains("type"))
				{
					ConfigData cdata;
					cdata.configDescription = valuemap["title"].toString();
					cdata.configTitle = j.key();
					cdata.elementSize = valuemap["size"].toInt();
					cdata.locationId = locid;
					cdata.offset = valuemap["offset"].toInt();
					cdata.type = valuemap["type"].toString();
					QVariantMap calcmap = valuemap["calc"].toMap();
					QList<QPair<QString,double> > calclist;
					QVariantMap::iterator k = calcmap.begin();
					while (k != calcmap.end())
					{
						calclist.append(QPair<QString,double>(k.key(),k.value().toDouble()));
						k++;
					}
					cdata.elementCalc = calclist;
					if (valuemap["type"] == "value")
					{

					}
					else if (valuemap["type"] == "array")
					{
						cdata.arraySize = valuemap["arraysize"].toInt();
					}
					m_configMetaData.append(cdata);
				}

			}
			j++;
		}*/
		i++;
	}
	qDebug() << m_readOnlyMetaData.size() << "Ram entries found";
	QVariantMap tables = topmap["tables"].toMap();
	i = tables.begin();
	while (i != tables.end())
	{
		QVariantMap tabledata = i.value().toMap();
		if (tabledata["type"] == "3D")
		{
			Table3DMetaData meta;
			QString id = tabledata["locationid"].toString();
			QString xtitle = tabledata["xtitle"].toString();
			QVariantList xcalc = tabledata["xcalc"].toList();
			QString xdp = tabledata["xdp"].toString();
			unsigned int size = tabledata["size"].toInt();

			QString ytitle = tabledata["ytitle"].toString();
			QVariantList ycalc = tabledata["ycalc"].toList();
			QString ydp = tabledata["ydp"].toString();

			QString ztitle = tabledata["ztitle"].toString();
			QVariantList zcalc = tabledata["zcalc"].toList();
			QString zdp = tabledata["zdp"].toString();

			QString xhighlight = tabledata["xhighlight"].toString();
			QString yhighlight = tabledata["yhighlight"].toString();

			//QVariantMap::iterator calci = xcalc.begin();
			QList<QPair<QString,double> > xcalclist;
			QList<QPair<QString,double> > ycalclist;
			QList<QPair<QString,double> > zcalclist;
			for (int j=0;j<xcalc.size();j++)
			{
				qDebug() << "XCalc:" << xcalc[j].toMap()["type"].toString() << xcalc[j].toMap()["value"].toDouble();
				xcalclist.append(QPair<QString,double>(xcalc[j].toMap()["type"].toString(),xcalc[j].toMap()["value"].toDouble()));
			}
			for (int j=0;j<ycalc.size();j++)
			{
				ycalclist.append(QPair<QString,double>(ycalc[j].toMap()["type"].toString(),ycalc[j].toMap()["value"].toDouble()));
			}
			for (int j=0;j<zcalc.size();j++)
			{
				zcalclist.append(QPair<QString,double>(zcalc[j].toMap()["type"].toString(),zcalc[j].toMap()["value"].toDouble()));
			}

			bool ok = false;
			meta.locationId = id.mid(2).toInt(&ok,16);
			meta.tableTitle = i.key();
			meta.xAxisCalc = xcalclist;
			meta.xAxisTitle = xtitle;
			meta.xDp = xdp.toInt();
			meta.yAxisCalc = ycalclist;
			meta.yAxisTitle = ytitle;
			meta.yDp = ydp.toInt();
			meta.zAxisCalc = zcalclist;
			meta.zAxisTitle = ztitle;
			meta.zDp = zdp.toInt();
			meta.size = size;
			meta.valid = true;
			meta.xHighlight = xhighlight;
			meta.yHighlight = yhighlight;
			m_table3DMetaData.append(meta);
		}
		else if (tabledata["type"] == "2D")
		{
			Table2DMetaData meta;
			QString id = tabledata["locationid"].toString();
			QString xtitle = tabledata["xtitle"].toString();
			QVariantList xcalc = tabledata["xcalc"].toList();
			QString xdp = tabledata["xdp"].toString();
			QString ytitle = tabledata["ytitle"].toString();
			QVariantList ycalc = tabledata["ycalc"].toList();
			QString ydp = tabledata["ydp"].toString();
			unsigned int size = tabledata["size"].toInt();
			QString xhighlight = tabledata["xhighlight"].toString();

			QList<QPair<QString,double> > xcalclist;
			QList<QPair<QString,double> > ycalclist;

			for (int j=0;j<xcalc.size();j++)
			{
				qDebug() << "XCalc:" << xcalc[j].toMap()["type"].toString() << xcalc[j].toMap()["value"].toDouble();
				xcalclist.append(QPair<QString,double>(xcalc[j].toMap()["type"].toString(),xcalc[j].toMap()["value"].toDouble()));
			}
			for (int j=0;j<ycalc.size();j++)
			{
				ycalclist.append(QPair<QString,double>(ycalc[j].toMap()["type"].toString(),ycalc[j].toMap()["value"].toDouble()));
			}
			bool ok = false;
			meta.locationId = id.mid(2).toInt(&ok,16);
			meta.tableTitle = i.key();
			meta.xAxisCalc = xcalclist;
			meta.xAxisTitle = xtitle;
			meta.xDp = xdp.toInt();
			meta.yAxisCalc = ycalclist;
			meta.yAxisTitle = ytitle;
			meta.yDp = ydp.toInt();
			meta.size = size;
			meta.valid = true;
			meta.xHighlight = xhighlight;
			m_table2DMetaData.append(meta);
		}
		i++;
	}
	return true;
}

bool MemoryMetaData::loadMetaDataFromFile(QString filestr)
{
	qDebug() << "Loading config file from:" << filestr;
	QFile file(filestr);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(0,"Error","Error opening config file: " + file.errorString());
		return false;
		//Can't open the file.
	}
	QByteArray filebytes = file.readAll();
	qDebug() << "Loaded:" << filebytes.size() << "chars from config file";
	file.close();
	return parseMetaData(filebytes);
}

const Table3DMetaData MemoryMetaData::get3DMetaData(unsigned short locationid)
{
	for (int i=0;i<m_table3DMetaData.size();i++)
	{
		if (m_table3DMetaData[i].locationId == locationid)
		{
			return m_table3DMetaData[i];
		}
	}
	return Table3DMetaData();
}


const Table2DMetaData MemoryMetaData::get2DMetaData(unsigned short locationid)
{
	for (int i=0;i<m_table2DMetaData.size();i++)
	{
		if (m_table2DMetaData[i].locationId == locationid)
		{
			return m_table2DMetaData[i];
		}
	}
	return Table2DMetaData();
}
bool MemoryMetaData::has3DMetaData(unsigned short locationid)
{
	for (int i=0;i<m_table3DMetaData.size();i++)
	{
		if (m_table3DMetaData[i].locationId == locationid)
		{
			return true;
		}
	}
	return false;
}

bool MemoryMetaData::has2DMetaData(unsigned short locationid)
{
	for (int i=0;i<m_table2DMetaData.size();i++)
	{
		if (m_table2DMetaData[i].locationId == locationid)
		{
			return true;
		}
	}
	return false;
}
bool MemoryMetaData::hasRORMetaData(unsigned short locationid)
{
	for (int i=0;i<m_readOnlyMetaData.size();i++)
	{
		if (m_readOnlyMetaData[i].locationId == locationid)
		{
			return true;
		}
	}
	return false;
}

const ReadOnlyRamData MemoryMetaData::getRORMetaData(unsigned short locationid)
{
	for (int i=0;i<m_readOnlyMetaData.size();i++)
	{
		if (m_readOnlyMetaData[i].locationId == locationid)
		{
			return m_readOnlyMetaData[i];
		}
	}
	return ReadOnlyRamData();
}

const QString MemoryMetaData::getErrorString(unsigned short code)
{
	if (m_errorMap.contains(code))
	{
		return m_errorMap[code];
	}
	return "0x" + QString::number(code,16).toUpper();
}
