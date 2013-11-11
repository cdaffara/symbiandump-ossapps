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

#include "PopupWebChromeItem.h"
#include "ChromeWidget.h"
#include "WebChromeSnippet.h"
#include "ExternalEventCharm.h"

namespace GVA {

PopupWebChromeItem::PopupWebChromeItem(
        ChromeWidget * chrome,
        const QWebElement & element,
        QGraphicsItem * parent,
        bool modal)
: WebChromeItem(chrome, element, parent),
  m_modal(modal),
  m_externalEventCharm(0)
{
}

PopupWebChromeItem::~PopupWebChromeItem()
{
    delete m_externalEventCharm;
}

void PopupWebChromeItem::init(WebChromeSnippet * snippet)
{
    WebChromeItem::init(snippet);
    m_externalEventCharm = new ExternalEventCharm(this);

    // Forward externalMouseEvent signals from context items.
    QObject::connect(
            m_externalEventCharm,
            SIGNAL(externalMouseEvent(QEvent *, const QString &, const QString &)),
            snippet,
            SIGNAL(externalMouseEvent(QEvent *, const QString &, const QString &)));
}

bool PopupWebChromeItem::event(QEvent * e)
{
    switch (e->type()) {
      case QEvent::Show:
        if(snippet() && m_modal) {
            chrome()->emitPopupShown(snippet()->objectName());
        }
        break;
      case QEvent::Hide:
        if(snippet() && m_modal) {
            chrome()->emitPopupHidden(snippet()->objectName());
        }
        break;
      default: break;
    }

    // Let the parent class handle the event.

    return WebChromeItem::event(e);
}

} // end of namespace GVA
