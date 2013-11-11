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

#ifndef __KINETIC_HELPER_H__
#define __KINETIC_HELPER_H__

#include <QObject>
#include <QPoint>
#include <QTime>

class QTimer;

#define KINETIC_TIMEOUT  60
#define DECELERATION     100


class KineticScrollable
{
public:
    virtual void scrollTo(QPoint& pos) = 0;

    virtual QPoint getScrollPosition() = 0;

    virtual QPoint getInitialPosition() = 0;

    virtual QPointF getInitialSpeed() = 0;
};



class KineticHelper: public QObject
{
    Q_OBJECT

public:
    KineticHelper(KineticScrollable* scrollable, qreal decel = DECELERATION, int kineticTimeout = KINETIC_TIMEOUT);
    ~KineticHelper();

    void setDeceleration(qreal decel) { m_decel = decel; }
    void setKineticTimeout(int timeout) { m_kineticTimeout = timeout; }
    void stopScrolling();
    void startScrolling();
    bool isScrolling();
private slots:
    void kineticScroll();
private:
    KineticScrollable* m_scrollable;
    QPointF            m_initialSpeed;
    QPoint             m_initialPos;
    QTimer*            m_kineticTimer;
    int                m_kineticSteps;
    qreal              m_decel;
    int                m_kineticTimeout;
    QTime              m_actualTime;
    qreal              m_kineticScrollTime;
};

#endif //__KINETIC_HELPER_H__
