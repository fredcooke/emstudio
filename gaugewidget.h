#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H
#include <QDeclarativeView>
#include <QDeclarativePropertyMap>

#include "gaugeitem.h"
class GaugeWidget : public QDeclarativeView
{
public:
    GaugeWidget(QWidget *parent=0);
    QDeclarativePropertyMap propertyMap;
};

#endif // GAUGEWIDGET_H
