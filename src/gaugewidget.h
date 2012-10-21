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
    void setFile(QString file);
};

#endif // GAUGEWIDGET_H
