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

#ifndef COMSETTINGS_H
#define COMSETTINGS_H

#include <QWidget>
#include <QMdiSubWindow>
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
protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void browseLogFolderClicked();
private:
	Ui::ComSettings ui;
signals:
	void saveClicked();
	void cancelClicked();
	void windowHiding(QMdiSubWindow *parent);
};

#endif // COMSETTINGS_H
