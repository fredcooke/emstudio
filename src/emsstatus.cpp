#include "emsstatus.h"


EmsStatus::EmsStatus(QWidget *parent) : QDockWidget(parent)
{
	ui.setupUi(this);
	connect(ui.dockPushButton,SIGNAL(clicked()),this,SIGNAL(dockRequested()));
}

EmsStatus::~EmsStatus()
{
}
void EmsStatus::dockButtonClicked()
{

}
