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

QList<QPair<QString,QString> > getArgs(int argc, char **argv)
{
	bool nextisarg = false;
	QString current = "";
	QString currentarg;
	QList<QPair<QString,QString> > retval;
	for (int i=1;i<argc;i++)
	{
		QString arg = QString(argv[i]);
		if (arg.startsWith("-") || arg.startsWith("--"))
		{
			if (nextisarg)
			{
				//qDebug() << "Param with no arg" << current << currentarg;
				retval.append(QPair<QString,QString>(current,currentarg.trimmed()));
				currentarg = "";
			}
			nextisarg = true;
			current = arg;
		}
		else
		{
			if (nextisarg)
			{
				//qDebug() << "Param:" << current << "Arg:" << arg;
				currentarg += arg + " ";
			}
			else
			{
				//Invalid
				qDebug() << "Invalid arg";
			}
		}
	}
	if (nextisarg)
	{
		//qDebug() << "Param with no arg" << current << currentarg;
		retval.append(QPair<QString,QString>(current,currentarg.trimmed()));
	}
	return retval;
}
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString port = "";
	QList<QPair<QString,QString> > args = getArgs(argc,argv);
	for (int i=0;i<args.size();i++)
	{
		if (args[i].first == "--dev" || args[i].first == "-d")
		{
			port = args[i].second;
		}
		else if (args[i].first == "--help" || args[i].first == "-h")
		{
			qDebug() << "Help";
			qDebug() << "Available Commands:";
			qDebug() << "-d <dev>\t--dev <dev>\tOpen EMStudio, connecting to device <dev>";
			qDebug() << "-h\t\t--help\t\tShow this help";
			return 0;
		}
		else
		{
			qDebug() << "Unknown command" << args[i].first;
			qDebug() << "Help";
			qDebug() << "Available Commands:";
			qDebug() << "-d <dev>\t--dev <dev>\tOpen EMStudio, connecting to device<dev>";
			qDebug() << "-h\t\t--help\t\tShow this help";
			return 0;
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
