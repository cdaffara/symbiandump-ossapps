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

#include "WebChromeItem.h"
#include "ChromeWidget.h"
#include "ChromeRenderer.h"
#include "ChromeDOM.h"
#include "WebChromeSnippet.h"
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif
#include <QSizePolicy>
#include <QWebHitTestResult>
#include <QWebFrame>
#include <QGraphicsView>
#include <QDebug>

namespace GVA
{
#ifndef NO_QSTM_GESTURE
using namespace qstmGesture;
#endif
WebChromeItem::WebChromeItem(ChromeWidget *chrome, const QWebElement & element, QGraphicsItem* parent) :
    ChromeItem(NULL, parent)
    , m_chrome(chrome)
    , m_element(element)
    , m_painting(false)
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    //Adjust the element size to match the element rectangle
    updateSizes();
#ifndef NO_QSTM_GESTURE
    grabGesture(QStm_Gesture::assignedType());
#endif
    //Use QGraphicsScene cached rendering NB: This might degrade rendering quality for some animation transforms
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

WebChromeItem::~WebChromeItem()
{

}

void WebChromeItem::init(WebChromeSnippet * snippet)
{
    //TODO: revisit this, don't use owner area
    //setCachedHandlers(chrome()->dom()->getCachedHandlers(snippet->elementId(), ownerArea()));
    m_snippet = snippet;

    //When chrome is resized owner areas for snippets may change
    //NB: Maybe this needs to be done on chromeRepainted too?

    QObject::connect(renderer(), SIGNAL(chromeResized()), snippet, SLOT(updateOwnerArea()));

    QObject::connect(this, SIGNAL(contextMenu(QGraphicsSceneContextMenuEvent *)), snippet,
        SLOT(onContextMenuEvent(QGraphicsSceneContextMenuEvent *)));
}

QGraphicsScene * WebChromeItem::scene()
{
  return m_chrome->layout()->scene();
}

ChromeRenderer * WebChromeItem::renderer()
{
    return m_chrome->renderer();
}

//Adjust size to match the element rectangle
void WebChromeItem::updateSizes()
{
    //Set preferred size so item will resize as part of anchor layout
    setPreferredSize(m_element.geometry().width(), m_element.geometry().height());
    //Also resize in case item is not part of anchor layout
    resize(preferredSize());
}

// NB: For now we only handle onclick (actually mouseUp). Fix this
// NB: Modify for multiple cached handlers: mouse click, long press
// and support handlers not executed in js engine.

void WebChromeItem::cachedHandlerEvent(QGraphicsSceneMouseEvent * ev)
{
    for (int i = 0; i < m_handlers.size(); i++) {
        const CachedHandler & handler = m_handlers.at(i);
        if (handler.rect().contains(ev->pos())) {
            //m_chrome->evalWithEngineContext(handler.script());
            handler.invoke();
            return;
        }
    }
}

void WebChromeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
{
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    m_painting = true;
    m_element.render(painter);
    m_painting = false;
    ChromeItem::paint(painter, opt, widget);
}

void WebChromeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * ev)
{
    ev->setAccepted(true);
    //Signal context menu event
    emit contextMenu(ev);
}

void WebChromeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * ev)
{
    forwardMouseEvent(QEvent::MouseButtonDblClick, ev);
}

void WebChromeItem::grabFocus()
{
    //setFocusProxy(m_chrome->renderer());
    setFocus();
}

void WebChromeItem::mousePressEvent(QGraphicsSceneMouseEvent * ev)
{
    //On mouse press, first invoke any cached handlers.
    cachedHandlerEvent(ev);
    //Then do normal mouse press handling
    setFocus();
    // setFocusProxy(m_chrome->renderer());
    forwardMouseEvent(QEvent::MouseButtonPress, ev);
}

void WebChromeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * ev)
{
    forwardMouseEvent(QEvent::MouseButtonRelease, ev);
#if defined __SYMBIAN32__
    // FIXME Remove this, it will be fixed Qt 4.6.3 ?
    /*   
     if (ev->button() == Qt::LeftButton) {
     QRectF elementRectangle = m_element.geometry();
     QPoint p = QPoint(int(elementRectangle.x()), int(elementRectangle.y())) + ev->pos().toPoint();
     QWebFrame* frame = m_chrome->renderer()->page()->mainFrame();
     QWebHitTestResult htr = frame->hitTestContent(p);
     if (htr.isContentEditable()) {
     QEvent vkbEvent(QEvent::RequestSoftwareInputPanel);
     QList<QGraphicsView*> views = m_chrome->renderer()->scene()->views();
     QWidget* view = qobject_cast<QWidget*>(views.value(0));
     if (view)
     QApplication::sendEvent(view, &vkbEvent);
     }
     }
     */
#endif
}

void WebChromeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * ev)
{
    forwardMouseEvent(QEvent::MouseMove, ev);
}

void WebChromeItem::forwardMouseEvent(QEvent::Type type, QGraphicsSceneMouseEvent *ev)
{

    emit mouseEvent(type);
    QRectF elementRectangle = m_element.geometry();
    QMouseEvent shiftedEv(type, QPoint(int(elementRectangle.x()), int(elementRectangle.y()))
        + ev->pos().toPoint(), ev->button(), ev->buttons(), ev->modifiers());
    //qDebug() << "elementRectangle: " << elementRectangle << "ev->pos(): " << ev->pos() <<
    //      "shiftedEv.pos(): " << shiftedEv.pos();
    QApplication::sendEvent(m_chrome->renderer()->page(), &shiftedEv);
}

void WebChromeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    // Translate to a mouse move event.
    /*
     QRectF elementRectangle = m_element.geometry(); 
     QMouseEvent shiftedEv( QEvent::MouseMove, QPoint(int(elementRectangle.x()), int(elementRectangle.y()))+event->pos().toPoint(),
     Qt::NoButton, Qt::NoButton, Qt::NoModifier);
     QApplication::sendEvent(m_chrome->renderer(), &shiftedEv); */
}
/*
 void WebChromeItem::keyPressEvent ( QKeyEvent * event ) {
 QApplication::sendEvent(m_chrome->renderer(), event);
 }

 void WebChromeItem::keyReleaseEvent ( QKeyEvent * event ) {
 QApplication::sendEvent(m_chrome->renderer(), event);
 }
 */

bool WebChromeItem::event(QEvent* event)
{
#ifndef NO_QSTM_GESTURE
    if (event->type() == QEvent::Gesture) {
        QStm_Gesture* gesture = getQStmGesture(event);
        if (gesture) {
            QStm_GestureType gtype = gesture->getGestureStmType();
            if (gtype == QStmTouchGestureType || gtype == QStmReleaseGestureType) {
                gesture->sendMouseEvents();
                return true;
            }
        }
    }
#endif
    return QGraphicsWidget::event(event);
}

} // endof namespace GVA
