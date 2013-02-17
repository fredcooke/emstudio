#include "configblock.h"

ConfigBlock::ConfigBlock()
{
}
ConfigBlock::ConfigBlock(QString name,QString type,unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc)
{
	m_name = name;
	m_type = type;
	m_locationId = locationid;
	m_size = size;
	m_elementSize = elementsize;
	m_offset = offset;
	m_calc = calc;
}
