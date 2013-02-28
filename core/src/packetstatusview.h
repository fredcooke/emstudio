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
	void clear();
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
