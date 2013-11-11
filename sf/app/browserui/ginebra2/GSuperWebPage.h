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

#ifndef GSUPERWEBPAGE_H
#define GSUPERWEBPAGE_H

#include "GWebPage.h"

namespace GVA {

// ------------------------------
/*! \ingroup JavascriptAPI
* \brief A content view that has full access to the Javascript APIs.
*
* Example code to load an HTML file into a super page:
* \code
* window.views.WebView.createSuperPage("BookmarkView", true);
* window.views.WebView.BookmarkView.load("./chrome/BookmarkView.html");
* \endcode
*/
class GSuperWebPage : public GWebPage {
    Q_OBJECT
public:
    GSuperWebPage(WebPageWrapper *page, ChromeWidget *chromeWidget);
    void onShown() { emit shown(); }
    void onHidden() { emit hidden(); }

public slots:
    void load(const QString &url);

signals:
    /*!
    * Triggered by the javascript code within the superpage when it wants a context menu to be displayed by
    * the chrome's javascript.
    *
    * The normal chain of events is:
    * \li User executes a long-press (or RMB click).
    * \li Qt sends QContextMenuEvent from QWebView.
    * \li GWebContentViewWidget::contextMenuEvent is called, which passes the event to the superpage
    * (if one is currently displayed).
    * \li The superpage emits \c contextEvent().
    * \li The context event handler in the superpage's javascript determines what was clicked on
    * and emits \c showContextMenu() from the superpage.
    * \li Javascript \c showContextMenu signal handler in the chrome is called which then displays the context menu.
    */
    void showContextMenu(QVariant obj);

    /*!
     * Triggered when the page is shown.
     */
    void shown();

    /*!
     * Triggered when the page is hidden.
     */
    void hidden();

private slots:
    void onJavaScriptWindowObjectCleared();

private:
    ChromeWidget *m_chromeWidget;  // not owned
};

}   // GVA namespace

#endif // GSUPERWEBPAGE_H
