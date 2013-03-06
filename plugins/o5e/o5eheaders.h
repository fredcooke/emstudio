#ifndef O5EHEADERS_H
#define O5EHEADERS_H
#include <QString>


class scalarclass
{
public:
	QString type;
	QString unit;
	unsigned int offset;
	unsigned int size;
	bool signedval;
	float scale;
	float translate;
};




#endif // O5EHEADERS_H
