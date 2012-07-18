/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/
#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QString>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString port = "";
	if (argc > 1)
	{
		QString arg = QString(argv[1]);
		if (arg.contains("="))
		{
			if (arg.split("=")[0] == "--dev")
			{
				port = arg.split("=")[1];
			}
		}
	}
	MainWindow w;
	if (port != "")
	{
		w.setDevice(port);
	}
	w.connectToEms();
	w.show();
	return a.exec();
}
