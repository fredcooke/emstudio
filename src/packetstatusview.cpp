#include "packetstatusview.h"
#include <QCloseEvent>
#include <QMdiSubWindow>
PacketStatusView::PacketStatusView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.packetCountTableWidget->setColumnCount(3);
	ui.packetCountTableWidget->setColumnWidth(0,150);
	ui.packetCountTableWidget->setColumnWidth(1,50);
	ui.packetCountTableWidget->setColumnWidth(2,50);
	ui.packetCountTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Payload ID"));
	ui.packetCountTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Success"));
	ui.packetCountTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Failure"));

	ui.failedPacketTableWidget->setColumnCount(3);
	ui.failedPacketTableWidget->setColumnWidth(0,150);
	ui.failedPacketTableWidget->setColumnWidth(1,50);
	ui.failedPacketTableWidget->setColumnWidth(2,500);
	ui.failedPacketTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Payload ID"));
	ui.failedPacketTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Count"));
	ui.failedPacketTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Packet Contents"));

	ui.decoderFailureTableWidget->setColumnCount(1);
	ui.decoderFailureTableWidget->setColumnWidth(0,1000);
	ui.decoderFailureTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Failed Packet Bytes"));
}
void PacketStatusView::passPacketSent(unsigned short locationid,QByteArray header,QByteArray payload)
{
	for (int i=0;i<ui.packetCountTableWidget->rowCount();i++)
	{
		if (ui.packetCountTableWidget->item(i,0)->text() == QString::number(locationid,16).toUpper())
		{
			return;
		}
	}
	ui.packetCountTableWidget->setRowCount(ui.packetCountTableWidget->rowCount()+1);
	ui.packetCountTableWidget->setItem(ui.packetCountTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(locationid,16).toUpper()));
	ui.packetCountTableWidget->setItem(ui.packetCountTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.packetCountTableWidget->setItem(ui.packetCountTableWidget->rowCount()-1,2,new QTableWidgetItem("0"));
}

void PacketStatusView::passPacketAck(unsigned short locationid,QByteArray header,QByteArray payload)
{
	for (int i=0;i<ui.packetCountTableWidget->rowCount();i++)
	{
		if (ui.packetCountTableWidget->item(i,0)->text() == QString::number(locationid,16).toUpper())
		{
			int count = ui.packetCountTableWidget->item(i,1)->text().toInt();
			count++;
			ui.packetCountTableWidget->item(i,1)->setText(QString::number(count));
			return;
		}
	}
}

void PacketStatusView::passPacketNak(unsigned short locationid,QByteArray header,QByteArray payload)
{
	for (int i=0;i<ui.packetCountTableWidget->rowCount();i++)
	{
		if (ui.packetCountTableWidget->item(i,0)->text() == QString::number(locationid,16).toUpper())
		{
			int count = ui.packetCountTableWidget->item(i,2)->text().toInt();
			count++;
			ui.packetCountTableWidget->item(i,1)->setText(QString::number(count));
			ui.failedPacketTableWidget->setRowCount(ui.failedPacketTableWidget->rowCount()+1);
			ui.failedPacketTableWidget->setItem(ui.failedPacketTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(locationid,16).toUpper()));
			ui.failedPacketTableWidget->setItem(ui.failedPacketTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
			QString packettext="";
			for (int j=0;j<header.size();j++)
			{
				packettext += ((header[j] <= 0xF) ? "0" : "") + QString::number(header[j],16).toUpper() + " ";
			}
			for (int j=0;j<payload.size();j++)
			{
				packettext += ((payload[j] <= 0xF) ? "0" : "") + QString::number(payload[j],16).toUpper();
			}
			ui.failedPacketTableWidget->setItem(ui.failedPacketTableWidget->rowCount()-1,2,new QTableWidgetItem(packettext));
			return;
		}
	}
}
void PacketStatusView::passDecoderFailure(QByteArray packet)
{
	QString packettext="";
	for (int j=0;j<packet.size();j++)
	{
		packettext += ((packet[j] <= 0xF) ? "0" : "") + QString::number(packet[j],16).toUpper();
	}
	ui.decoderFailureTableWidget->setRowCount(ui.decoderFailureTableWidget->rowCount()+1);
	ui.decoderFailureTableWidget->setItem(ui.decoderFailureTableWidget->rowCount()-1,0,new QTableWidgetItem(packettext));
}

void PacketStatusView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
PacketStatusView::~PacketStatusView()
{
}
