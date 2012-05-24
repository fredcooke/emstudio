#include "rawdataview.h"
#include "ui_rawdataview.h"

RawDataView::RawDataView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RawDataView)
{
	ui->setupUi(this);
}

RawDataView::~RawDataView()
{
	delete ui;
}
