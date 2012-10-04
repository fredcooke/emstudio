#include "emsstatus.h"


EmsStatus::EmsStatus(QWidget *parent) : QDockWidget(parent)
{
	ui.setupUi(this);
	connect(ui.hardResetPushButton,SIGNAL(clicked()),this,SIGNAL(hardResetRequest()));
	connect(ui.softResetPushButton,SIGNAL(clicked()),this,SIGNAL(softResetRequest()));
}

EmsStatus::~EmsStatus()
{
}
