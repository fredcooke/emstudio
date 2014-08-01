#include "tableviewnew3d.h"
#include <QPainter>
#include <QMouseEvent>

TableViewNew3D::TableViewNew3D(QWidget *parent) : QWidget(parent)
{
	m_itemHeight = 30;
	m_itemWidth = 60;
	multiSelect = false;
	m_rowCount = 16;
	m_columnCount = 16;
	setFocusPolicy(Qt::ClickFocus);
	m_traceEnabled = false;
	m_updateTimer = new QTimer(this);
	connect(m_updateTimer,SIGNAL(timeout()),this,SLOT(update()));
	m_updateTimer->start(100);

}
void TableViewNew3D::setTracingValue(double x,double y)
{
	m_traceX = x;
	m_traceY = y;
}

void TableViewNew3D::addHotkey(int key,Qt::KeyboardModifier modifier)
{
	m_hotkeyMap.append(QPair<int,Qt::KeyboardModifier>(key,modifier));
}

void TableViewNew3D::resizeEvent(QResizeEvent *evt)
{
	m_itemWidth = this->width() / (m_columnCount + 1);
	m_itemHeight = this->height() / (m_rowCount + 1);
	this->update();
}

void TableViewNew3D::setXAxis(int index,QString number)
{
	xaxis[index] = number;
}

void TableViewNew3D::setYAxis(int index,QString number)
{
	yaxis[index] = number;
}
QString TableViewNew3D::xAxis(int index)
{
	return xaxis[index];
}

QString TableViewNew3D::yAxis(int index)
{
	return yaxis[index];
}

void TableViewNew3D::clear()
{
	//Clear everything out
	yaxis.clear();
	m_highlightList.clear();
	values.clear();
	xaxis.clear();
}
void TableViewNew3D::rebuildTable()
{
	yaxis.clear();
	m_highlightList.clear();
	values.clear();
	xaxis.clear();
	for (int y=0;y<m_rowCount;y++)
	{
		yaxis.append("0");
		QList<QString> row;
		QList<int> highlightrow;
		highlightrow.append(0); //For the Y column
		for (int x=0;x<m_columnCount;x++)
		{
			xaxis.append("0");
			row.append("0");
			highlightrow.append(0);
		}
		m_highlightList.append(highlightrow);
		if (y == 0)
		{
			m_highlightList.append(highlightrow); //Double add it for the last row
		}
		highlightrow.clear();
		values.append(row);
	}

}

void TableViewNew3D::setRowCount(int count)
{
	if (count < m_rowCount)
	{
		//Reducing the number of rows
	}
	else if (count > m_rowCount)
	{

	}
	else
	{
		//Row count stays the same
		return;
	}
	//Assume row count is changing from 0 to whatever
	m_rowCount = count;
	rebuildTable();
}
void TableViewNew3D::setItem(int row,int column,QString text)
{
	if (row == -1)
	{
		//X Axis
		if (column == -1)
		{
			//Ignore
			return;
		}
		setXAxis(column,text);
	}
	else if (column == -1)
	{
		//Y Axis
		setYAxis(row,text);
	}
	else
	{
		values[row][column] = text;
	}
	update();
}
QString TableViewNew3D::item(int row,int column)
{
	if (row == -1)
	{
		//XAxis
		if (column == -1)
		{
			return "";
		}
		return xaxis[column];
	}
	else if (column == -1)
	{
		return yaxis[row];
	}
	else
	{
		return values[row][column];
	}
}
void TableViewNew3D::setColumnCount(int count)
{
	m_columnCount = count;
	rebuildTable();
}

