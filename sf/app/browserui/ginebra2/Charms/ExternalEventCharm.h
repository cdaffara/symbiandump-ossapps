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

#ifndef EXTERNALEVENTCHAR_H
#define EXTERNALEVENTCHAR_H

#include "ObjectCharm.h"

namespace GVA {

/*!
 * This class emits a signal when the user clicks outside of the given graphics object.
 */
class ExternalEventCharm : public ObjectCharm {
    Q_OBJECT
  public:
    ExternalEventCharm(QGraphicsObject *object);

    static const QString s_mouseClick;

  signals:
    /*! This signal is emitted when the user clicks outside of the object.
     * \param type The Qt event type.
     * \param name The Qt event name (or "MouseClick" if click gesture is detected).
     * \param description The Qt event description.
     */
    void externalMouseEvent(
            QEvent * ev,
            const QString & name,
            const QString & description);
  private:
    void checkForExternalEvent(QObject * o, QEvent * e);
    void emitExternalEvent(QEvent * e);
    bool eventFilter(QObject *object, QEvent *event);
    QGraphicsScene *scene();

  private:
    QPointF m_pressPos;
    bool m_pressed;
};

}   // end GVA namepace

#endif // EXTERNALEVENTCHAR_H
