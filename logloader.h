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
