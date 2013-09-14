#ifndef WIZARDVIEW_H
#define WIZARDVIEW_H

#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <emscomms.h>
class WizardView : public QDeclarativeView
{
	Q_OBJECT
public:
	explicit WizardView(EmsComms *comms,QWidget *parent = 0);
	
signals:
	
public slots:
	
};

#endif // WIZARDVIEW_H
