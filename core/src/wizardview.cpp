#include "wizardview.h"

WizardView::WizardView(EmsComms *comms,QWidget *parent) : QDeclarativeView(parent)
{
	this->rootContext()->setContextProperty("decoder",comms->getDecoder());
	this->rootContext()->setContextProperty("emscomms",comms);
	setSource(QUrl::fromLocalFile("wizard.qml"));
}
