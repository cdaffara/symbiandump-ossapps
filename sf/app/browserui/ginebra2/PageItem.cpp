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

#include "PageItem.h"
#include "GWebTouchNavigation.h"
#include "Utilities.h"
#include "GSuperWebPage.h"
#include "ChromeWidget.h"

#include <QGraphicsWebView>
#include <QTimer>

namespace GVA {

/// Helper web view class that disables the default context menu.
class PageItemWebView : public QGraphicsWebView {
public:
    PageItemWebView(QGraphicsItem * parent = 0)
        : QGraphicsWebView(parent)
    {}
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
        // Do nothing.
        event->accept();
    }
};

// ---------------------------------

PageItem::PageItem(ChromeSnippet * snippet, ChromeWidget* chrome)
  : NativeChromeItem(snippet, chrome->layout()),
    m_webView(0),
    m_touchNavigation(0),
    m_superPage(0),
    m_page(0),
    m_chrome(chrome),
    m_cleanupTimer(0),
    m_isInstantiated(false)
{
    setVisible(false);
}

PageItem::~PageItem() {
    if (!m_isInstantiated) return;
    cleanUp();
}

void PageItem::instantiate() {
    if (m_isInstantiated) {
        m_cleanupTimer->stop(); // make sure cleanup not scheduled
        return;
    }

    // Create the web page.
    m_page = new WebPageWrapper(this, "Page snippet javascript error");
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    // Change page item base brush to transparent -
    // this allows HTML to set the background color.
    QPalette pagePalette = palette();
    pagePalette.setBrush(QPalette::Base, Qt::transparent);
    m_page->setPalette(pagePalette);

    // Use a super page here because they know how to inject our javascript objects
    // into their javascript engines.
    m_superPage = new GSuperWebPage(m_page, m_chrome);

    // Create the web view.
    m_webView = new PageItemWebView(this);
    m_webView->setPage(m_page);

    m_touchNavigation = new GWebTouchNavigation(m_page, m_webView);

    // create and setup timer to delay cleanup
    m_cleanupTimer = new QTimer();
    m_cleanupTimer->setSingleShot(true);
    m_cleanupTimer->setInterval(0);
    connect(m_cleanupTimer, SIGNAL(timeout()), this, SLOT(cleanUp()));

    if (!m_url.isEmpty())
        m_webView->setUrl(m_url);
    else if (!m_html.isEmpty()) {
        m_webView->setHtml(m_html);
        m_html = QString::null;
    }

    m_isInstantiated = true;
}

void PageItem::cleanUpOnTimer() {
    if (!m_isInstantiated) return;

    m_cleanupTimer->start();
}

void PageItem::cleanUp() {   // slot
    delete m_cleanupTimer;
    m_cleanupTimer = 0;
    delete m_touchNavigation;
    m_touchNavigation = 0;
    delete m_webView;
    m_webView = 0;
    delete m_superPage;
    m_superPage = 0;
    m_isInstantiated = false;
}

void PageItem::resizeEvent(::QGraphicsSceneResizeEvent *event) {
    setWebViewSize(event->newSize());
}

void PageItem::setSize(const QSizeF &size) {
    resize(size);
    setWebViewSize(size);
}

void PageItem::setWebViewSize(const QSizeF &size) {
    if (m_webView) {
        m_webView->resize(size);
        m_webView->page()->setViewportSize(size.toSize());
    }
}

void PageItem::showEvent(QShowEvent *event) {
    instantiate();
    NativeChromeItem::showEvent(event);
}

void PageItem::setUrl(const QString &value) {
    m_url = value;
    if (m_webView)
        m_webView->setUrl(value);
}

QString PageItem::url() const {
    if (m_webView) {
        return m_webView->url().toString();
    }
    return QString();
}

void PageItem::setHtml(const QString &value) {
    if (m_webView)
        m_webView->setHtml(value);
    else
        m_html = value;
}

QString PageItem::html() const {
    if (m_webView) {
        QWebFrame *frame = m_webView->page()->mainFrame();
        if (frame)
            return frame->toHtml();
    }
    return QString();
}

//void PageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget) {
//    Q_UNUSED(opt)
//    Q_UNUSED(widget)
//
//    // Paint the background.
//    painter->fillRect(QRectF(0,0, geometry().width(), geometry().height()), Qt::blue);
//}

QVariant PageItem::evaluateJavaScript(const QString &expression) {
    if (m_webView) {
        QWebFrame *frame = m_webView->page()->mainFrame();
        if (frame)
            return frame->evaluateJavaScript(expression);
    }
    return QVariant();
}

} // GVA namespace
