#ifndef TABLE2DDATA_H
#define TABLE2DDATA_H
#include <QString>
#include <QList>
#include <QByteArray>

class Table2DData
{
public:
	Table2DData();
	void parsePayload(QByteArray payload);
private:
	QList<double> m_valueList;
	QString xAxisLabel;
	QString yAxisLabel;
};

#endif // TABLE2DDATA_H
