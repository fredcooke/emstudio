#include "rawdataview.h"


RawDataView::RawDataView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
}
void RawDataView::setData(unsigned short locationid,QByteArray data)
{
	ui.hexEditor->setData(data);
	ui.locationIdLabel->setText("0x" + QString::number(locationid,16).toUpper());
}

RawDataView::~RawDataView()
{
}
