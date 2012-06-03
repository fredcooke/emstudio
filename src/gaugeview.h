/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#ifndef GAUGEVIEW_H
#define GAUGEVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include "ui_datagauges.h"
#include "gaugewidget.h"
#include "datapacketdecoder.h"
class GaugeView : public QWidget
{
	Q_OBJECT

public:
	explicit GaugeView(QWidget *parent = 0);
	~GaugeView();
	void passData(QVariantMap data);
	void passDecoder(DataPacketDecoder *decoder);
private:
	QTimer *guiUpdateTimer;
	DataPacketDecoder *dataPacketDecoder;
	QVariantMap m_valueMap;
	Ui::DataGauges ui;
	GaugeWidget *widget;
protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void guiUpdateTimerTick();
};

#endif // GAUGEVIEW_H
