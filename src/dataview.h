#ifndef DATAVIEW_H
#define DATAVIEW_H
#include <QWidget>

class DataView : public QWidget
{
	Q_OBJECT
public:
	virtual bool setData(unsigned short locationid,QByteArray data)=0;
};

#endif // DATAVIEW_H
