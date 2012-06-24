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

#ifndef PACKETSTATUSVIEW_H
#define PACKETSTATUSVIEW_H

#include <QWidget>
#include "ui_packetstatusview.h"
class PacketStatusView : public QWidget
{
	Q_OBJECT
	
public:
	explicit PacketStatusView(QWidget *parent = 0);
	~PacketStatusView();
public slots:
	void passPacketSent(unsigned short locationid,QByteArray header,QByteArray payload);
	void passPacketAck(unsigned short locationid,QByteArray header,QByteArray payload);
	void passPacketNak(unsigned short locationid,QByteArray header,QByteArray payload,unsigned short errornum);
	void passDecoderFailure(QByteArray packet);
protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::PacketStatusView ui;
};

#endif // PACKETSTATUSVIEW_H
