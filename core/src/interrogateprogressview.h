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

#ifndef INTERROGATEPROGRESSVIEW_H
#define INTERROGATEPROGRESSVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include "ui_interrogateprogressview.h"
#include "overviewprogressitemdelegate.h"
class InterrogateProgressView : public QWidget
{
	Q_OBJECT
	
public:
	explicit InterrogateProgressView(QWidget *parent = 0);
	~InterrogateProgressView();
	void setProgress(int progress);
	void setMaximum(int maximum);
	void addOutput(QString output);
	void reset();
	void done();
	void addTask(QString task, int sequencenumber,int type);
	void taskFail(int sequencenumber);
	void taskSucceed(int sequencenumber);
	int maximum() { return ui.progressBar->maximum(); }
	int progress() { return ui.progressBar->value(); }
protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::InterrogateProgressView ui;
	QMap<int,int> m_typeToOverviewListMap;
private slots:
	void cancelClickedSlot();
signals:
	void cancelClicked();
};

#endif // INTERROGATEPROGRESSVIEW_H
