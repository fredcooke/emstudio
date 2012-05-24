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

#ifndef DATARAWVIEW_H
#define DATARAWVIEW_H

#include <QWidget>
#include "qhexedit.h"
class DataRawView : public QWidget
{
	Q_OBJECT
public:
	explicit DataRawView(QWidget *parent = 0);
private:
	QHexEdit *hexEdit;
protected:
	void closeEvent(QCloseEvent *event);
signals:
	
public slots:
	void setData(QByteArray data);
};

#endif // DATARAWVIEW_H
