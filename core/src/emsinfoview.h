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

#ifndef EMSINFO_H
#define EMSINFO_H

#include <QWidget>
#include <QCloseEvent>
#include "memorylocationinfo.h"
#include "datatype.h"
#include "ui_emsinfo.h"

class EmsInfoView : public QWidget
{
    Q_OBJECT

public:
	explicit EmsInfoView(QWidget *parent = 0);
	~EmsInfoView();
	void setLocalRam(bool dirty);

	void setDeviceFlash(bool dirty);
	void clear();
	void setInterrogationData(QMap<QString,QString> datamap);
protected:
	void closeEvent(QCloseEvent *event);
public slots:
	void locationInfoWidgetDoubleClicked(int row, int column);
	void locationIdInfo(unsigned short locationid,QString title,MemoryLocationInfo info);
	void normalViewClicked();
	void hexViewClicked();
private:
	Ui::EmsInfo ui;
signals:
	void displayLocationId(int locid, bool isram,DataType type);
	void checkSync();
};

#endif // EMSINFO_H
