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

#ifndef ROUNDGAUGEITEM_H
#define ROUNDGAUGEITEM_H

#include <QPen>
#include <QPainter>
#include <QTimer>

#include "gaugeutil.h"
#include "abstractgaugeitem.h"

class RoundGaugeItem : public AbstractGaugeItem
{

public:
    explicit RoundGaugeItem(QWidget *parent = 0);

private:
    void init();
    void drawBackground();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *style , QWidget *w);

    static const int arcSize = 3600; //225 * 16;
    static const int arcStart = 2160; //135 * 16;

    int m_size;
    float m_halfsize;
    double m_range;
    int m_gap;

    int m_locationXY;
    int m_locationHighlightXY;
    int m_sizeXY;
    int m_sizeHighlightXY;
    QFont m_fontValue;
    QFont m_fontTitle;
    QFont m_fontUnit;
    QPen m_penArc;
    QPen m_penNeedle;

    /*
    int m_degreesLowDanger;
    int m_degreesLowWarning;
    int m_degreesHighDanger;
    int m_degreesHighWarning;
    int m_valueDeg;
    */

    QRectF m_arcLowDanger;
    QRectF m_arcLowWarning;
    QRectF m_arcHighDanger;
    QRectF m_arcHighWarning;
    QRectF m_arcValue;

signals:
    
public slots:

};

#endif // ROUNDGAUGEITEM_H
