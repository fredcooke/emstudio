#ifndef HEADERS_H
#define HEADERS_H
#include <QPair>
#include <QList>
#include <QString>
class Table3DMetaData
{
public:
	QString tableTitle;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	QString zAxisTitle;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
	QList<QPair<QString,double> > zAxisCalc;
};
class Table2DMetaData
{
public:
	QString tableTitle;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
};

#endif // HEADERS_H
