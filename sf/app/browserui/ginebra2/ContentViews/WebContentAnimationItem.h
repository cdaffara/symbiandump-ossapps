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

#ifndef WebContentAnimationItem_h
#define WebContentAnimationItem_h

#include "ViewportMetaDataParser.h"

#include <QGraphicsWidget>
#include <QTimer>

class QGraphicsWebView;

namespace GVA {

class WebContentAnimationItem : public QGraphicsWidget {
    Q_OBJECT
    Q_PROPERTY(qreal zoomScale READ zoomScale WRITE setZoomScale)

public:
    WebContentAnimationItem(QGraphicsWidget* parent = 0, Qt::WindowFlags wFlags = 0);
    ~WebContentAnimationItem();

    void setWebView(QGraphicsWebView* webView);
    QGraphicsWebView* webView() const { return m_webView; }

    void updatePreferredContentSize(const QSize& size);
    QSize contentsSize() const;

    void setZoomScale(qreal, bool immediateCommit = false);
    qreal zoomScale() const;

    QRectF findZoomableRectForPoint(const QPointF& point);

    void setViewportMetaData(ViewportMetaData* viewportMetaData) { m_viewportMetaData = viewportMetaData; }

public Q_SLOTS:
    void enableContentUpdates();
    void disableContentUpdates();
    void commitZoom();

protected:
    void resizeEvent(QGraphicsSceneResizeEvent* event);
	void notifyZoomActions(qreal newScale);

Q_SIGNALS:
    void updateZoomActions(bool enableZoomIn, bool enableZoomOut);

private:
    QGraphicsWebView* m_webView;
    QTimer m_zoomCommitTimer;
    ViewportMetaData* m_viewportMetaData; //Not owned!
}; //WebContentAnimationItem

} //namespace GVA

#endif //WebContentAnimationItem_h

