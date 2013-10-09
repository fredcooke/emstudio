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

#include <cassert>
#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QString>
//#include "QsLog.h"

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

	//Init the logger
#ifdef QLOG_DEBUG()
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::DebugLevel);
#ifdef Q_OS_WIN
	QString appDataDir = QString(getenv("USERPROFILE")).replace("\\","/");
#else
	QString appDataDir = getenv("HOME");
#endif
    QDir appDir(appDataDir);
	if (appDir.exists())
	{
		if (!appDir.cd("EMStudio"))
		{
			appDir.mkdir("EMStudio");
		}
	}
	const QString sLogPath(QDir(appDataDir + "/EMStudio").filePath("log.txt"));

	QsLogging::DestinationPtr fileDestination(
	   QsLogging::DestinationFactory::MakeFileDestination(sLogPath, true, 0, 5) );
	QsLogging::DestinationPtr debugDestination(
	   QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination);
	logger.addDestination(fileDestination);
#endif

	QString port = "";
	bool autoconnect = true;
	QString plugin = "";
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
		else if (args[i].first == "--plugin" || args[i].first == "-p")
		{
			plugin = args[i].second;
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
	if (plugin == "")
	{
		if (QFile::exists("plugins/libfreeemsplugin.so"))
		{
			plugin = "plugins/libfreeemsplugin.so";
		}
		else if (QFile::exists("/usr/share/emstudio/plugins/libfreeemsplugin.so"))
		{
			plugin = "/usr/share/emstudio/plugins/libfreeemsplugin.so";
        }
        else if (QFile::exists("plugins/freeemsplugin.lib"))
        {
            plugin = "plugins/freeemsplugin.lib";
        }
        else if (QFile::exists("plugins/libfreeemsplugin.a"))
        {
            plugin = "plugins/libfreeemsplugin.a";
        }
        else if (QFile::exists("plugins/freeemsplugin.dll"))
        {
            plugin = "plugins/freeemsplugin.dll";
        }
	}
	w.setPlugin(plugin);
	if (autoconnect)
	{
		w.connectToEms();
	}
	w.show();
	return a.exec();
}
