#include "feconfigdata.h"
#include "QsLog.h"
FEConfigData::FEConfigData() : ConfigData()
{
	m_value = QVariant((double)0);
}
FEConfigData::FEConfigData(QString name,QString type, QString override,unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc) : ConfigData()
{
    m_name = name;
    m_type = type;
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
	QLOG_DEBUG() << "value requested:" << m_value;
	return m_value;
}
void FEConfigData::setValue(QVariant value)
{
	if (m_type == "value")
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
	else if (m_type == "array")
	{

	}
}

void FEConfigData::setData(QByteArray data)
{
    if (m_type == "value")
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
    else if (m_type == "array")
    {
    }
    emit update();
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
