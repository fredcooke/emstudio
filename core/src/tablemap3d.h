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

#ifndef TABLEMAP3D_H
#define TABLEMAP3D_H

#include <QWidget>
#include <QPaintEvent>
#include <table3ddata.h>
#include <QtOpenGL>
#include <QPoint>
class TableMap3D : public QGLWidget
{
	Q_OBJECT
public:
	explicit TableMap3D(QWidget *parent = 0);
	void passData(Table3DData *tableData);
protected:
	float m_xrot;
	float m_yrot;
	float m_zrot;
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *evt);
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	bool m_mouseDown;
	QPoint lastPos;
private:
	Table3DData *m_tableData;
signals:

public slots:
	
};

#endif // TABLEMAP3D_H
