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

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <QWidget>
#include "configblock.h"
#include "ui_configview.h"

class ConfigView : public QWidget
{
	Q_OBJECT
	
public:
	explicit ConfigView(QWidget *parent = 0);
	~ConfigView();
	void passConfig(QList<ConfigBlock> config,QByteArray data);
	double calcAxis(unsigned short val,QList<QPair<QString,double> > metadata);
	unsigned short backConvertAxis(double val,QList<QPair<QString,double> > metadata);
private:
	Ui::ConfigView ui;
	QList<ConfigBlock> m_configList;
	QByteArray m_rawData;
signals:
	void saveData(unsigned short locationid,QByteArray data);
private slots:
	void treeItemChanged(QTreeWidgetItem *item,int column);
	void treeItemDoubleClicked(QTreeWidgetItem *item,int column);
	void treeCurrentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *previous);
};

#endif // CONFIGVIEW_H
