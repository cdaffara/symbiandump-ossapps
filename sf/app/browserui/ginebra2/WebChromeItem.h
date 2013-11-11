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


#ifndef __GINEBRA_WEBCHROMEITEM_H__
#define __GINEBRA_WEBCHROMEITEM_H__

#include <QtGui>
#include <QWebElement>
#include "CachedHandler.h"
#include "ChromeItem.h"

namespace GVA {

  class ChromeWidget;
  class ChromeRenderer;
  class WebChromeSnippet;

  class WebChromeItem : public ChromeItem
  {
    Q_OBJECT
  public:
    WebChromeItem(ChromeWidget* chrome, const QWebElement & element, QGraphicsItem* parent=0);
    virtual ~WebChromeItem();
    virtual void init(WebChromeSnippet * snippet);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    QRectF elementRect() {return QRectF(m_element.geometry());}
    void updateSizes();
    void setCachedHandlers(QList<CachedHandler> handlers) {m_handlers = handlers;}
    QGraphicsScene * scene();
    ChromeRenderer * renderer();
    void grabFocus();
    bool event(QEvent* event);
    ChromeWidget * chrome() { return m_chrome; }
    inline bool isPainting() { return m_painting; }
  public slots:
    QWebElement element() { return m_element; }
    //inline void setCacheMode(bool mode) {m_cacheMode = mode;};
  signals:
    void contextMenu(QGraphicsSceneContextMenuEvent * ev);
  protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * ev);
    void mousePressEvent(QGraphicsSceneMouseEvent * ev);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * ev);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent  *ev);
    void mouseMoveEvent(QGraphicsSceneMouseEvent  *ev);
    //void keyPressEvent ( QKeyEvent * event );
    //void keyReleaseEvent ( QKeyEvent * event );
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void forwardMouseEvent(QEvent::Type type, QGraphicsSceneMouseEvent *ev);

    //virtual bool event(QEvent * ev);
  private:
    void cachedHandlerEvent(QGraphicsSceneMouseEvent * ev);
    //Owner area obsolete, use element rectangle
    //QRectF m_ownerArea;
    QPixmap * m_pageBits;
    ChromeWidget* m_chrome;
    QWebElement m_element;
    //ChromeRenderer * m_renderer;
    QList<CachedHandler> m_handlers;
    //bool m_cacheMode;
    bool m_painting;
  };

} // end of namespace GVA

#endif // __GINEBRA_REDCHROMESNIPPET_H__
