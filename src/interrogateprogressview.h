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

#ifndef INTERROGATEPROGRESSVIEW_H
#define INTERROGATEPROGRESSVIEW_H

#include <QWidget>
#include "ui_interrogateprogressview.h"

class InterrogateProgressView : public QWidget
{
	Q_OBJECT
	
public:
	explicit InterrogateProgressView(QWidget *parent = 0);
	~InterrogateProgressView();
	void setProgress(int progress);
	void setMaximum(int maximum);
	int maximum() { return ui.progressBar->maximum(); }
	int progress() { return ui.progressBar->value(); }
private:
	Ui::InterrogateProgressView ui;
private slots:
	void cancelClickedSlot();
signals:
	void cancelClicked();
};

#endif // INTERROGATEPROGRESSVIEW_H
