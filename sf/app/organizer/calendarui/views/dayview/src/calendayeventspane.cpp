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
* Description:  Day view control of calendar
*
*/
// System includes
#include <QPainter>
#include <HbColorScheme>

// User includes
#include "calendayeventspane.h"
#include "calendaycommonheaders.h"

/*!
 \class CalenDayEventsPane
 \brief Events pane draws timelines in content view.
 */

/*!
 \brief Constructor
 
 \param parent The parent of widget
 */
CalenDayEventsPane::CalenDayEventsPane(HbWidget *parent) : HbWidget(parent),
    mDrawTopLine(false)
{
    // Necessary when widget implements own paint method
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    HbDeviceProfile deviceProfile;
    mUnitInPixels = deviceProfile.unitValue();
    mHourLineColor = HbColorScheme::color(KCalenHourLineColor);
    
    // Set custom dashed pen
    mCustomDashedPen.setWidth(KCalenHourLineThickness);
    QVector<qreal> dashes;
    dashes << KCalenHalfHourLineDashWidth * mUnitInPixels 
        << KCalenHalfHourLineDashWidth * mUnitInPixels;
    mCustomDashedPen.setDashPattern(dashes);
    mCustomDashedPen.setCapStyle(Qt::FlatCap);
    mCustomDashedPen.setColor(mHourLineColor);
}

/*!
 \brief Destructor
 */
CalenDayEventsPane::~CalenDayEventsPane()
{
}

/*!
 \brief Shows/hides a line at top of event pane.
 
 \param drawTopLine Flag to be set if top line should be drawn.
 */
void CalenDayEventsPane::setDrawTopLine(bool drawTopLine)
{
    mDrawTopLine = drawTopLine;
}

/*!
   \brief It return if top line is drawed.
 */
bool CalenDayEventsPane::isTopLineDrawed() const
{
    return mDrawTopLine;
}

/*!
 \brief Paints the item with given painter.
 
 \param painter
 \param option
 \param widget
 */
void CalenDayEventsPane::paint(QPainter *painter, 
    const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    
    QRectF drawArea = option->rect;
    
    const qreal hourLineThickness = KCalenHourLineThickness;

    painter->save();
    
    // Draw full hour line
    QPen linePen = QPen(mHourLineColor, hourLineThickness * mUnitInPixels);
    painter->setPen(linePen);
    QLineF fullHourLine(drawArea.bottomLeft(), drawArea.bottomRight());
    painter->drawLine(fullHourLine);
    
    // Draw extra line on top if needed
    if (mDrawTopLine) {
        fullHourLine = QLineF(drawArea.topLeft(), drawArea.topRight());
        painter->drawLine(fullHourLine);
    }

    // Draw half hour line
    painter->setPen(mCustomDashedPen);
    qreal halfHourYPos = drawArea.height() / 2;
    QLineF halfHourLine(drawArea.left(), halfHourYPos, drawArea.right(), halfHourYPos);
    painter->drawLine(halfHourLine);
    
    painter->restore();
}

