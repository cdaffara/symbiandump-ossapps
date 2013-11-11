/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include <QPainter>

#include "perfmondatapopupwidget_p.h"

const qreal popupMargin = 5.;

PerfMonDataPopupWidget::PerfMonDataPopupWidget(QGraphicsItem *parent) :
    HbWidget(parent)
{
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    mFont = fontSpec.font();
    mFont.setPixelSize(12);
}

QStringList PerfMonDataPopupWidget::lines() const
{
    return mLines;
}

void PerfMonDataPopupWidget::setLines(const QStringList &lines)
{
    mLines = lines;
    updateGeometry();
}

QSizeF PerfMonDataPopupWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if (which == Qt::PreferredSize)
    {
        QFontMetricsF metrics(mFont);

        qreal width = 0;
        foreach (const QString &line, mLines) {
            width = qMax(width, metrics.width(line));
        }
        qreal height = metrics.height() * mLines.length();

        return QSizeF(width + 2 * popupMargin, height + 2 * popupMargin);
    }

    return HbWidget::sizeHint(which, constraint);
}

void PerfMonDataPopupWidget::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QFontMetricsF metrics(mFont);
    qreal lineHeight = metrics.height();
    qreal verticalPos = lineHeight + popupMargin;

    // draw background
    painter->setBrush(Qt::white);
    painter->drawRect(option->rect);

    painter->setFont(mFont);

    foreach (const QString &line, mLines) {
        painter->drawText(QPointF(popupMargin, verticalPos), line);
        verticalPos += lineHeight;
    }

}
