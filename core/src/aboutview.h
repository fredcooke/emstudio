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
	void setCommit(QString commit);
protected:
	void closeEvent(QCloseEvent *event);
private:
	QString m_commit;
	QString m_hash;
	Ui::AboutView ui;
};

#endif // ABOUTVIEW_H
