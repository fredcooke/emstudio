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
#ifndef EMSINFO_H
#define EMSINFO_H

#include <QWidget>
#include <QCloseEvent>
#include "ui_emsinfo.h"
#include "freeemscomms.h"
class EmsInfoView : public QWidget
{
    Q_OBJECT

public:
	explicit EmsInfoView(QWidget *parent = 0);
	~EmsInfoView();
	void setInterfaceVersion(QString version);
	void setFirmwareVersion(QString firmware);
	void setLocalRam(bool dirty);

	void setDeviceFlash(bool dirty);
	void clear();
protected:
	void closeEvent(QCloseEvent *event);
public slots:
	void locationInfoWidgetDoubleClicked(int row, int column);
	void locationIdInfo(unsigned short locationid,QString title,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size);
	void normalViewClicked();
	void hexViewClicked();
private:
	Ui::EmsInfo ui;
signals:
	void displayLocationId(int locid, bool isram,int type);
	void checkSync();
};

#endif // EMSINFO_H
