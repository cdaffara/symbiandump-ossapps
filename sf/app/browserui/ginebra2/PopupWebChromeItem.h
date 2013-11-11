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

#ifndef __GINEBRA_POPUPWEBCHROMEITEM_H__
#define __GINEBRA_POPUPWEBCHROMEITEM_H__

#include "WebChromeItem.h"

namespace GVA {

/*!
 * This class displays a modal dialog-like snippet.
 *
 * To use PopupWebChromeItem:
 *
 * 1. Add data-GinebraItemType="popup" to the DIV element defining your
 *    snippet in chrome.html
 *
 * 2. Add data-GinebraPopupModal="false" to the DIV to prevent the system
 *    from dimming the other items on the screen while your popup is displayed.
 *
 * 3. Connect to the externalMouseEvent signal AFTER chromeLoadComplete
 *    has been emitted:
 *
 *      window.chrome.chromeComplete.connect(chromeLoadComplete);
 *
 *      function chromeLoadComplete() {
 *        window.snippets.ContextMenuId.externalMouseEvent.connect(
 *          function(type, name, description) {
 *            if (name == "QGraphicsSceneMouseReleaseEvent") {
 *              window.snippets.ContextMenuId.hide();
 *            }
 *          }
 *        );
 *      }
 */

class PopupWebChromeItem : public WebChromeItem
{
    Q_OBJECT

public:
    PopupWebChromeItem(
            ChromeWidget * chrome,
            const QWebElement & element,
            QGraphicsItem * parent = 0,
            bool modal = true);

    virtual ~PopupWebChromeItem();

    virtual void init(WebChromeSnippet * snippet);

signals:
    void externalMouseEvent(
            QEvent * ev,
            const QString & name,
            const QString & description);

private:
    void checkForExternalEvent(QObject * object, QEvent * event);
    void emitExternalEvent(QEvent * event);
    bool event(QEvent * e);

    bool m_modal;
    class ExternalEventCharm *m_externalEventCharm;
};

} // end of namespace GVA

#endif // __GINEBRA_POPUPWEBCHROMEITEM_H__
