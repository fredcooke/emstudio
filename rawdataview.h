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
	void setData(unsigned short locationid,QByteArray data);
private:
	Ui::RawDataView ui;
private slots:
	void saveButtonClicked();
};

#endif // RAWDATAVIEW_H
