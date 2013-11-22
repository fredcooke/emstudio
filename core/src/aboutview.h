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

#ifndef ABOUTVIEW_H
#define ABOUTVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include <QMdiSubWindow>
#include "ui_aboutview.h"

class AboutView : public QWidget
{
	Q_OBJECT
	
public:
	explicit AboutView(QWidget *parent = 0);
	~AboutView();
	void setHash(QString hash);
	void setVersionLabel(QString version);
	void setCommit(QString commit);
	void setDate(QString date);
protected:
	void closeEvent(QCloseEvent *event);
private:
	QString m_commit;
	QString m_hash;
	Ui::AboutView ui;
signals:
	void windowHiding(QMdiSubWindow *parent);
};

#endif // ABOUTVIEW_H
