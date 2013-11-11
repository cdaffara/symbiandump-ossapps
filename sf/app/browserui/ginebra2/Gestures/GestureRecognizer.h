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
*
*/

/*
 W A R N I N G
 -------------
 THIS IS A TEMPORARY GESTURE CODE. WOULD BE REPLACED WHEN BROWSER HAS ITS OWN GESTURE FRAMEWORK
 */

#ifndef GestureRecognizer_h
#define GestureRecognizer_h

#include "GestureEvent.h"

class QGraphicsSceneMouseEvent;

namespace GVA {

class GestureListener;
class GestureRecognizerPrivate;

class GestureRecognizer : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal dragInertia READ dragInertia WRITE setDragInertia)
    Q_PROPERTY(int directionErrorMargin READ directionErrorMargin WRITE setDirectionErrorMargin)
    Q_PROPERTY(qreal axisLockThreshold READ axisLockThreshold WRITE setAxisLockThreshold)
    Q_PROPERTY(qreal maximumVelocity READ maximumVelocity WRITE setMaximumVelocity)
    Q_PROPERTY(qreal minimumVelocity READ minimumVelocity WRITE setMinimumVelocity)
    Q_PROPERTY(qreal panningThreshold READ panningThreshold WRITE setPanningThreshold)
    Q_PROPERTY(qreal fastVelocityFactor READ fastVelocityFactor WRITE setFastVelocityFactor)
    Q_PROPERTY(qreal scrollsPerSecond READ scrollsPerSecond WRITE setScrollsPerSecond)

public:
    GestureRecognizer(GestureListener*);
    ~GestureRecognizer();

    bool mouseEventFilter(QGraphicsSceneMouseEvent *);

    qreal dragInertia() const;
    void setDragInertia(qreal inertia);

    int directionErrorMargin() const;
    void setDirectionErrorMargin(int errorMargin);

    qreal axisLockThreshold() const;
    void setAxisLockThreshold(qreal threshold);

    qreal maximumVelocity() const;
    void setMaximumVelocity(qreal v);

    qreal minimumVelocity() const;
    void setMinimumVelocity(qreal v);

    int panningThreshold() const;
    void setPanningThreshold(int threshold);

    qreal fastVelocityFactor() const;
    void setFastVelocityFactor(qreal f);

    int scrollsPerSecond() const;
    void setScrollsPerSecond(int sps);

    QScopedPointer<GestureRecognizerPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(GestureRecognizer)
};

} //namespace GVA
#endif //GestureRecognizer_h
