#include "rawdataview.h"


RawDataView::RawDataView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.saveChangesButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
}
void RawDataView::setData(unsigned short locationid,QByteArray data)
{
	m_locationId = locationid;
	ui.hexEditor->setData(data);
	ui.locationIdLabel->setText("0x" + QString::number(locationid,16).toUpper());
}

RawDataView::~RawDataView()
{
}

void RawDataView::saveButtonClicked()
{
	emit saveData(m_locationId,ui.hexEditor->data(),0); //0 for RAM, 1 for flash.
}
