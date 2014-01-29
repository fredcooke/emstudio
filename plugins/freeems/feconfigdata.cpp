#include "feconfigdata.h"
#include "QsLog.h"
FEConfigData::FEConfigData() : ConfigData()
{
	m_value = QVariant((double)0);
}
FEConfigData::FEConfigData(QString name,QString type, QString override,unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc) : ConfigData()
{
    m_name = name;
    m_typeString = type;
    m_sizeOverride = override;
    m_locationId = locationid;
    m_size = size;
    m_elementSize = elementsize;
    m_offset = offset;
    m_calc = calc;
    //Size of element is the number of bytes per value
    //size is the number of values (1 for value, 1+ for array);

}
QVariant FEConfigData::value()
{
	//QLOG_DEBUG() << "value requested for" << m_name << ":" << m_value;
	return m_value;
}
void FEConfigData::setValue(QVariant value)
{
	if (m_typeString == "value")
	{
		double dval = value.toDouble();
		unsigned short usval = reverseCalcAxis(dval,m_calc);
		QByteArray data;
		for (int i=0;i<m_elementSize;i++)
		{
			data.append(usval >> (((m_elementSize-1)-i)*8));
		}
		emit saveSingleDataToFlash(m_locationId,m_offset,m_elementSize,data);
	}
	else if (m_typeString == "array")
	{
		//It will be a list of doubles?
		QVariantList list = value.toList();
		QByteArray data;
		for (int i=0;i<list.size();i++)
		{
			double dval = list[i].toDouble();
			unsigned short usval = reverseCalcAxis(dval,m_calc);
			for (int j=0;j<m_elementSize;j++)
			{
				data.append(usval >> (((m_elementSize-1)-j)*8));
			}
		}
		if (data.size() < m_size)
		{
			//We're only editing the first part of the array.
		}
		emit saveSingleDataToFlash(m_locationId,m_offset,data.size(),data);
	}
}

void FEConfigData::setData(QByteArray data)
{
    if (m_typeString == "value")
    {
	if (data.size() >= (m_offset + m_elementSize))
	{
	    QByteArray newdata = data.mid(m_offset,m_elementSize);
	    unsigned long val = 0;
	    for (int i=0;i<m_elementSize;i++)
	    {
		val += ((unsigned char)newdata[i]) << (((m_elementSize-1) - i) * 8);
	    }
	    m_value = QVariant(calcAxis(val,m_calc));

	}
	else
	{
		//QLOG_DEBUG() << "Data size:" << data.size() << "Expected:" << m_offset + m_elementSize;
	}

    }
    else if (m_typeString == "array")
    {
	    qDebug() << "Array type";
	    QByteArray newdata = data.mid(m_offset,m_size);
	    QVariantList valuelist;
	    for (int i=0;i<m_size;i+=m_elementSize)
	    {
		    unsigned long val = 0;
		    for (int j=0;j<m_elementSize;j++)
		    {
			val += ((unsigned char)newdata[i+j]) << (((m_elementSize-1) - j) * 8);
		    }
		    double properval = calcAxis(val,m_calc);
		    valuelist.append(properval);
	    }
	    m_value = valuelist;
    }
    emit update();
}
QStringList FEConfigData::getEnumValues()
{
	return QStringList();
}

void FEConfigData::setEnumValues(QStringList values)
{
	Q_UNUSED(values)
}

void FEConfigData::setEnumBits(int min,int max)
{
	Q_UNUSED(min)
	Q_UNUSED(max)
}

int FEConfigData::getMinEnumBit()
{
	return 0;
}

int FEConfigData::getMaxEnumBit()
{
	return 0;
}

void FEConfigData::saveToFlash()
{

}

double FEConfigData::calcAxis(unsigned short val,QList<QPair<QString,double> > metadata)
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
unsigned short FEConfigData::reverseCalcAxis(double val,QList<QPair<QString,double> > metadata)
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
