#ifndef RAWDATAVIEW_H
#define RAWDATAVIEW_H

#include <QWidget>
#include "ui_rawdataview.h"

class RawDataView : public QWidget
{
	Q_OBJECT
	
public:
	explicit RawDataView(QWidget *parent = 0);
	~RawDataView();
	void setData(unsigned short locationid,QByteArray data,bool isram);
private:
	bool m_isRam;
	Ui::RawDataView ui;
	unsigned short m_locationId;

private slots:
	void saveButtonClicked();
signals:
	void saveData(unsigned short locationid,QByteArray data,int physicallocation);
};

#endif // RAWDATAVIEW_H
