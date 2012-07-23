#include "tabledata.h"

TableData::TableData()
{
}
double TableData::calcAxis(unsigned short val,QList<QPair<QString,double> > metadata)
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
unsigned short TableData::backConvertAxis(double val,QList<QPair<QString,double> > metadata)
{
	if (metadata.size() == 0)
	{
		return val;
	}
	double newval = val;
	for (int j=metadata.size()-1;j<=0;j++)
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
