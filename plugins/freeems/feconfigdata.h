#ifndef FECONFIGDATA_H
#define FECONFIGDATA_H

#include "configdata.h"
#include <QPair>
#include <QString>
#include <QList>

class FEConfigData : public ConfigData
{
    Q_OBJECT
public:
    FEConfigData();
    FEConfigData(QString name,QString type, QString override,unsigned short locationid, unsigned short size,unsigned short elementsize,unsigned short offset, QList<QPair<QString,double> > calc);

    void setName(QString name) { m_name = name; }
    void setType(QString type) { m_type = type; }
    void setSizeOverride(QString override) { m_sizeOverride = override; }
    void setSizeOverrideMult(double mult) { m_sizeOverrideMult = mult; }
    void setLocationId(unsigned short locationid) { m_locationId = locationid; }
    void setSize(unsigned short size) { m_size = size; }
    void setElementSize(unsigned short size) { m_elementSize = size; }
    void setOffset(unsigned short offset) { m_offset = offset; }
    void setCalc(QList<QPair<QString,double> > calc) { m_calc = calc; }
    QString name() { return m_name; }
    QString type() { return m_type; }
    QString sizeOverride() { return m_sizeOverride; }
    double sizeOverrideMult() { return m_sizeOverrideMult; }
    unsigned short locationId() { return m_locationId; }
    unsigned short offset() { return m_offset; }
    unsigned short size() { return m_size; }
    unsigned short elementSize() { return m_elementSize; }
    QList<QPair<QString,double> > calc() { return m_calc; }
    int elements() { return m_size / m_elementSize; }
    QVariant value();
    void setData(QByteArray data);
    void setValue(QVariant value);
    double calcAxis(unsigned short val,QList<QPair<QString,double> > metadata);
    unsigned short reverseCalcAxis(double val,QList<QPair<QString,double> > metadata);
private:
    QVariant m_value;
    QString m_name;
    QString m_type;
    QString m_sizeOverride;
    double m_sizeOverrideMult;
    unsigned short m_locationId;
    unsigned short m_size;
    unsigned short m_elementSize;
    unsigned short m_offset;
    QList<QPair<QString,double> > m_calc;
signals:
    void update();
    void saveSingleDataToFlash(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);
    void saveSingleDataToRam(unsigned short locationid,unsigned short offset, unsigned short size,QByteArray data);

};

#endif // FECONFIGDATA_H
