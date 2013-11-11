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

#ifndef OBJECTCHARM_H_
#define OBJECTCHARM_H_

#include <QtGui>

namespace GVA {

// -------------------------------

/*!
 * Base class for object "charms".  Charms set an event filter on a particular
 * object and perform some action in response to the events sent to the object.
 * ObjectCharms automatically destroy themselves when the target object is
 * destroyed.
 */
class ObjectCharm : public QObject {
    Q_OBJECT
  public:
    ObjectCharm(QObject *object);

  private slots:
    virtual void onObjectDestroyed();

  protected:
    QObject *m_object;  // not owned
};

// -------------------------------

/*!
 * This class draws a circle in response to mouse click events on the given object.  Intended
 * for testing purposes only.
 */
class TouchCircleCharm : public ObjectCharm {
    Q_OBJECT
  public:
    TouchCircleCharm(QObject *object, QGraphicsItem *parent = 0);
    ~TouchCircleCharm();

  private slots:
    void onTimer();

  private:
    bool eventFilter(QObject *object, QEvent *event);

    class QGraphicsEllipseItem * m_item;
    class QTimer * m_timer;
};

}

#endif /* OBJECTCHARM_H_ */
