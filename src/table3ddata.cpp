#include "table3ddata.h"
#include <QDebug>
Table3DData::Table3DData(unsigned short locationid, QByteArray data) : QObject()
{
	setData(locationid,data);
}
void Table3DData::setData(unsigned short locationid,QByteArray data)
{
	/*if (data.size() != 64)
	{
		qDebug() << "Passed a data pack to a 2d table that was of size" << data.size() << "should be 64!!!";
		return;
	}*/
	//qDebug() << "TableView2D::passData" << "0x" + QString::number(locationid,16).toUpper();
	m_locationId = locationid;
	//m_physicalid = physicallocation;

	//ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
	//ui.tableWidget->clear();
	//ui.tableWidget->horizontalHeader()->hide();
	//ui.tableWidget->verticalHeader()->hide();
	unsigned short xaxissize = (((unsigned char)data[0]) << 8) + (unsigned char)data[1];
	unsigned short yaxissize = (((unsigned char)data[2]) << 8) + (unsigned char)data[3];
	qDebug() << "XAxis:" << xaxissize;
	qDebug() << "YAxis:" << yaxissize;

	//m_xAxisSize = xaxissize;
	//m_yAxisSize = yaxissize;

	//ui.tableWidget->setRowCount(xaxissize+1);
	//ui.tableWidget->setColumnCount(yaxissize+1);

	for (int i=0;i<xaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[4+i]) << 8) + (unsigned char)data[5+i];
		//ui.tableWidget->setItem((xaxissize-1) - (i/2),0,new QTableWidgetItem(QString::number(val)));
		m_xAxis.append(val);
	}
	for (int i=0;i<yaxissize*2;i+=2)
	{
		unsigned short val = (((unsigned char)data[58+i]) << 8) + (unsigned char)data[59+i];
		//ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,(i/2)+1,new QTableWidgetItem(QString::number(val)));
		m_yAxis.append(val);
	}
	/*for (int i=0;i<xaxissize;i++)
	{
		m_values.append(QList<unsigned short>());
		for (int j=0;j<yaxissize;j++)
		{
			m_values[m_values.size()-1].append(0);
		}
	}*/
	for (int i=0;i<yaxissize*2;i+=2)
	{
		QList<unsigned short> currrow;
		for (int j=0;j<xaxissize*2;j+=2)
		{
			//unsigned short val = (((unsigned char)data[4+((xaxissize+yaxissize)*2) + (i*(xaxissize*2)) + j]) << 8) + (unsigned char)data[5+((yaxissize+xaxissize)*2) + (i*((xaxissize)*2)) + j];
			unsigned short val = (((unsigned char)data[100 + j + (i * xaxissize)]) << 8) + (unsigned char)data[101 + j + (i * xaxissize)];
			//ui.tableWidget->setItem((xaxissize-1)-((i/2)),(j/2)+1,new QTableWidgetItem(QString::number(val)));
			//m_values[i/2].replace(j/2,val);
			currrow.append(val);
			/*if (val < 65535/4)
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(0,(255*((val)/(65535.0/4.0))),255));
			}
			else if (val < ((65535/4)*2))
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(0,255,255-(255*((val-((65535/4.0)))/(65535.0/4.0)))));
			}
			else if (val < ((65535/4)*3))
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb((255*((val-((65535/4.0)*2))/(65535.0/4.0))),255,0));
			}
			else
			{
				ui.tableWidget->item((xaxissize-1)-((i/2)),(j/2)+1)->setBackgroundColor(QColor::fromRgb(255,255-(255*((val-((65535/4.0)*3))/(65535.0/4.0))),0));
			}*/

		}
		m_values.append(currrow);
	}
}
void Table3DData::setXAxis(int column,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,4+(((m_xAxis.size()) - column)*2),2);
}

void Table3DData::setYAxis(int row,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationId,data,58+((m_xAxis.size() - row)*2),2);
}

void Table3DData::setCell(int row, int column,unsigned short newval)
{
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	qDebug() << "Attempting to save data at:" << row << column;
	emit saveSingleData(m_locationId,data,100+((column)*2)+(row * (m_xAxis.size()*2)),2);
}
QByteArray Table3DData::data()
{
	return QByteArray();
}

QList<unsigned short> Table3DData::yAxis()
{
	return m_yAxis;
}

QList<unsigned short> Table3DData::xAxis()
{
	return m_xAxis;
}

QList<QList<unsigned short> > Table3DData::values()
{
	return m_values;
}

int Table3DData::columns()
{
	return m_xAxis.size();
}

int Table3DData::rows()
{
	return m_yAxis.size();
}
