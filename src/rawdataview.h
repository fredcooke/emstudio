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
	//void verifyData(unsigned short locationid,QByteArray data);
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
