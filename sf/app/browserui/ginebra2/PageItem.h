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

#ifndef PAGEITEM_H_
#define PAGEITEM_H_

#include "NativeChromeItem.h"

#include <QVariant>

class QGraphicsWebView;

namespace GVA {

class GWebTouchNavigation;
class ChromeWidget;
class GSuperWebPage;
class WebPageWrapper;

/*!
 * This class provides a full QWebView in a single chrome item
 * \sa PageSnippet
 */
class PageItem: public NativeChromeItem {
    Q_OBJECT
  public:
    PageItem(ChromeSnippet * snippet=0, ChromeWidget* chrome=0);
    virtual ~PageItem();

    QString url() const;
    void setUrl(const QString &url);

    void setHtml(const QString &value);
    QString html() const;

    QVariant evaluateJavaScript(const QString &expression);
    void instantiate();
    void setSize(const QSizeF &size);

    /// Puts a call to \ref cleanup on the end of the event queue.  Similar to QObject::deleteLater.
    /// Could be problematic if an event is subsequently placed on the queue that expects this object
    /// to be fully instantiated.
    void cleanUpOnTimer();

  public slots:
    void cleanUp();

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    void setWebViewSize(const QSizeF &size);

  protected:
    QGraphicsWebView *m_webView;
    GWebTouchNavigation *m_touchNavigation;
    GSuperWebPage *m_superPage;
    WebPageWrapper *m_page;
    ChromeWidget* m_chrome;
    QTimer* m_cleanupTimer;
    QString m_url;
    bool m_isInstantiated;
    QString m_html;
};

}

#endif /* PAGEITEM_H_ */
