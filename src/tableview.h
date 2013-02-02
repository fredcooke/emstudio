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

#ifndef DATATABLES_H
#define DATATABLES_H

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>
#include "ui_datatables.h"
#include "freeems/datapacketdecoder.h"
class TableView : public QWidget
{
    Q_OBJECT

public:
	explicit TableView(QWidget *parent = 0);
	~TableView();
	void passData(QVariantMap data);
	void passDecoder(DataPacketDecoder *decoder);
private:
	QTimer *guiUpdateTimer;
	DataPacketDecoder *dataPacketDecoder;
	Ui::DataTables ui;
	QVariantMap m_valueMap;
	QMap<QString,int> m_nameToIndexMap;
protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void guiUpdateTimerTick();
};

#endif // DATATABLES_H
