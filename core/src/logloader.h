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
#ifndef LOGLOADER_H
#define LOGLOADER_H

#include <QThread>
#include "datafield.h"
class LogLoader : public QThread
{
	Q_OBJECT
public:
	LogLoader(QObject *parent = 0);
	void loadFile(QString filename);
private:
	QString m_filename;
	QList<DataField> *m_dataFieldList;

protected:
	void run();
signals:
	void parseBuffer(QByteArray buffer);
	void payloadReceived(QByteArray header,QByteArray payload);
	void logProgress(qlonglong current,qlonglong total);
	void endOfLog();
public slots:

};

#endif // LOGLOADER_H
