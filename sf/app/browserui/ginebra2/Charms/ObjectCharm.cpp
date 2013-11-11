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

#include "ObjectCharm.h"
#include "Utilities.h"

#include <QtGui>
#include <QDebug>
#include <QApplication>
#include <QTimer>


namespace GVA {

// ----------------------------
// ObjectCharm
// ----------------------------

ObjectCharm::ObjectCharm(QObject *object)
  : m_object(object)
{
    m_object->installEventFilter(this);
    connect(m_object, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
}

void ObjectCharm::onObjectDestroyed() {
    deleteLater();
}

// ----------------------------
// TouchCircleCharm
// ----------------------------

TouchCircleCharm::TouchCircleCharm(QObject *object, QGraphicsItem *parent)
    : ObjectCharm(object),
      m_timer(new QTimer())
{
    m_item = new QGraphicsEllipseItem(QRect(0,0,20,20), parent);
    m_item->hide();
    m_item->setPen(QPen(QColor(Qt::black)));
    m_item->setBrush(QBrush(QColor(200, 0, 0)));
    m_item->setOpacity(0.7);
    m_item->setZValue(100);  // make sure it isn't obscured

    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

TouchCircleCharm::~TouchCircleCharm() {
    delete m_item;
    delete m_timer;
}

bool TouchCircleCharm::eventFilter(QObject *object, QEvent *event) {
    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            // Show the item.
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);
            m_item->setPos(me->scenePos().x() - m_item->rect().width()/2,
                           me->scenePos().y() - m_item->rect().height()/2);

            // Change the color to indicate click.
            m_item->setBrush(QBrush(QColor(0, 200, 0)));

            m_item->show();
            m_timer->stop();
            break;
        }
        case QEvent::GraphicsSceneMouseMove: {
            if(m_item->isVisible()) {
                // Move it to new position.
                QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);
                m_item->setPos(me->scenePos().x() - m_item->rect().width()/2,
                               me->scenePos().y() - m_item->rect().height()/2);

                // Change the color to indicate drag.
                m_item->setBrush(QBrush(QColor(255, 255, 0)));
            }
            break;
        }
        case QEvent::GraphicsSceneMouseDoubleClick: {
            if(m_item->isVisible()) {
                // Change the color to indicate double click.
                m_item->setBrush(QBrush(QColor(0, 0, 200)));

                m_timer->stop();
            }
            break;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            if(m_item->isVisible()) {
                // Change the color to indicate release.
                m_item->setBrush(QBrush(QColor(200, 0, 0)));

                // Start timer to hide it.
                m_timer->start(2000);
            }
            break;
        }
//        case QEvent::GraphicsSceneContextMenu: {
//            if(m_item->isVisible()) {
//                // Change the color to indicate context event.
//                m_item->setBrush(QBrush(QColor(0, 200, 200)));
//            }
//            break;
//        }
        default:
            break;
    }
    return object->eventFilter(object, event);
}

void TouchCircleCharm::onTimer() {   // slot
    m_item->hide();
}

}   // namespace GVA
