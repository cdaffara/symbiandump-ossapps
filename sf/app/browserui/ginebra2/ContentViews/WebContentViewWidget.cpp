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

#include "WebContentViewWidget.h"

#include "ScrollableWebContentView.h"
#include "WebContentAnimationItem.h"
#include "webpagecontroller.h"
#include "WebView.h"
#include "wrtbrowsercontainer.h"
#include "ZoomMetaData.h"

#include <QGraphicsLinearLayout>
#include <QStyleOptionGraphicsItem>
#include <QWebFrame>

namespace GVA {

WebContentViewWidget::WebContentViewWidget(QObject* parent, QWebPage* page)
    : m_webViewportProxy(new WebContentAnimationItem())
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    setParent(parent);

    m_webViewport = new ScrollableWebContentView(m_webViewportProxy, this);

    m_webView = new WebView();

    if (page)
        setPage(page);

    m_webViewportProxy->setWebView(m_webView);
    updatePreferredContentSize();
    m_webViewportProxy->setPos(QPointF(0,0));

    //FIX ME : Should we have to delete layout??
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0.);
    layout->addItem(m_webViewport);

    connect(m_webViewportProxy
            , SIGNAL(updateZoomActions(bool,bool))
            , this
            , SIGNAL(updateZoomActions(bool,bool)));
    connect(m_webViewport
            , SIGNAL(contextEventObject(QWebHitTestResult*))
            , this
            , SIGNAL(contextEventObject(QWebHitTestResult*)));

    connect(m_webViewport
            , SIGNAL(viewScrolled(QPoint&, QPoint&))
            , this
            , SIGNAL(viewScrolled(QPoint&, QPoint&)));

    connect(m_webViewport
            , SIGNAL(mouseEvent(QEvent::Type))
            , this
            , SIGNAL(mouseEvent(QEvent::Type)));
}

WebContentViewWidget::~WebContentViewWidget()
{
    delete m_webView;
    delete m_webViewportProxy;
    delete m_webViewport;
}

void WebContentViewWidget::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    QGraphicsWidget::resizeEvent(event);

    setGeometry(QRectF(pos(), size()));
    setPreferredSize(size());
}

QWebPage* WebContentViewWidget::page()
{
    return m_webView->page();
}

void WebContentViewWidget::setPage(QWebPage* page)
{
    m_webView->setPage(page);

    connect(this->page()->mainFrame(), SIGNAL(initialLayoutCompleted()), m_webViewport, SLOT(reset()));
    connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), m_webViewport, SLOT(contentsSizeChanged(const QSize&)));
    connect(this->page()->mainFrame(), SIGNAL(loadFinished(bool)), m_webViewport, SLOT(pageLoadFinished(bool)));
}

QGraphicsWebView* WebContentViewWidget::webView()
{
    return m_webView;
}

ZoomMetaData WebContentViewWidget::currentPageInfo()
{
    return m_webViewport->currentPageInfo();
}

void WebContentViewWidget::setCurrentPageInfo(ZoomMetaData data)
{
    m_webViewport->setCurrentPageInfo(data);
}

ZoomMetaData WebContentViewWidget::defaultZoomData()
{
    return m_webViewport->defaultZoomData();
}

void WebContentViewWidget::setPageZoom(bool zoomIn)
{
    Q_ASSERT(m_webViewport);
    m_webViewport->zoomToScreenCenter(zoomIn);
}

void WebContentViewWidget::showPage(bool isSuperPage)
{
    if (!isSuperPage) {
        WRT::WrtBrowserContainer* wbc = WebPageController::getSingleton()->currentPage();
        setPage((QWebPage*)wbc);  // static_cast here gives compiler error
    } else { 
        //Its a super page
        m_webViewport->setSuperPage();
    }
}

void WebContentViewWidget::updatePreferredContentSize()
{
    m_webViewport->updatePreferredContentSize();
}

void WebContentViewWidget::setGesturesEnabled(bool value)
{
    m_webViewport->setGesturesEnabled(value);
}
    
bool WebContentViewWidget::gesturesEnabled()
{
    return m_webViewport->gesturesEnabled();
}

} // namespace GVA
