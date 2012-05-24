#include "rawdataview.h"


RawDataView::RawDataView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.saveChangesButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
}
void RawDataView::setData(unsigned short locationid,QByteArray data)
{
	ui.hexEditor->setData(data);
	ui.locationIdLabel->setText("0x" + QString::number(locationid,16).toUpper());
}

RawDataView::~RawDataView()
{
}

void RawDataView::saveButtonClicked()
{

}
