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

#include "roundgaugeitem.h"

RoundGaugeItem::RoundGaugeItem(QWidget *parent)
{
    Q_UNUSED(parent);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    m_redrawBackground = true;
}

void RoundGaugeItem::init() {
    m_size = this->width();
    m_halfsize = m_size / 2;
    m_gap = 25;

    m_range = m_high - m_low;

    m_locationXY = (int (m_size * 0.05f));
    m_locationHighlightXY = (m_size * 0.12f);
    m_sizeXY = (int (m_size * 0.9f));
    m_sizeHighlightXY = (m_size * 0.77f);

    m_penArc = QPen(colorOk, (int) (m_size * 0.05f), Qt::SolidLine, Qt::FlatCap);
    m_penNeedle = QPen(colorOk, (int) (m_size * 0.04f), Qt::SolidLine, Qt::RoundCap);

    m_fontValue = QFont("helvetica", (int) (m_size * 0.12f), QFont::Bold);
    m_fontTitle = QFont("helvetica", (int) (m_size * 0.07f), QFont::Bold);
    m_fontUnit = QFont("helvetica", (int) (m_size * 0.05f), QFont::Bold);
}

void RoundGaugeItem::drawBackground() {
    QPainter painter(m_background);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background fill
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height());

    // Arcs
    int degreesNormalStart = 0;
    int degreesNormalSize = arcSize;

    // Paddings for warning zones
    int degreesLowDanger = 0;
    int degreesHighDanger = 0;

    // Danger zones
    m_penArc.setColor(colorDanger);
    painter.setPen(m_penArc);

    if (m_lowDanger) {
	degreesLowDanger = (int ((m_lowDanger - m_low) * arcSize / m_range));
        painter.drawArc(m_locationXY, m_locationXY, m_sizeXY, m_sizeXY, (arcSize - degreesLowDanger + m_gap), (degreesLowDanger - m_gap));
        degreesNormalSize -= degreesLowDanger;
    }

    if (m_highDanger) {
	degreesHighDanger = (int ((m_high - m_highDanger - m_low) * arcSize / m_range));
        painter.drawArc(m_locationXY, m_locationXY, m_sizeXY, m_sizeXY, 0, degreesHighDanger - m_gap);
        degreesNormalSize -= degreesHighDanger;
        degreesNormalStart += degreesHighDanger;
    }

    // Warning zones
    m_penArc.setColor(colorWarning);
    painter.setPen(m_penArc);

    if (m_lowWarning) {
	int degreesLowWarning = (int ((m_lowWarning - m_low) * arcSize / m_range));
        painter.drawArc(m_locationXY, m_locationXY, m_sizeXY, m_sizeXY, (arcSize - degreesLowWarning + m_gap), (degreesLowWarning - degreesLowDanger - m_gap));
        degreesNormalSize -= (degreesLowWarning - degreesLowDanger);
    }

    if (m_highWarning) {
	int degreesHighWarning = (int ((m_high - m_highWarning - m_low) * arcSize / m_range));
        painter.drawArc(m_locationXY, m_locationXY, m_sizeXY, m_sizeXY, degreesHighDanger, (degreesHighWarning - degreesHighDanger - m_gap));
        degreesNormalSize -= (degreesHighWarning - degreesHighDanger);
        degreesNormalStart += (degreesHighWarning - degreesHighDanger);
    }

    // Normal zone
    m_penArc.setColor(colorOk);
    painter.setPen(m_penArc);
    painter.drawArc(m_locationXY, m_locationXY, m_sizeXY, m_sizeXY, degreesNormalStart, degreesNormalSize);

    // -------

    // Title
    m_penArc.setColor(Qt::white);
    painter.setPen(m_penArc);
    painter.setFont(m_fontTitle);
    QFontMetrics metrics2(m_fontTitle);
    QRect bbox = metrics2.boundingRect(m_title);

    int valueX = (int (this->width() * 0.90f - bbox.width()));
    int valueY = (int (this->height() * 0.90f));

    painter.drawText(valueX, valueY, m_title);

    // Unit
    painter.setFont(m_fontUnit);
    QFontMetrics metrics3(m_fontUnit);
    bbox = metrics3.boundingRect(m_unit);

    valueX = (int (this->width() / 2 - (bbox.width() / 2)));
    valueY = (int (this->height() * 0.35f));

    painter.drawText(valueX, valueY, m_unit);

    painter.end();
}

void RoundGaugeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *style , QWidget *w)
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

    // Highlight arc
    if (isHighlighted()) {
        m_penArc.setColor(getStatusColor());
        painter->setPen(m_penArc);
        painter->drawArc(m_locationHighlightXY, m_locationHighlightXY, m_sizeHighlightXY, m_sizeHighlightXY, 0, arcSize);
    }

    // Needle
    if (isRangeSet()) {
        m_penNeedle.setColor(Qt::white);
        painter->setPen(m_penNeedle);
        float valueAmount = (float) ((m_value - m_low) / m_range);

        int *needlePoints1 = GaugeUtil::getPointInArc((int) m_halfsize, (int) m_halfsize, arcSize, arcStart, (int) (m_size * 0.1f), valueAmount);
        int *needlePoints2 = GaugeUtil::getPointInArc((int) m_halfsize, (int) m_halfsize, arcSize, arcStart, (int) (m_size * 0.3f), valueAmount);

        painter->drawLine(needlePoints1[0], needlePoints1[1], needlePoints2[0], needlePoints2[1]);
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

    int valueX = (int (this->width() * 0.90f - (float) bbox.width()));
    int valueY = (int (this->height() * 0.80f));

    painter->drawText(valueX, valueY, valueText);
}
