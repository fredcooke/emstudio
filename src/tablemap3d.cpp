#include "tablemap3d.h"
#include <QPainter>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
TableMap3D::TableMap3D(QWidget *parent) : QGLWidget(parent)
{
	m_tableData=0;
	setAutoFillBackground(false);
	m_mouseDown = false;
	m_xrot=-65;
	m_yrot=45;
	m_zrot=-3.0;
}
void TableMap3D::paintEvent(QPaintEvent *evt)
{
	QGLWidget::paintEvent(evt);
}
void TableMap3D::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	m_mouseDown = false;
	//qDebug() << "Mouse UP";
}

void TableMap3D::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	m_mouseDown = true;
}

void TableMap3D::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_mouseDown)
	{
		return;
	}
	update();
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton)
	{
		m_xrot += dy;
		m_yrot += dx;
	}
	else if (event->buttons() & Qt::RightButton)
	{
		m_xrot += dy;
		m_zrot += dx;
	}
	lastPos = event->pos();
}
void TableMap3D::initializeGL()
{
	makeCurrent();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	qglClearColor(QColor::fromRgb(0,0,0));
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glViewport((width() - qMin(width(), height())) / 2, (height() - qMin(width(), height())) / 2, qMin(width(), height()), qMin(width(), height()));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, +1, -1, 1, 1.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}
void TableMap3D::resizeGL(int w, int h)
{
	glViewport((w - qMin(w, h)) / 2, (h - qMin(w, h)) / 2, qMin(w, h), qMin(w, h));
	glMatrixMode(GL_MODELVIEW);
}

void TableMap3D::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0,0,-3); //Move back 3 units
	glRotatef(m_xrot, 1.0, 0.0, 0.0); //Rotate it up/down
	glRotatef(m_yrot, 0.0, 0.0, 1.0); //Rotate it left/right
	glTranslatef(-0.5,-0.5,-0.5); //Center the graph in view
	//float minx = 0;
	float maxx = 1;
	float miny = 0;
	float maxy = 1;
	if (m_tableData->xAxis().size() < m_tableData->yAxis().size())
	{
		maxx = maxy * ((float)m_tableData->yAxis().size()/(float)m_tableData->xAxis().size());
	}
	else
	{
		maxy = maxx * ((float)m_tableData->xAxis().size() / (float)m_tableData->yAxis().size());
	}
	Q_UNUSED(miny);
	glColor4f(1,1,1,1);


	//Draw 10 horizontal lines for VE values
	for (int i=0;i<=10;i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(maxx-((float)4)/((float)m_tableData->xAxis().size()),0,(float)i/10.0);
		glVertex3f(maxx-((float)4)/((float)m_tableData->xAxis().size()),maxy+((float)2)/((float)m_tableData->xAxis().size()),(float)i/10.0);
		glVertex3f(0,maxy+((float)2)/((float)m_tableData->xAxis().size()),(float)i/10.0);
		glEnd();
		this->renderText(maxy,-0.05,(float)i/100.0,QString::number((m_tableData->maxZAxis() / 75.0) / (i/10.0)));
	}

	//Longer X axis (RPM)
	for (int i=0;i<m_tableData->xAxis().size();i+=1)
	{
		glBegin(GL_LINES);
		float tmpf2=0;
		tmpf2 = (float)i/((float)m_tableData->xAxis().size());
		glVertex3f(maxx-((float)4)/((float)m_tableData->xAxis().size()),tmpf2 * (maxy / 1.0),0);
		glVertex3f(maxx-((float)4)/((float)m_tableData->xAxis().size()),tmpf2 * (maxy / 1.0),1);
		glEnd();
		this->renderText(0,tmpf2,-0.05,QString::number(m_tableData->xAxis()[i]));
	}

	//Shorter Y Axis (KPA)
	for (int i=0;i<m_tableData->yAxis().size();i+=1)
	{
		glBegin(GL_LINES);
		float tmpf1 = (float)i/((float)m_tableData->yAxis().size());
		glVertex3f(tmpf1 * (maxy / 1.0),maxy+((float)2)/((float)m_tableData->yAxis().size()-2.0),0);
		glVertex3f(tmpf1 * (maxy / 1.0),maxy+((float)2)/((float)m_tableData->yAxis().size()-2.0),1);
		glEnd();
		//this->renderText(tmpf1,0,-0.05,QString::number(m_tableData->yAxis()[i]));
	}


	//Line around the top of the graph
	glBegin(GL_LINE_STRIP);
	glVertex3f(0,1,1);
	glVertex3f(1,1,1);
	glVertex3f(1,0,1);
	glEnd();


	//Square outline around the bottom of the graph
	glBegin(GL_LINE_STRIP);
	glVertex3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glVertex3f(1,1,0);
	glVertex3f(0,1,0);
	glEnd();

	for(int x=0;x<m_tableData->xAxis().size()-1;x++)
	{
		glBegin(GL_QUADS);
		for(int y=0;y<m_tableData->yAxis().size()-1;y++)
		{
			double r=0;
			double g=0;
			double b=0;
			double val = m_tableData->values()[y][x];
			if (val < m_tableData->maxZAxis()/4)
			{
				r=0;
				g=(255*((val)/(m_tableData->maxZAxis()/4.0)));
				b=255;
			}
			else if (val < ((m_tableData->maxZAxis()/4)*2))
			{
				r=0;
				g=255;
				b=255-(255*((val-((m_tableData->maxZAxis()/4.0)))/(m_tableData->maxZAxis()/4.0)));
			}
			else if (val < ((m_tableData->maxZAxis()/4)*3))
			{
				r=(255*((val-((m_tableData->maxZAxis()/4.0)*2))/(m_tableData->maxZAxis()/4.0)));
				g=255;
				b=0;
			}
			else
			{
				r=255;
				g=255-(255*((val-((m_tableData->maxZAxis()/4.0)*3))/(m_tableData->maxZAxis()/4.0)));
				b=0;
			}
			r = r/255.0;
			g = g/255.0;
			b = b/255.0;

			float x0 = (float)x/((float)m_tableData->xAxis().size()-2.0);
			float y0 = (float)y/((float)m_tableData->yAxis().size()-2.0);
			float z0 = (float)m_tableData->values()[y][x] / 75.0;
			glColor4f(r,g,b,1);
			glVertex3f(x0,y0,z0);

			float x1 = ((float)x)/((float)m_tableData->xAxis().size()-2.0);
			float y1 = (float)y+1/((float)m_tableData->yAxis().size()-2.0);
			float z1 = (float)m_tableData->values()[y+1][x] / 75.0;
			glColor4f(r,g,b,1);
			glVertex3f(x1,y1,z1);

			float x2 = ((float)x+1.0)/((float)m_tableData->xAxis().size()-2.0);
			float y2 = ((float)y+1.0)/((float)m_tableData->yAxis().size()-2.0);
			float z2 = (float)m_tableData->values()[y+1][x+1] / 75.0;
			glColor4f(r,g,b,1);
			glVertex3f(x2,y2,z2);

			float x3 = (float)x+1/((float)m_tableData->xAxis().size()-2.0);
			float y3 = ((float)y)/((float)m_tableData->yAxis().size()-2.0);
			float z3 = (float)m_tableData->values()[y][x+1]/75.0;
			glColor4f(r,g,b,1);
			glVertex3f(x3,y3,z3);
		}
		glEnd();
	}
}
void TableMap3D::passData(Table3DData *tableData)
{
	m_tableData = tableData;
}
