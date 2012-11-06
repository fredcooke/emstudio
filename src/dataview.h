#ifndef DATAVIEW_H
#define DATAVIEW_H
#include <QWidget>
#include <QVariantMap>
class DataView : public QWidget
{
	Q_OBJECT
public:
	virtual bool setData(unsigned short locationid,QByteArray data)=0;
	virtual void passDatalog(QVariantMap data)=0;
};

#endif // DATAVIEW_H
