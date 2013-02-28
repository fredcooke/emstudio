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

#ifndef RAWDATAVIEW_H
#define RAWDATAVIEW_H

#include <QWidget>
#include "dataview.h"
#include "ui_rawdataview.h"

class RawDataView : public DataView
{
	Q_OBJECT
	
public:
	explicit RawDataView(bool isram, bool isflash,QWidget *parent = 0);
	~RawDataView();
	bool setData(unsigned short locationid,QByteArray data);
	bool setData(unsigned short locationid,QByteArray data,TableData *tdata){Q_UNUSED(locationid); Q_UNUSED(data); Q_UNUSED(tdata); return false;}
	//void verifyData(unsigned short locationid,QByteArray data);
	void passDatalog(QVariantMap data);
private:
	bool m_isRam;
	Ui::RawDataView ui;
	unsigned short m_locationId;

private slots:
	void saveFlashButtonClicked();
	void saveRamButtonClicked();
	void loadRamButtonClicked();
	void loadFlashButtonClicked();
signals:
	void saveData(unsigned short locationid,QByteArray data,int physicallocation);
	void reloadData(unsigned short locationid,bool isram);
};

#endif // RAWDATAVIEW_H
