#include "gaugewidget.h"
#include <QMetaType>
#include <QDeclarativeContext>
GaugeWidget::GaugeWidget(QWidget *parent) : QDeclarativeView(parent)
{
	qmlRegisterType<GaugeItem>("GaugeImage",0,1,"GaugeImage");
	this->rootContext()->setContextProperty("propertyMap",&propertyMap);
	setSource(QUrl("gauges.qml"));
}
