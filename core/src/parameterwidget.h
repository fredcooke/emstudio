#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include "configblock.h"
#include "dialogfield.h"
class ParameterWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ParameterWidget(QWidget *parent = 0);
	void addParam(QString title,DialogField field,ConfigBlock block);
	QList<unsigned short> getLocationIdList();
	void updateValue(unsigned short locationid,QByteArray block);
	void setValueRamFlash(unsigned short locationid,bool isRam,bool isFlash);
private:
	double calcAxis(int val,QList<QPair<QString,double> > metadata);
	int backConvertAxis(double val,QList<QPair<QString,double> > metadata);
	QList<QPair<DialogField,ConfigBlock> > m_fieldConfigList;
	QMap<QString,QLineEdit*> m_nameToLineEditMap;
	QScrollArea *scrollArea;
	QVBoxLayout *mainLayout;
	QWidget *scrollWidget;
private slots:
	void saveButtonClicked();
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
public slots:

};

#endif // PARAMETERWIDGET_H
