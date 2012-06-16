#include "tableview2d.h"
#include <QMessageBox>
#include <QDebug>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
TableView2D::TableView2D(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	//ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setRowCount(2);
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	//ui.tableWidget->setColumnWidth(0,100);
	connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(ui.loadPushButton,SIGNAL(clicked()),this,SLOT(loadClicked()));
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCurrentCellChanged(int,int,int,int)));
	/*#include <qwt_plot.h>
#include <qwt_plot_curve.h>

QwtPlot *myPlot = new QwtPlot("Two Curves", parent);

// add curves
QwtPlotCurve *curve1 = new QwtPlotCurve("Curve 1");
QwtPlotCurve *curve2 = new QwtPlotCurve("Curve 2");

// copy the data into the curves
curve1->setData(...);
curve2->setData(...);

curve1->attach(myPlot);
curve2->attach(myPlot);

// finally, refresh the plot
myPlot->replot();*/
	QPalette pal = ui.plot->palette();
	pal.setColor(QPalette::Background,QColor::fromRgb(0,0,0));
	ui.plot->setPalette(pal);
	curve = new QwtPlotCurve("Test");
	curve->attach(ui.plot);
	curve->setPen(QPen(QColor::fromRgb(255,0,0),3));
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->setPen(QPen(QColor::fromRgb(100,100,100)));
	grid->attach(ui.plot);

	//curve->setData()
	//QwtSeriesData<QwtIntervalSample> series;


}
void TableView2D::tableCurrentCellChanged(int currentrow,int currentcolumn,int prevrow,int prevcolumn)
{
	if (currentrow == -1 || currentcolumn == -1)
	{
		return;
	}
	currentvalue = ui.tableWidget->item(currentrow,currentcolumn)->text().toInt();
}

void TableView2D::loadClicked()
{
	//emit reloadTableData(m_locationId);
}

void TableView2D::tableCellChanged(int row,int column)
{
	if (row == -1 || column == -1)
	{
		return;
	}
	if (row >= ui.tableWidget->rowCount() || column >= ui.tableWidget->columnCount())
	{
		return;
	}
	bool ok = false;
	if (ui.tableWidget->item(row,column)->text().toInt(&ok) == currentvalue)
	{
		return;
	}
	if (ui.tableWidget->item(row,column)->text().toInt(&ok) > 65535)
	{
		QMessageBox::information(0,"Error",QString("Value entered too large! Value range 0-65535. Entered value:") + ui.tableWidget->item(row,column)->text());
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	else if (ok == false)
	{
		QMessageBox::information(0,"Error","Value entered is not a number!");
		ui.tableWidget->item(row,column)->setText(QString::number(currentvalue));
		return;
	}
	if (samples.size() <= column)
	{
		return;
	}
	unsigned short newval = ui.tableWidget->item(row,column)->text().toInt();
	if (row == 0)
	{
		samples.replace(column,QPointF(ui.tableWidget->item(row,column)->text().toInt(),samples.at(column).y()));
		curve->setSamples(samples);
		ui.plot->replot();
	}
	else if (row == 1)
	{
		samples.replace(column,QPointF(samples.at(column).x(),ui.tableWidget->item(row,column)->text().toInt()));
		curve->setSamples(samples);
		ui.plot->replot();
	}
	//New value has been accepted. Let's write it.
	//void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
	//Data is 64
	//offset = column + (row * 32), size == 2
	QByteArray data;
	data.append((char)((newval >> 8) & 0xFF));
	data.append((char)(newval & 0xFF));
	emit saveSingleData(m_locationid,data,(column*2)+(row * 32),2);
}
void TableView2D::resizeEvent(QResizeEvent *evt)
{
	for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		ui.tableWidget->setColumnWidth(i,(ui.tableWidget->width() / ui.tableWidget->columnCount())-1);
	}
}

void TableView2D::passData(unsigned short locationid,QByteArray data,int physicallocation)
{
	if (data.size() != 64)
	{
		qDebug() << "Passed a data pack to a 2d table that was of size" << data.size() << "should be 64!!!";
		return;
	}
	qDebug() << "TableView2D::passData" << "0x" + QString::number(locationid,16).toUpper();
	samples.clear();
	m_locationid = locationid;
	m_physicalid = physicallocation;
	//connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	disconnect(ui.tableWidget,SIGNAL(cellChanged(int,int)));
	ui.tableWidget->clear();
	ui.tableWidget->setColumnCount(0);
	ui.tableWidget->setRowCount(2);
	//QwtSeriesData<QPointF> *series = new QwtSeriesData<QPointF>();
	//QwtIntervalSeriesData *series = new QwtIntervalSeriesData();

	//QwtArraySeriesData<QwtIntervalSample> series;
	for (int i=0;i<data.size()/2;i+=2)
	{
		unsigned short x = (((unsigned char)data[i]) << 8) + ((unsigned char)data[i+1]);
		unsigned short y = (((unsigned char)data[(data.size()/2)+ i]) << 8) + ((unsigned char)data[(data.size()/2) + i+1]);
		/*ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
		ui.tableWidget->setVerticalHeaderItem(ui.tableWidget->rowCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(y)));*/
		ui.tableWidget->setColumnCount(ui.tableWidget->columnCount()+1);
		ui.tableWidget->setColumnWidth(ui.tableWidget->columnCount()-1,ui.tableWidget->width() / (data.size()/4));
		ui.tableWidget->setItem(0,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(x)));
		ui.tableWidget->setItem(1,ui.tableWidget->columnCount()-1,new QTableWidgetItem(QString::number(y)));

		/*QwtIntervalSample sample;
		QwtInterval interval;
		interval.setMinValue(x);
		interval.setMaxValue(nextx);
		sample.interval = interval;
		sample.value = y;*/
		//vector.append(sample);
		samples.append(QPointF(x,y));
	}
	//series->setSamples(vector);
	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableCellChanged(int,int)));
	curve->setSamples(samples);
	ui.plot->replot();

}

TableView2D::~TableView2D()
{
}
void TableView2D::saveClicked()
{
	emit saveToFlash(m_locationid);
	/*QByteArray data;
	bool ok = false;
	QByteArray first;
	QByteArray second;
	for (int i=0;i<ui.tableWidget->columnCount();i++)
	{
		unsigned short loc = ui.tableWidget->item(0,i)->text().toInt(&ok,10);
		first.append((char)((loc >> 8) & 0xFF));
		first.append((char)(loc & 0xFF));
		unsigned short loc2 = ui.tableWidget->item(1,i)->text().toInt(&ok,10);
		second.append((char)((loc2 >> 8) & 0xFF));
		second.append((char)(loc2 & 0xFF));
	}
	data.append(first);
	data.append(second);
	emit saveData(m_locationid,data,m_physicalid);*/
}