void TableViewNew3D::paintEvent (QPaintEvent *evt)
{
	Q_UNUSED(evt)
	QPainter painter(this);
	painter.setPen(QColor::fromRgb(0,0,0));
	painter.drawRect(0,0,width()-1,height()-1);
	//item width = 40
	//item height = 20
	double m_currentTrace = 0;
	double drawTraceY = 0;
	bool foundy = false;

	double drawTraceX = 0;
	bool foundx = false;

	for (int y=0;y<m_rowCount;y++)
	{
		if (currentCell.x() == -1 && currentCell.y() == y)
		{
			painter.setPen(QColor::fromRgb(0,0,255));
		}
		painter.drawRect(0,y*m_itemHeight,m_itemWidth-1,m_itemHeight-1);
		QString numval = yaxis.at(y);
		int width = painter.fontMetrics().width(numval);
		painter.drawText((m_itemWidth/2.0) - (width / 2.0),(y)*m_itemHeight + ((m_itemHeight/2.0)-2) + (painter.fontMetrics().height()/2.0),numval);
		painter.setPen(QColor::fromRgb(0,0,0));
		if (m_traceEnabled)
		{
			m_currentTrace = yaxis.at(y).toDouble();
			if (m_traceY < m_currentTrace)
			{
				if (y == m_rowCount-1)
				{
					double prev = yaxis.at(y-1).toDouble();
					double curr = yaxis.at(y).toDouble();
					curr = curr - ((prev - curr) / 2.0);
					if (m_traceY < curr)
					{
						//No go, don't trace.
						continue;
					}
					else
					{

					}
				}
			}
			else if (!foundy)
			{
				double prev = 0;
				if (y == 0)
				{
					//Value is between the top and null values
					prev = yaxis.at(y).toDouble() - ((yaxis.at(y+1).toDouble() - yaxis.at(y).toDouble()) / 2.0);
				}
				else
				{
					prev = yaxis.at(y-1).toDouble();
				}
				//Between the current trace and the last one, we have our value

				double diff = prev - m_currentTrace;
				double percent =(prev - m_traceY) / diff;
				//Percent is a 0.0-1.0 of where the trace should lie,between i-1, and i;
				double currentY = (y)*m_itemHeight + ((m_itemHeight/2.0)-2) + (painter.fontMetrics().height()/2.0);
				double lastY = (y-1)*m_itemHeight + ((m_itemHeight/2.0)-2) + (painter.fontMetrics().height()/2.0);
				drawTraceY = (lastY + (percent * (currentY - lastY)));
				foundy = true;
			}
		}
	}
	if (!foundy)
	{
		//Value is between the bottom cell and the last value.
	}
	for (int x=0;x<m_columnCount;x++)
	{
		if (currentCell.y() == -1 && currentCell.x() == x)
		{
			painter.setPen(QColor::fromRgb(0,0,255));
		}
		painter.drawRect((x+1)*m_itemWidth,m_itemHeight*m_rowCount,m_itemWidth-1,m_itemHeight-1);
		QString numval = xaxis.at(x);
		int width = painter.fontMetrics().width(numval);
		painter.drawText(((x+1)*m_itemWidth) + ((m_itemWidth/2.0) - (width / 2.0)),m_itemHeight*m_rowCount + ((m_itemHeight/2.0)-2) + (painter.fontMetrics().height()/2.0),numval);
		painter.setPen(QColor::fromRgb(0,0,0));

		if (m_traceEnabled)
		{
			m_currentTrace = xaxis.at(x).toDouble();
			if (m_traceX > m_currentTrace)
			{
				if (x == m_columnCount-1)
				{
					double prev = xaxis.at(x-1).toDouble();
					double curr = xaxis.at(x).toDouble();
					curr = curr - ((prev - curr) / 2.0);
					if (m_traceX < curr)
					{
						//No go, don't trace.
						continue;
					}
					else
					{

					}
				}
			}
			else if (!foundx)
			{
				double prev = 0;
				if (x == 0)
				{
					//Value is between the top and null values
					prev = xaxis.at(x).toDouble() - ((xaxis.at(x+1).toDouble() - xaxis.at(x).toDouble()) / 2.0);
				}
				else
				{
					prev = xaxis.at(x-1).toDouble();
				}
				//Between the current trace and the last one, we have our value

				double diff = prev - m_currentTrace;
				double percent =(prev - m_traceX) / diff;
				//Percent is a 0.0-1.0 of where the trace should lie,between i-1, and i;
				double currentX = (x+1)*m_itemWidth+ ((m_itemWidth/2.0)-2);
				double lastX = (x)*m_itemWidth+ ((m_itemWidth/2.0)-2);
				drawTraceX = (lastX + (percent * (currentX - lastX)));
				foundx = true;
			}
		}

	}

	for (int y=0;y<m_rowCount;y++)
	{
		for (int x=0;x<m_columnCount;x++)
		{
			if (currentCell.y() == y && currentCell.x() == x)
			{
				painter.setPen(QColor::fromRgb(0,0,255));
			}
			painter.drawRect(m_itemWidth*(x+1),m_itemHeight*y,m_itemWidth-1,m_itemHeight-1);

			double val = values.at(y).at(x).toDouble();
			double max = 255;
			if (val < max/4.0)
			{
				QColor bgcolor = QColor::fromRgb(0,(255*((val)/(max/4.0))),255);
				painter.fillRect(m_itemWidth*(x+1),m_itemHeight*y,m_itemWidth-1,m_itemHeight-1,bgcolor);
			}
			else if (val < ((max/4.0)*2))
			{
				QColor bgcolor = QColor::fromRgb(0,255,255-(255*((val-((max/4.0)))/(max/4.0))));
				painter.fillRect(m_itemWidth*(x+1),m_itemHeight*y,m_itemWidth-1,m_itemHeight-1,bgcolor);
			}
			else if (val < ((max/4.0)*3))
			{
				QColor bgcolor = QColor::fromRgb((255*((val-((max/4.0)*2))/(max/4.0))),255,0);
				painter.fillRect(m_itemWidth*(x+1),m_itemHeight*y,m_itemWidth-1,m_itemHeight-1,bgcolor);
			}
			else
			{
				QColor bgcolor = QColor::fromRgb(255,255-(255*((val-((max/4.0)*3))/(max/4.0))),0);
				painter.fillRect(m_itemWidth*(x+1),m_itemHeight*y,m_itemWidth-1,m_itemHeight-1,bgcolor);
			}

			QString numval = values.at(y).at(x);
			int width = painter.fontMetrics().width(numval);
			painter.drawText((m_itemWidth*(x+1)) + ((m_itemWidth / 2.0) - (width / 2.0)),((m_itemHeight*y) + ((m_itemHeight/2.0)-2)) + (painter.fontMetrics().height()/2.0),numval);
			painter.setPen(QColor::fromRgb(0,0,0));
		}
	}
	if (foundy && foundx && m_traceEnabled)
	{
		QPen pen = painter.pen();
		pen.setWidth(5);
		pen.setColor(QColor::fromRgb(255,255,0));
		painter.setPen(pen);
		//painter.drawLine(0,drawTraceY,width(),drawTraceY);
		painter.drawEllipse(drawTraceX-2,drawTraceY-1,4,2);
		pen.setColor(QColor::fromRgb(0,0,0));
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawEllipse(drawTraceX-6,drawTraceY-4,12,8);
	}
}
void TableViewNew3D::mouseMoveEvent(QMouseEvent *evt)
{
	m_x = evt->x() / m_itemWidth;
	m_y = evt->y() / m_itemHeight;
	update();
}
void TableViewNew3D::mousePressEvent(QMouseEvent *evt)
{
	m_x = evt->x() / m_itemWidth;
	m_y = evt->y() / m_itemHeight;
	if (m_y == m_rowCount)
	{
		m_y = -1;
	}
	currentCell.setX(m_x-1);
	currentCell.setY(m_y);
	//if (currentCell)
	/*if (m_highlightList.size() > m_y)
	{
		if (m_highlightList.at(m_y).size() > m_x)
		{
			m_highlightList[m_y][m_x] = 1;
		}
	}*/

	update();
}

