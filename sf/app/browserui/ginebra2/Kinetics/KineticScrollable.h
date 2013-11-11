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

#ifndef KineticScrollable_h
#define KineticScrollable_h

#include <QSize>
#include <QPoint>

namespace GVA {

class KineticScrollable {

public:
    enum State {
        Inactive,
        Pushing,
        AutoScrolling
    };

    virtual QSize viewportSize() const = 0;
    virtual QPoint maximumScrollPosition() const = 0;
    virtual QPoint scrollPosition() const = 0;
    virtual void setScrollPosition(const QPoint& pos, const QPoint& overshootDelta) = 0;
    virtual void stateChanged(KineticScrollable::State oldState, KineticScrollable::State newState) = 0;
};

} //namespace GVA

#endif //KineticScrollable_h
