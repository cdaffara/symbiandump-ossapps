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

#ifndef ScrollableWebContentView_h
#define ScrollableWebContentView_h

#include "Gestures/GestureEvent.h"
#include "Gestures/GestureListener.h"
#include "Gestures/GestureRecognizer.h"
#include "ScrollableViewBase.h"
#include "ZoomMetaData.h"

#include <QPropertyAnimation>
#include <QTimer>

class QWebHitTestResult;

namespace GVA {

class ViewportMetaData;
class WebContentAnimationItem;

class ScrollableWebContentView : public ScrollableViewBase, protected GestureListener {
    Q_OBJECT
public:
    ScrollableWebContentView(WebContentAnimationItem* viewportProxyWidget, QGraphicsItem* parent = 0);
    ~ScrollableWebContentView();

    WebContentAnimationItem* viewportWidget() const;
    void zoomToScreenCenter(bool zoomIn);

    ZoomMetaData currentPageInfo();
    void setCurrentPageInfo(ZoomMetaData data);
    ZoomMetaData defaultZoomData();
    void updatePreferredContentSize();
    void setSuperPage();
    void setGesturesEnabled(bool value) { m_gesturesEnabled = value; } 
    bool gesturesEnabled() { return m_gesturesEnabled; }

Q_SIGNALS:
    void contextEventObject(QWebHitTestResult* eventTarget);
    void viewScrolled(QPoint& scrollPos, QPoint& delta);
    void mouseEvent(QEvent::Type type);

public Q_SLOTS:
    void reset();
    void contentsSizeChanged(const QSize&);
    void pageLoadFinished(bool);

protected:
    bool sceneEventFilter(QGraphicsItem*, QEvent*);

    //From GestureListener
    void handleGesture(GestureEvent*);

    //Helpers
    void handlePress(GestureEvent*);
    void handleRelease(GestureEvent*);
    void handleFlick(GestureEvent*);
    void handleDoubleTap(GestureEvent*);
    void handlePan(GestureEvent*);
    void handleLongTap(GestureEvent*);

    void setViewportWidgetGeometry(const QRectF& r);
    void startZoomAnimToItemHotspot(const QPointF& hotspot, const QPointF& viewTargetHotspot, qreal scale, QRectF target = QRectF());
    bool isZoomedIn() const;

    //To handle kinetic scroller state changes
    void stateChanged(KineticScrollable::State oldState, KineticScrollable::State newState);

    void startZoomAnimation(const QRectF& zoomedRect);
    void stopZoomAnimation();
    void updateZoomEndRect();
    void resizeEvent(QGraphicsSceneResizeEvent* event);
    void adjustViewportSize(QSizeF oldSize, QSizeF newSize);
    void sendEventToWebKit(QEvent::Type type, QPointF& scenPos);
   
protected Q_SLOTS:
    void zoomAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State);

private:
    GestureRecognizer m_gestureRecognizer;
    QRectF m_animationEndRect;
    QPropertyAnimation* m_zoomAnimator;          //Animates zooming transitions
    ViewportMetaData* m_viewportMetaData;
    QTimer m_tileUpdateEnableTimer;
    bool m_gesturesEnabled;
}; //ScrollableWebContentView

} //namespace GVA

#endif //ScrollableWebContentView_h
