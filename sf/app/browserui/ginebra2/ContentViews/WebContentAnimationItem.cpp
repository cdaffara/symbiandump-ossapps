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

#include "WebContentAnimationItem.h"

#include <QGraphicsWebView>
#include <QtGlobal>
#include <QWebElement>
#include <QWebFrame>

static const int MinDoubleClickZoomTargetWidth = 100;     //Target block width for applying double tap zoom
static const int ZoomCommitDuration = 50;                 //Timeout before commiting zoom
static const qreal ZoomableContentMinWidth = 300.;

namespace GVA {

WebContentAnimationItem::WebContentAnimationItem(QGraphicsWidget* parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
    , m_zoomCommitTimer(this)
    , m_viewportMetaData(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    setFiltersChildEvents(true);

    connect(&m_zoomCommitTimer, SIGNAL(timeout()), this, SLOT(commitZoom()));
    m_zoomCommitTimer.setSingleShot(true);
}

WebContentAnimationItem::~WebContentAnimationItem()
{}

void WebContentAnimationItem::setWebView(QGraphicsWebView* webView)
{
    Q_ASSERT(m_webView);
    m_webView = webView;

    //Enabling resize to contents avoids scrollbars in mainframe
    m_webView->setResizesToContents(true);
    m_webView->setParentItem(this);
    m_webView->setAttribute(Qt::WA_OpaquePaintEvent, true);
}

void WebContentAnimationItem::updatePreferredContentSize(const QSize& size)
{
    // FIXME: we have bug in QtWebKit API when tileCacheEnabled is true.
    // this causes viewport not to reset between the page loads.
    // Thus, we need to update viewport manually until we have fix for this.

    m_webView->page()->setPreferredContentsSize(size);
    resize(contentsSize());
}

QSize WebContentAnimationItem::contentsSize() const
{
    return m_webView->page()->mainFrame()->contentsSize();
}

void WebContentAnimationItem::setZoomScale(qreal value, bool immediateCommit)
{
    value = qBound(m_viewportMetaData->m_minimumScale, value, m_viewportMetaData->m_maximumScale);
    qreal curZoomScale = zoomScale();

    if (qFuzzyCompare(value, curZoomScale)) {
        notifyZoomActions(curZoomScale);
        return;
    }

    if (!immediateCommit)
        disableContentUpdates();

    m_webView->setScale(value);

    if (immediateCommit)
        commitZoom();
    else
        m_zoomCommitTimer.start(ZoomCommitDuration);
}

qreal WebContentAnimationItem::zoomScale() const
{
    if (!m_webView)
        return 1.;

    return m_webView->scale();
}

QRectF WebContentAnimationItem::findZoomableRectForPoint(const QPointF& point)
{
    QPointF zoomPoint = m_webView->mapFromParent(point);

    QWebHitTestResult hitResult = m_webView->page()->mainFrame()->hitTestContent(zoomPoint.toPoint());
    QWebElement targetElement = hitResult.enclosingBlockElement();

    while (!targetElement.isNull() && targetElement.geometry().width() < MinDoubleClickZoomTargetWidth)
        targetElement = targetElement.parent();

    if (!targetElement.isNull()) {
        QRectF elementRect = targetElement.geometry();
        qreal overMinWidth = elementRect.width() - ZoomableContentMinWidth;
        if (overMinWidth < 0)
            elementRect.adjust(overMinWidth / 2, 0, -overMinWidth / 2, 0);
        zoomPoint.setX(elementRect.x());
        QRectF resultRect(zoomPoint, elementRect.size());
        return QRectF(m_webView->mapToParent(resultRect.topLeft()),
                      m_webView->mapToParent(resultRect.bottomRight()));
    }
    return QRectF();
}

void WebContentAnimationItem::disableContentUpdates()
{
    //Disable tiling updates
    m_webView->setTiledBackingStoreFrozen(true);
}

void WebContentAnimationItem::enableContentUpdates()
{
    //Enable tiling updates
    m_webView->setTiledBackingStoreFrozen(false);
}

void WebContentAnimationItem::commitZoom()
{
    m_zoomCommitTimer.stop();
    notifyZoomActions(zoomScale());
    enableContentUpdates();
}

void WebContentAnimationItem::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    QGraphicsWidget::resizeEvent(event);
    setZoomScale(size().width() / contentsSize().width());
}

void WebContentAnimationItem::notifyZoomActions(qreal newScale)
{
    bool enableZoomIn = false;
    bool enableZoomOut = false;

    if (m_viewportMetaData->m_userScalable) {

        if (newScale > m_viewportMetaData->m_minimumScale)
            enableZoomOut = true;
        else
            enableZoomOut = false;

        if (newScale < m_viewportMetaData->m_maximumScale)
            enableZoomIn = true;
        else
            enableZoomIn = false;
	} 

    emit updateZoomActions(enableZoomIn, enableZoomOut);
}

} //namespace GVA
