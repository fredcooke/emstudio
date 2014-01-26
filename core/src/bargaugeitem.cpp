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

#include <math.h>
#include <QString>
#include <QDebug>

#include "bargaugeitem.h"

BarGaugeItem::BarGaugeItem(QWidget *parent)
{
    Q_UNUSED(parent);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    m_redrawBackground = true;
}

void BarGaugeItem::init() {
    m_size = this->width();
    m_barSize = m_size * 0.95f;
    m_range = m_high - m_low;
    m_gap = 2; // FIXME Fixed value for now...

    m_padding = (m_size - m_barSize) / 2;

    m_penBar = QPen(colorOk, (int) (m_size * 0.06f), Qt::SolidLine, Qt::FlatCap);
    m_penNeedle = QPen(colorOk, (int) (m_size * 0.04f), Qt::SolidLine, Qt::RoundCap);

    m_fontValue = QFont("helvetica", (int) (m_size * 0.12f), QFont::Bold);
    m_fontTitle = QFont("helvetica", (int) (m_size * 0.07f), QFont::Bold);
    m_fontUnit = QFont("helvetica", (int) (m_size * 0.05f), QFont::Bold);
}

void BarGaugeItem::drawBackground() {
    QPainter painter(m_background);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background fill
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height());

    // Bar
    int locationY = this->height() / 4;
    int barNormalStart = m_padding;
    int barNormalEnd = m_barSize + m_padding;

    // Paddings for warning zones
    int lengthLowDanger = 0;
    int lengthHighDanger = 0;

    // Danger zones
    m_penBar.setColor(colorDanger);
    painter.setPen(m_penBar);

    if (m_lowDanger) {
	lengthLowDanger = ((m_lowDanger - m_low) / m_range) * m_barSize;
        painter.drawLine(m_padding, locationY, lengthLowDanger + m_padding - m_gap, locationY);
        barNormalStart += lengthLowDanger;
    }

    if (m_highDanger) {
	lengthHighDanger = ((m_high - m_highDanger) / m_range) * m_barSize;
        painter.drawLine(m_padding + m_barSize - lengthHighDanger + m_gap, locationY, m_padding + m_barSize, locationY);
        barNormalEnd -= lengthHighDanger;
    }

    // Warning zones
    m_penBar.setColor(colorWarning);
    painter.setPen(m_penBar);

    if (m_lowWarning) {
	int lengthLowWarning = ((m_lowWarning - m_low) / m_range) * m_barSize;
        painter.drawLine(m_padding + lengthLowDanger, locationY, lengthLowWarning + m_padding - m_gap, locationY);
        barNormalStart += (lengthLowWarning - lengthLowDanger);
    }

    if (m_highWarning) {
	int lengthHighWarning = ((m_high - m_highWarning) / m_range) * m_barSize;
        painter.drawLine(m_padding + m_barSize - lengthHighWarning + m_gap, locationY, m_padding + m_barSize - lengthHighDanger, locationY);
        barNormalEnd -= (lengthHighWarning - lengthHighDanger);
    }

    // Normal zone
    m_penBar.setColor(colorOk);
    painter.setPen(m_penBar);
    painter.drawLine(barNormalStart, locationY, barNormalEnd, locationY);

    // Reset pen
    m_penBar.setColor(Qt::white);
    painter.setPen(m_penBar);

    // Title
    painter.setFont(m_fontTitle);
    int valueY = (int (this->height() * 0.65f));
    painter.drawText(m_padding, valueY, m_title);

    // Unit
    painter.setFont(m_fontUnit);
    QFontMetrics metrics(m_fontUnit);
    QRect bbox = metrics.boundingRect(m_unit);
    int valueX = m_size - bbox.width() - m_padding;
    valueY = (int (this->height() * 0.85f));
    painter.drawText(valueX, valueY, m_unit);

    painter.end();
}

void BarGaugeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *style , QWidget *w)
{
    Q_UNUSED(style);
    Q_UNUSED(w);

    if (m_redrawBackground) {
        m_redrawBackground = false;
        this->init();
        this->drawBackground();
    }

    painter->setRenderHint(QPainter::Antialiasing);

    // Background
    painter->drawImage(0, 0, *m_background);

    m_penNeedle.setColor(Qt::white);
    painter->setPen(m_penNeedle);

    // Needle
    if (isRangeSet()) {
        int valuePoint = ((m_value - m_low) / m_range) * m_barSize + m_padding;
        painter->drawLine(valuePoint, m_padding * 2, valuePoint, m_padding * 7);
    }

    // Value
    painter->setFont(m_fontValue);

    if (isHighlighted()) {
        m_penNeedle.setColor(getStatusColor());
        painter->setPen(m_penNeedle);
    }

    QString valueText;
    valueText = valueText.setNum(m_value, 'f', m_decimal);

    QFontMetrics metrics(m_fontValue);
    QRect bbox = metrics.boundingRect(valueText);

    int valueX = m_size - bbox.width() - m_padding;
    int valueY = (int (this->height() * 0.70f));

    painter->drawText(valueX, valueY, valueText);
}
