#ifndef TABLE2DDATA_H
#define TABLE2DDATA_H
#include <QString>
#include <QList>
#include <QByteArray>

class Table2DData
{
public:
	Table2DData();
	Table2DData(unsigned short locationid,QByteArray payload);
	void setData(unsigned short locationid,QByteArray payload);
	QByteArray data();
private:
	unsigned short m_locationId;
	QList<unsigned short> m_xaxis;
	QList<unsigned short> m_yaxis;
	QString xAxisLabel;
	QString yAxisLabel;
};

#endif // TABLE2DDATA_H
