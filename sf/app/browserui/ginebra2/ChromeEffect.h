/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*/

#ifndef __GINEBRA_CHROMEEFFECT_H__
#define __GINEBRA_CHROMEEFFECT_H__

#include <QColor>
#include <QRectF>

class QPainter;

namespace GVA {

/*!
 * The ChromeEffect namespace is intended to provide settings and drawing methods that can be
 * used by various classes for common display requirements, so far, just disabled mode drawing is
 * supported but more can be added...
 */
namespace ChromeEffect {
    /*!
     * Paint a rectangle that will give the underlying item a "disabled" look -- ie. whitewashed
     * or darkened or whatever the UI spec is.
     */
    void paintDisabledRect(QPainter *painter, const QRectF &rect);

    static qreal disabledOpacity = 0.65;
    static QColor disabledColor = Qt::white;
}

}  // end namespace GVA

#endif  // __GINEBRA_CHROMEEFFECT_H__
