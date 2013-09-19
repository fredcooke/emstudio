#ifndef WIZARDVIEW_H
#define WIZARDVIEW_H

#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QTimer>
#include <emscomms.h>
class WizardView : public QDeclarativeView
{
	Q_OBJECT

public:
	explicit WizardView(EmsComms *comms,QWidget *parent = 0);
	Q_INVOKABLE void setMemory(unsigned short locationid,unsigned short offset,unsigned short length,QVariantList array);
private:
	QTimer *updateTimer;
	QVariantMap m_savedPayload;
	EmsComms *emscomms;
signals:
	void payloadDecoded(QVariantMap data);
public slots:
private slots:
	void decoderPayloadDecoded(QVariantMap values);
	void updateTimerTick();
};

#endif // WIZARDVIEW_H
