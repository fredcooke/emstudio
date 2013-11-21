/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

#ifndef GAUGEVIEW_H
#define GAUGEVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include <QMdiSubWindow>
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
	void setFile(QString file);
private:
	QList<QString> propertiesInUse;
	QString file;
	QTimer *guiUpdateTimer;
	DataPacketDecoder *dataPacketDecoder;
	QVariantMap m_valueMap;
	Ui::DataGauges ui;
	GaugeWidget *widget;
protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void guiUpdateTimerTick();
signals:
	void windowHiding(QMdiSubWindow *parent);
};

#endif // GAUGEVIEW_H
