#ifndef CONFIGBLOCK_H
#define CONFIGBLOCK_H
#include <QString>
#include <QPair>
#include <QList>
class ConfigBlock
{
public:
	ConfigBlock();
	ConfigBlock(QString name,QString type, unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc);
	void setName(QString name) { m_name = name; }
	void setType(QString type) { m_type = type; }
	void setLocationId(unsigned short locationid) { m_locationId = locationid; }
	void setSize(unsigned short size) { m_size = size; }
	void setElementSize(unsigned short size) { m_elementSize = size; }
	void setOffset(unsigned short offset) { m_offset = offset; }
	void setCalc(QList<QPair<QString,double> > calc) { m_calc = calc; }
	QString name() { return m_name; }
	QString type() { return m_type; }
	unsigned short locationId() { return m_locationId; }
	unsigned short offset() { return m_offset; }
	unsigned short size() { return m_size; }
	unsigned short elementSize() { return m_elementSize; }
	QList<QPair<QString,double> > calc() { return m_calc; }
private:
	QString m_name;
	QString m_type;
	unsigned short m_locationId;
	unsigned short m_size;
	unsigned short m_elementSize;
	unsigned short m_offset;
	QList<QPair<QString,double> > m_calc;


};

#endif // CONFIGBLOCK_H
