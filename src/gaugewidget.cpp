/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/
#include "gaugewidget.h"
#include <QMetaType>
#include <QDeclarativeContext>
#include <QFile>
#include <QDir>
GaugeWidget::GaugeWidget(QWidget *parent) : QDeclarativeView(parent)
{
	qmlRegisterType<GaugeItem>("GaugeImage",0,1,"GaugeImage");
	this->rootContext()->setContextProperty("propertyMap",&propertyMap);
	/*if (QFile::exists("gauges.qml"))
	{
		setSource(QUrl("gauges.qml"));
	}
	else if (QFile::exists("src/gauges.qml"))
	{
		setSource(QUrl("src/gauges.qml"));
	}
	else if (QFile::exists("/etc/emstudio/gauges.qml"))
	{
		setSource(QUrl("/etc/emstudio/gauges.qml"));
	}
	else
	{
		qDebug() << "Unable to locate gauges.qml";
	}*/
}

void GaugeWidget::setFile(QString file)
{
	setSource(QUrl(file));
}
