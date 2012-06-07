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
	void passPacketNak(unsigned short locationid,QByteArray header,QByteArray payload);
	void passDecoderFailure(QByteArray packet);
protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::PacketStatusView ui;
};

#endif // PACKETSTATUSVIEW_H
