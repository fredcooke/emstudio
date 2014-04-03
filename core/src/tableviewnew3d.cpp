#include "tableviewnew3d.h"
#include <QPainter>
#include <QMouseEvent>
#define ITEM_WIDTH 60
#define ITEM_HEIGHT 30
#define ROWS 19
#define COLUMNS 21
TableViewNew3D::TableViewNew3D(QWidget *parent) : QWidget(parent)
{
	multiSelect = false;
	for (int y=0;y<ROWS;y++)
	{
		yaxis.append(10 * y);
		QList<double> row;
		QList<int> highlightrow;
		highlightrow.append(0); //For the Y column
		for (int x=0;x<COLUMNS;x++)
		{
			xaxis.append(10 * x);
			row.append((COLUMNS * 10) - (10 * x));
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
void TableViewNew3D::paintEvent (QPaintEvent *evt)
{
	Q_UNUSED(evt)
	QPainter painter(this);
	painter.setPen(QColor::fromRgb(0,0,0));
	painter.drawRect(0,0,width()-1,height()-1);
	//item width = 40
	//item height = 20
	for (int y=0;y<ROWS;y++)
	{
		if (currentCell.x() == 0 && currentCell.y() == y)
		{
			painter.setPen(QColor::fromRgb(0,0,255));
		}
		painter.drawRect(0,y*ITEM_HEIGHT,ITEM_WIDTH-1,ITEM_HEIGHT-1);
		QString numval = QString::number(yaxis.at(y));
		int width = painter.fontMetrics().width(numval);
		painter.drawText((ITEM_WIDTH/2.0) - (width / 2.0),(y)*ITEM_HEIGHT + ((ITEM_HEIGHT/2.0)-2) + (painter.fontMetrics().height()/2.0),numval);
		painter.setPen(QColor::fromRgb(0,0,0));
	}
	for (int x=0;x<COLUMNS;x++)
	{
		if (currentCell.y() == ROWS && currentCell.x() == x+1)
		{
			painter.setPen(QColor::fromRgb(0,0,255));
		}
		painter.drawRect((x+1)*ITEM_WIDTH,ITEM_HEIGHT*ROWS,ITEM_WIDTH-1,ITEM_HEIGHT-1);
		QString numval = QString::number(xaxis.at(x));
		int width = painter.fontMetrics().width(numval);
		painter.drawText(((x+1)*ITEM_WIDTH) + ((ITEM_WIDTH/2.0) - (width / 2.0)),ITEM_HEIGHT*ROWS + ((ITEM_HEIGHT/2.0)-2) + (painter.fontMetrics().height()/2.0),numval);
		painter.setPen(QColor::fromRgb(0,0,0));
	}

	for (int y=0;y<ROWS;y++)
	{
		for (int x=0;x<COLUMNS;x++)
		{
			if (currentCell.y() == y && currentCell.x() == x+1)
			{
				painter.setPen(QColor::fromRgb(0,0,255));
			}
			painter.drawRect(ITEM_WIDTH*(x+1),ITEM_HEIGHT*y,ITEM_WIDTH-1,ITEM_HEIGHT-1);

			double val = values.at(y).at(x);
			double max = 255;
			if (val < max/4.0)
			{
				QColor bgcolor = QColor::fromRgb(0,(255*((val)/(max/4.0))),255);
				painter.fillRect(ITEM_WIDTH*(x+1),ITEM_HEIGHT*y,ITEM_WIDTH-1,ITEM_HEIGHT-1,bgcolor);
			}
			else if (val < ((max/4.0)*2))
			{
				QColor bgcolor = QColor::fromRgb(0,255,255-(255*((val-((max/4.0)))/(max/4.0))));
				painter.fillRect(ITEM_WIDTH*(x+1),ITEM_HEIGHT*y,ITEM_WIDTH-1,ITEM_HEIGHT-1,bgcolor);
			}
			else if (val < ((max/4.0)*3))
			{
				QColor bgcolor = QColor::fromRgb((255*((val-((max/4.0)*2))/(max/4.0))),255,0);
				painter.fillRect(ITEM_WIDTH*(x+1),ITEM_HEIGHT*y,ITEM_WIDTH-1,ITEM_HEIGHT-1,bgcolor);
			}
			else
			{
				QColor bgcolor = QColor::fromRgb(255,255-(255*((val-((max/4.0)*3))/(max/4.0))),0);
				painter.fillRect(ITEM_WIDTH*(x+1),ITEM_HEIGHT*y,ITEM_WIDTH-1,ITEM_HEIGHT-1,bgcolor);
			}

			QString numval = QString::number(values.at(y).at(x));
			int width = painter.fontMetrics().width(numval);
			painter.drawText((ITEM_WIDTH*(x+1)) + ((ITEM_WIDTH / 2.0) - (width / 2.0)),((ITEM_HEIGHT*y) + ((ITEM_HEIGHT/2.0)-2)) + (painter.fontMetrics().height()/2.0),numval);
			painter.setPen(QColor::fromRgb(0,0,0));
		}
	}
}
void TableViewNew3D::mouseMoveEvent(QMouseEvent *evt)
{
	m_x = evt->x() / ITEM_WIDTH;
	m_y = evt->y() / ITEM_HEIGHT;
	update();
}
void TableViewNew3D::mousePressEvent(QMouseEvent *evt)
{
	m_x = evt->x() / ITEM_WIDTH;
	m_y = evt->y() / ITEM_HEIGHT;
	currentCell.setX(m_x);
	currentCell.setY(m_y);
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
		if (currentCell.y() < ROWS)
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
		if (currentCell.x() < COLUMNS)
		{
			currentCell.setX(currentCell.x()+1);
			update();
			return;
		}
	}
	if (evt->key() == Qt::Key_Equal)
	{
		if (currentCell.x() == 0 && currentCell.y() != 0)
		{
			//Y Axis
			yaxis[currentCell.y()]++;
			update();
			return;
		}
		else if (currentCell.y() == ROWS)
		{
			xaxis[currentCell.x()-1]++;
			update();
			return;
			//X Axis
		}
		values[currentCell.y()][currentCell.x()-1]++;
		update();
		return;
	}
	if (evt->key() == Qt::Key_Minus)
	{
		if (currentCell.x() == 0 && currentCell.y() != 0)
		{
			//Y axis
			yaxis[currentCell.y()]--;
			update();
			return;
		}
		else if (currentCell.y() == ROWS)
		{
			//X Axis
			xaxis[currentCell.x()-1]--;
			update();
			return;
		}
		values[currentCell.y()][currentCell.x()-1]--;
		update();
		return;
	}
}
