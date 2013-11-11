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

#include "PageSnippet.h"
#include "PageItem.h"
#include "Utilities.h"
#include "ExternalEventCharm.h"

#include <QGraphicsWebView>
#include <QVariant>

namespace GVA {

// ----------------------------
// Inlines
// ----------------------------

inline PageItem *PageSnippet::pageItem() {
    return static_cast<PageItem *>(widget());
}

inline PageItem const *PageSnippet::constPageItem() const {
    return static_cast<PageItem const *>(constWidget());
}

// ----------------------------
// PageSnippet
// ----------------------------

PageSnippet::PageSnippet(const QString & elementId, ChromeWidget * chrome,
                         QGraphicsWidget * widget, const QWebElement & element)
  : ChromeSnippet(elementId, chrome, widget, element),
    m_externalEventCharm(0)
{
}

PageSnippet::~PageSnippet() {
    delete m_externalEventCharm;
}

PageSnippet * PageSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    PageSnippet* that = new PageSnippet(elementId, chrome, 0, element);
    that->setChromeWidget( new PageItem( that, chrome ) );
    return that;
}

void PageSnippet::setChromeWidget(QGraphicsWidget * widget) {
    ChromeSnippet::setChromeWidget(widget);
    m_externalEventCharm = new ExternalEventCharm(widget);
    safe_connect(m_externalEventCharm, SIGNAL(externalMouseEvent(QEvent*, const QString &, const QString &)),
                 this, SIGNAL(externalMouseEvent(QEvent*, const QString &, const QString &)));
}

void PageSnippet::setVisible(bool visiblity, bool animate) {
    ChromeSnippet::setVisible(visiblity, animate);
}

QString PageSnippet::url() const {
    return constPageItem()->url();
}

void PageSnippet::setUrl(const QString &url) {
    pageItem()->setUrl(url);
}

void PageSnippet::setHtml(const QString &value) {
    pageItem()->setHtml(value);
}

QString PageSnippet::html() const {
    return constPageItem()->html();
}

bool PageSnippet::hasFocus() {
    return element().hasFocus();
}

void PageSnippet::setGeometry(int x, int y, int width, int height) {
    widget()->setGeometry(x, y, width, height);
}

void PageSnippet::setSize(int width, int height) {
    pageItem()->setSize(QSizeF(width, height));
}

QVariant PageSnippet::evaluateJavaScript(const QString &expression) {
    return pageItem()->evaluateJavaScript(expression);
}

void PageSnippet::instantiate() {
    pageItem()->instantiate();
}

void PageSnippet::cleanUp() {
    pageItem()->cleanUpOnTimer();
}

}
