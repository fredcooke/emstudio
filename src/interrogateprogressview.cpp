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

#include "interrogateprogressview.h"


InterrogateProgressView::InterrogateProgressView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelClickedSlot()));
	ui.outputTableWidget->setColumnCount(3);
	ui.outputTableWidget->setColumnWidth(0,50);
	ui.outputTableWidget->setColumnWidth(1,300);
	ui.outputTableWidget->setColumnWidth(2,100);
}

InterrogateProgressView::~InterrogateProgressView()
{
}
void InterrogateProgressView::addOutput(QString output)
{
	ui.outputTableWidget->setRowCount(ui.outputTableWidget->rowCount()+1);
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,0,new QTableWidgetItem());
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,1,new QTableWidgetItem(output));
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,2,new QTableWidgetItem());
	ui.outputTableWidget->scrollToBottom();
}
void InterrogateProgressView::addTask(QString task, int sequencenumber)
{
	//ui.outputTextBrowser->append(QString::number(sequencenumber) + ": " + task + "...");
	ui.outputTableWidget->setRowCount(ui.outputTableWidget->rowCount()+1);
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(sequencenumber)));
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,1,new QTableWidgetItem(task));
	ui.outputTableWidget->setItem(ui.outputTableWidget->rowCount()-1,2,new QTableWidgetItem("In progress"));
	ui.outputTableWidget->scrollToBottom();
}

void InterrogateProgressView::taskFail(int sequencenumber)
{
	for (int i=0;i<ui.outputTableWidget->rowCount();i++)
	{
		if (ui.outputTableWidget->item(i,0)->text() == QString::number(sequencenumber))
		{
			ui.outputTableWidget->item(i,2)->setText("Failed");
			ui.outputTableWidget->item(i,2)->setBackgroundColor(QColor::fromRgb(255,0,0));
		}
	}
}

void InterrogateProgressView::taskSucceed(int sequencenumber)
{
	for (int i=0;i<ui.outputTableWidget->rowCount();i++)
	{
		if (ui.outputTableWidget->item(i,0)->text() == QString::number(sequencenumber))
		{
			ui.outputTableWidget->item(i,2)->setText("Ok");
			ui.outputTableWidget->item(i,2)->setBackgroundColor(QColor::fromRgb(0,255,0));
		}
	}
}

void InterrogateProgressView::setProgress(int progress)
{
	ui.progressBar->setValue(progress);
}

void InterrogateProgressView::setMaximum(int maximum)
{
	ui.progressBar->setMaximum(maximum);
}

void InterrogateProgressView::cancelClickedSlot()
{
	emit cancelClicked();
}
