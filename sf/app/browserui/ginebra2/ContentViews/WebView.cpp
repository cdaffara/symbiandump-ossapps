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
#include "WebView.h"

#include "browserpagefactory.h"
#include "wrtbrowsercontainer.h"

#include <QWebFrame>
#include <QWebPage>

namespace GVA {

WebView::WebView()
    : QGraphicsWebView()
    , m_webPage(0)
{}

WebView::~WebView()
{}

QWebPage* WebView::page() const
{
    return m_webPage;
}

void WebView::setPage(QWebPage* page)
{
    if (m_webPage == page)
        return;

    if (m_webPage) {
        disconnect(m_webPage->mainFrame(), 0, this, 0);
        m_webPage->setView(0);
    }

    m_webPage = page;

    if (!m_webPage)
        m_webPage = createWebPage();

    QGraphicsWebView::setPage(m_webPage);

    emit titleChanged(title());
    emit urlChanged(url());
}

QWebPage* WebView::createWebPage()
{
    return reinterpret_cast<QWebPage*>(BrowserPageFactory::openBrowserPage());
}

}//namespace GVA
