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
void printHelp()
{
	qDebug() << "Help";
	qDebug() << "Available Commands:";
	qDebug() << "-h\t\t--help\t\tShow this help";
	qDebug() << "-d <dev>\t--dev <dev>\tOpen EMStudio, connecting to device <dev>";
	qDebug() << "-a <true/false>\t--autoconnect <true/false>\tEnable/Disable autoconnect. Default enabled";

}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString port = "";
	bool autoconnect = true;
	QList<QPair<QString,QString> > args = getArgs(argc,argv);
	for (int i=0;i<args.size();i++)
	{
		if (args[i].first == "--dev" || args[i].first == "-d")
		{
			port = args[i].second;
		}
		else if (args[i].first == "--help" || args[i].first == "-h")
		{
			printHelp();
			return 0;
		}
		else if (args[i].first == "--autoconnect" || args[i].first == "-a")
		{
			if (args[i].second.toLower() == "false")
			{
				autoconnect = false;
			}
		}
		else
		{
			qDebug() << "Unknown command" << args[i].first;
			printHelp();
			return 0;
		}
	}

	MainWindow w;
	if (port != "")
	{
		w.setDevice(port);
	}
	if (autoconnect)
	{
		w.connectToEms();
	}
	w.show();
	return a.exec();
}
