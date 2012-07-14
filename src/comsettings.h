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
#ifndef COMSETTINGS_H
#define COMSETTINGS_H

#include <QWidget>
#include "ui_comsettings.h"

class ComSettings : public QWidget
{
	Q_OBJECT

public:
	Q_PROPERTY(QString comport READ getComPort WRITE setComPort)
	explicit ComSettings(QWidget *parent = 0);
	~ComSettings();
	QString getComPort();
	void setComPort(QString port);
	int getBaud();
	QString getDataLogDir();
	bool getSaveDataLogs();
	bool getClearDataLogs();
	int getNumLogsToSave();
	void setDataLogDir(QString dir);
	void setSaveDataLogs(bool save);
	void setClearDataLogs(bool clear);
	void setNumLogsToSave(int num);
	void setBaud(int baud);
	int getInterByteDelay();
	void setInterByteDelay(int delay);
private:
	Ui::ComSettings ui;
signals:
	void saveClicked();
	void cancelClicked();
};

#endif // COMSETTINGS_H