void TableViewNew3D::mouseReleaseEvent(QMouseEvent *evt)
{
	Q_UNUSED(evt)
}
void TableViewNew3D::keyPressEvent(QKeyEvent *evt)
{
	if (evt->key() == Qt::Key_Up)
	{
		if (currentCell.y() > 0)
		{
			/*if (evt->modifiers() & Qt::ShiftModifier)
			{
				if (!multiSelect)
				{
					//No selection yet.
					startSelectCell = currentCell;
					multiSelect = true;
				}
				for (int i=)
				//Selecting a rectangle
				//selection will be
				for (int i=0;i<m_highlightList.size();i++)
				{
					//Each row here, test if the first is highlighted. If so,
				}
			}
			else if (evt->modifiers()  & Qt::ControlModifier)
			{
				//Moving selection without clearing
			}
			else
			{
				//Deselect everything, and move cursor to new point
				for (int i=0;i<m_highlightList.size();i++)
				{
					for (int j=0;j<m_highlightList[i].size();j++)
					{
						m_highlightList[i][j] = 0;
					}

				}
			}*/
			currentCell.setY(currentCell.y()-1);
			update();
			return;
		}
	}
	if (evt->key() == Qt::Key_Down)
	{
		if (currentCell.y() < m_rowCount)
		{
			currentCell.setY(currentCell.y()+1);
			update();
			return;
		}
	}
	if (evt->key() == Qt::Key_Left)
	{
		if (currentCell.x() > 0)
		{
			currentCell.setX(currentCell.x()-1);
			update();
			return;
		}
	}
	if (evt->key() == Qt::Key_Right)
	{
		if (currentCell.x() < m_columnCount)
		{
			currentCell.setX(currentCell.x()+1);
			update();
			return;
		}
	}
	for (int i=0;i<m_hotkeyMap.size();i++)
	{
		if (m_hotkeyMap.at(i).first == evt->key())
		{
			if (m_hotkeyMap.at(i).second != Qt::NoModifier)
			{
				if (evt->modifiers() & m_hotkeyMap.at(i).second)
				{
					emit hotKeyPressed(evt->key(),m_hotkeyMap.at(i).second);
				}
			}
			else
			{
				emit hotKeyPressed(evt->key(),Qt::NoModifier);
			}
		}
	}
	if (evt->key() == Qt::Key_Equal)
	{
		if (currentCell.x() == 0 && currentCell.y() != 0)
		{
			//Y Axis
		//	yaxis[currentCell.y()]++;
			update();
			return;
		}
		else if (currentCell.y() == m_rowCount)
		{
		//	xaxis[currentCell.x()-1]++;
			update();
			return;
			//X Axis
		}
		//values[currentCell.y()][currentCell.x()-1]++;
		update();
		return;
	}
	if (evt->key() == Qt::Key_Minus)
	{
		if (currentCell.x() == 0 && currentCell.y() != 0)
		{
			//Y axis
		//	yaxis[currentCell.y()]--;
			update();
			return;
		}
		else if (currentCell.y() == m_rowCount)
		{
			//X Axis
		//	xaxis[currentCell.x()-1]--;
			update();
			return;
		}
		//values[currentCell.y()][currentCell.x()-1]--;
		update();
		return;
	}
}
