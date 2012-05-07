#ifndef DATAPACKETDECODER_H
#define DATAPACKETDECODER_H

#include <QObject>
#include <QMap>
#include "datafield.h"
class DataPacketDecoder : public QObject
{
    Q_OBJECT
public:
	DataPacketDecoder(QObject *parent = 0);
	void populateDataFields();
	QList<DataField> m_dataFieldList;
signals:
	void payloadDecoded(QMap<QString,double> data);
public slots:
	void decodePayload(QByteArray payload);
};

#endif // DATAPACKETDECODER_H
