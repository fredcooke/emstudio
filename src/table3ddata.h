#ifndef TABLE3DDATA_H
#define TABLE3DDATA_H

#include <QObject>

class Table3DData : public QObject
{
	Q_OBJECT
public:
	Table3DData(unsigned short locationid, QByteArray data);
	void setData(unsigned short locationid,QByteArray payload);
	QByteArray data();
	QList<unsigned short> xAxis();
	QList<unsigned short> yAxis();
	QList<QList<unsigned short> > values();
	int columns();
	int rows();
	void setCell(int row, int column,unsigned short newval);
	void setXAxis(int column,unsigned short newval);
	void setYAxis(int row,unsigned short newval);
private:
	unsigned short m_locationId;
	QList<unsigned short> m_xAxis;
	QList<unsigned short> m_yAxis;
	QList<QList<unsigned short> > m_values;
	QString xAxisLabel;
	QString yAxisLabel;
	QString valuesLabel;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
public slots:
	
};

#endif // TABLE3DDATA_H
