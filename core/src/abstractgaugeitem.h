/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 * Author: Ari "MrOnion" Karhu (ari@baboonplanet.com)                               *
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

#ifndef ABSTRACTGAUGEITEM_H
#define ABSTRACTGAUGEITEM_H

#include <QPen>
#include <QDeclarativeItem>
#include <QPainter>
#include <QTimer>

enum GaugeStatus {
    STATUS_OK = 1,
    STATUS_WARNING = 2,
    STATUS_DANGER = 3
};

class AbstractGaugeItem : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit AbstractGaugeItem(QDeclarativeItem *parent = 0);
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QString unit READ getUnit WRITE setUnit)
    Q_PROPERTY(double low READ getLow WRITE setLow)
    Q_PROPERTY(double high READ getHigh WRITE setHigh)
    Q_PROPERTY(double lowDanger READ getLowDanger WRITE setLowDanger)
    Q_PROPERTY(double highDanger READ getHighDanger WRITE setHighDanger)
    Q_PROPERTY(double lowWarning READ getLowWarning WRITE setLowWarning)
    Q_PROPERTY(double highWarning READ getHighWarning WRITE setHighWarning)
    Q_PROPERTY(int decimal READ getDecimal WRITE setDecimal)
    Q_PROPERTY(double value READ getValue WRITE setValue)

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual void init() = 0;
    virtual void drawBackground() = 0;

    QImage *m_background;
    bool m_redrawBackground;
    QString m_title;
    QString m_unit;
    double m_low;
    double m_high;
    //boost::optional<double> m_lowDanger;
    //boost::optional<double> m_lowWarning;
    //boost::optional<double> m_highDanger;
    //boost::optional<double> m_highWarning;
    double m_lowDanger;
    double m_lowWarning;
    double m_highDanger;
    double m_highWarning;
    bool m_lowDangerSet;
    bool m_lowWarningSet;
    bool m_highDangerSet;
    bool m_highWarningSet;
    int m_decimal;
    double m_value;

    static const QColor colorOk;
    static const QColor colorWarning;
    static const QColor colorDanger;
    static const QColor colorValue;

    bool isRangeSet();
    int getStatus();
    bool isHighlighted();
    QColor getStatusColor();

private:
    bool m_isResized;

signals:
    void updated();

public slots:
    double getLow();
    void setLow(double low);
    double getHigh();
    void setHigh(double high);
    double getLowWarning();
    void setLowWarning(double lowWarning);
    double getLowDanger();
    void setLowDanger(double lowDanger);
    double getHighWarning();
    void setHighWarning(double highWarning);
    double getHighDanger();
    void setHighDanger(double highDanger);
    QString getTitle();
    void setTitle(QString title);
    QString getUnit();
    void setUnit(QString unit);
    int getDecimal();
    void setDecimal(int decimal);
    double getValue();
    void setValue(double value);
};

#endif // ABSTRACTGAUGEITEM_H
