#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>
class DataField
{
public:
	DataField();
	DataField(QString shortname,QString description,int offset,int size,float div,float min=0,float max=0);
	float getValue(QByteArray *payload);
	QString description() { return m_description; }
private:
	QString m_name;
	QString m_description;
	int m_offset;
	int m_size;
	float m_div;
	float m_min;
	float m_max;
};

#endif // DATAFIELD_H
