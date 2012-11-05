#ifndef HEADERS_H
#define HEADERS_H
#include <QPair>
#include <QList>
#include <QString>
class ConfigData
{
public:
	QString configTitle;
	QString configDescription;
	unsigned short locationId;
	unsigned char offset;
	unsigned char elementSize;
	QList<QPair<QString,double> > elementCalc;
	QString type;
	unsigned char arraySize;

};


class ReadOnlyRamData
{
public:
	QString dataTitle;
	QString dataDescription;
	unsigned short locationId;
	unsigned char offset;
	unsigned char size;
	QList<QPair<QString,double> > calc;
};
class ReadOnlyRamBlock
{
public:
	QString title;
	unsigned short locationid;
	QList<ReadOnlyRamData> m_ramData;
};

class Table3DMetaData
{
public:
	Table3DMetaData() { xDp=2; yDp=2; zDp=2; valid=false;}
	QString tableTitle;
	bool valid;
	unsigned int size;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	QString zAxisTitle;
	int xDp;
	int yDp;
	int zDp;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
	QList<QPair<QString,double> > zAxisCalc;
};
class Table2DMetaData
{
public:
	Table2DMetaData() { xDp=2; yDp=2; valid=false;}
	QString tableTitle;
	bool valid;
	unsigned int size;
	unsigned short locationId;
	QString xAxisTitle;
	QString yAxisTitle;
	int xDp;
	int yDp;
	QList<QPair<QString,double> > xAxisCalc;
	QList<QPair<QString,double> > yAxisCalc;
};

#endif // HEADERS_H
