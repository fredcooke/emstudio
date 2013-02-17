#ifndef TABLEDATA_H
#define TABLEDATA_H
#include <QObject>
#include <QList>
#include <QPair>
class TableData : public QObject
{
	Q_OBJECT
public:
	TableData();
	double calcAxis(unsigned short val,QList<QPair<QString,double> > metadata);
	unsigned short backConvertAxis(double val,QList<QPair<QString,double> > metadata);
};

#endif // TABLEDATA_H
