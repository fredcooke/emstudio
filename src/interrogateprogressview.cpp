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
}

InterrogateProgressView::~InterrogateProgressView()
{
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
