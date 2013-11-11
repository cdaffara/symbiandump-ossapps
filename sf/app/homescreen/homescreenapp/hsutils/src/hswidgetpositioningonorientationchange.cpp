/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGlobal>
#include "hswidgetpositioningonorientationchange.h"



/*!
    \class HsWidgetPositioningOnOrientationChange
    \ingroup group_hsutils
    \brief Defines widget positioning on orientation change.

    Widget positioning on orientation change sets positions for
    a set of home screen widgets from landscape to portrait
    orientation and vice versa.
*/

/*!
    Sets the positioning \a instance as the current one.
    Deletes the existing instance if present.
*/
void HsWidgetPositioningOnOrientationChange::setInstance(
    HsWidgetPositioningOnOrientationChange *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}

/*!
    Returns the current positioning instance.
*/
HsWidgetPositioningOnOrientationChange *HsWidgetPositioningOnOrientationChange::instance()
{
    return mInstance;
}

/*!
    Stores the current positioning instance.
*/
HsWidgetPositioningOnOrientationChange *HsWidgetPositioningOnOrientationChange::mInstance = 0;

/*!
    \class HsSimpleWidgetPositioningOnOrientationChange
    \brief Bounds center points of input geometries to
           the given output rectangle.
*/

/*!
    \class HsAdvancedWidgetPositioningOnOrientationChange
    \brief More advanced widget positioning algorithm.

    Calculates new center points of
    \a fromGeometries when moving from \a fromRect to \a toRect.
    Screen is divided to two pieces:
    \verbatim
    ------
    | A  |
    |    |
    ------
    | B  |
    |    |
    ------
    \endverbatim
    and after conversion widgets from A are moved to A' and widgets from B to B':
    \verbatim
    |-----|-----|
    |A'   | B'  |
    |-----|-----|
    \endverbatim
    and vice versa.
*/
QList<QRectF> HsAdvancedWidgetPositioningOnOrientationChange::convert(
    const QRectF &fromRect,
    const QList<QRectF> &fromGeometries,
    const QRectF &toRect)
{
    QList<QRectF> toGeometries;

    // Portrait -> Landscape
    if (fromRect.width() < fromRect.height()) {
        foreach (QRectF g, fromGeometries) {
            
            QRectF tg(g.topLeft() - fromRect.topLeft(), g.size());
            
            qreal x = tg.center().x() / fromRect.width();
            qreal y = tg.center().y() / (fromRect.height() / 2);

            if (y < 1) {
                x *= toRect.width() / 2;
                y *= toRect.height();
            } else {
                x *= toRect.width() / 2;
                x += toRect.width() / 2;
                y -= 1;
                y *= toRect.height();
            }
            
            x = qBound(g.width() / 2, x, toRect.width() - g.width() / 2);
            y = qBound(g.height() / 2, y, toRect.height() - g.height() / 2);

            g.moveCenter(QPointF(x, y) + toRect.topLeft());

            toGeometries.append(g);
        }
    } else { // Landscape -> Portrait
        foreach (QRectF g, fromGeometries) {
            
            QRectF tg(g.topLeft() - fromRect.topLeft(), g.size());
            
            qreal x = tg.center().x() / (fromRect.width() / 2);
            qreal y = tg.center().y() / fromRect.height();

            if (x < 1) {
                x *= toRect.width();
                y *= toRect.height() / 2;
            } else {
                x -= 1;
                x += toRect.width();
                y *= toRect.height() / 2;
                y += toRect.height() / 2;
            }
            
            x = qBound(g.width() / 2, x, toRect.width() - g.width() / 2);
            y = qBound(g.height() / 2, y, toRect.height() - g.height() / 2);

            g.moveCenter(QPointF(x, y) + toRect.topLeft());

            toGeometries.append(g);
        }
    }

    return toGeometries;
}
