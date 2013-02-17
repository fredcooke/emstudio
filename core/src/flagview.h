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
};

#endif // DATAFLAGS_H
