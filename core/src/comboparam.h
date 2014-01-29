#ifndef COMBOPARAM_H
#define COMBOPARAM_H

#include <QWidget>
#include "configdata.h"

namespace Ui {
class ComboParam;
}

class ComboParam : public QWidget
{
	Q_OBJECT
	
public:
	explicit ComboParam(QWidget *parent = 0);
	~ComboParam();
	void setName(QString name);
	void setConfig(ConfigData *data);
	void saveValue();
private slots:
	void dataUpdate();
	void currentIndexChanged(int index);
private:
	Ui::ComboParam *ui;
	ConfigData *m_data;
};

#endif // COMBOPARAM_H
