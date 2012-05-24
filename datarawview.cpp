/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "datarawview.h"
#include <QHBoxLayout>
DataRawView::DataRawView(QWidget *parent) : QWidget(parent)
{
	hexEdit = new QHexEdit(this);
	QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	horizontalLayout->addWidget(hexEdit);
}

void DataRawView::setData(unsigned short locid, QByteArray data)
{
	hexEdit->setData(data);
}
void DataRawView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
