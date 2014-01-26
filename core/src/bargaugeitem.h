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

#ifndef BARGAUGEITEM_H
#define BARGAUGEITEM_H

#include <QPen>
#include <QPainter>
#include <QTimer>

#include "gaugeutil.h"
#include "abstractgaugeitem.h"

class BarGaugeItem : public AbstractGaugeItem
{
public:
    explicit BarGaugeItem(QWidget *parent = 0);

private:
    void init();
    void drawBackground();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *style , QWidget *w);

    int m_size;
    int m_barSize;
    int m_padding;
    double m_range;
    int m_gap;

    QPen m_penBar;
    QPen m_penNeedle;
    QFont m_fontValue;
    QFont m_fontTitle;
    QFont m_fontUnit;

};

#endif // BARGAUGEITEM_H
