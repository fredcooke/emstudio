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

#ifndef DATAFLAGS_H
#define DATAFLAGS_H

#include <QWidget>
#include <QCloseEvent>
#include <QMdiSubWindow>
#include "datapacketdecoder.h"
#include "ui_dataflags.h"
class FlagView : public QWidget
{
	Q_OBJECT

public:
	explicit FlagView(QWidget *parent = 0);
	~FlagView();
	void passData(QVariantMap data);
	void passDecoder(DataPacketDecoder *decoder);
protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::DataFlags ui;
signals:
	void windowHiding(QMdiSubWindow *parent);
};

#endif // DATAFLAGS_H
