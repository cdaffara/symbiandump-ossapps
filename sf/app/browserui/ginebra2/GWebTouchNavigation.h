/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/* Temporary implementation of WebTouchNavigation until gestures are ready. */

#ifndef GWEBTOUCHNAVIGATION_H
#define GWEBTOUCHNAVIGATION_H

#include <QTime>
#include <QObject>
#include <QPoint>
#include <QEvent>
#include <QTimeLine>
#include <QWebFrame>
#include <QWebElement>
#include <QWebPage>
#include <QGraphicsWebView>
#include "wrtBrowserDefs.h"


class QWebFrame;
class QWebPage;
class QTimer;
class QWebElement;
class WebViewEventContext;

namespace GVA
{
    class GWebContentViewWidget;
    class ChromeWidget;

    class GWebTouchNavigation : public QObject
    {
    Q_OBJECT
    public:
        enum Direction
        {
            DOWN,           // SOUTH
            UP,             // NORTH
            RIGHT,          // EAST
            LEFT,           // WEST
            BOTTOMRIGHT,    // SOUTHEAST
            BOTTOMLEFT,     // SOUTHWEST
            TOPLEFT,        // NORTHWEST
            TOPRIGHT,       // NORTHEAST
            NONE,
        };

        enum PanDirection {
            HorizontalPan,
            VerticalPan,
            RandomPan
        };

        class DragPoint
        {
        public:
            QPoint iPoint;
            QTime  iTime;
        };

        GWebTouchNavigation(QWebPage* webPage, QGraphicsWebView* view);
        virtual ~GWebTouchNavigation();
        void install();
        void uninstall();
        void setPage( QWebPage * page);
        bool enabled() const { return m_enabled; }
        void setEnabled(bool value) { m_enabled = value; }
        void setWantSlideViewCalls(bool value) { m_wantSlideViewCalls = value; }

    signals:
        void longPressEvent(QPoint pos);// mouse long press signal
        void focusElementChanged(wrtBrowserDefs::BrowserElementType &);
        void pageScrollPositionZero();

        /// Sent when this object starts panning/scrolling the page.  Can be useful for
        /// cancelling things like long-press timers which otherwise wouldn't get
        /// mouse-move events.
        void startingPanGesture(int directionHint);
        void mouseEvent(QEvent::Type type);

        
    protected slots:
        void scrollToEdge();
        void doubleClickTimerExpired();
        void onLongPressTimer();
        void pan();
        void kineticScroll();
        void BlockFocusChanged(QPoint pt);
        void onLoadStarted();
        void onLoadFinished(bool ok);
        void onContentsSizeChanged(const QSize &);
        void enableDClick(bool aValue);
        
    protected:
        bool eventFilter(QObject *object, QEvent *event);
        void mousePressEvent(const QPoint& pos);
        void mouseMoveEvent(const QPoint& pos, const QPoint& diff);
        void mouseReleaseEvent(const QPoint& pos);
        void mouseDoubleClickEvent(const QPoint& pos);
        void contextMenuEvent();
        QWebFrame* getNextScrollableFrame(const QPoint& pos);
        void scrollFrame(const QPoint& diff);

        void startScrollTimer();
        void updateFlickScrollDistance();
        bool isFlick() ;
        QPoint speed() ;
        QPoint currentPos();
        QPoint previousPos();
        qreal dragTime() const;
        Direction findDirectionWithAngle(const QPoint& diff);
        bool isNear(qreal aAngleUnderTest, qreal aMinAngle, qreal aMaxAngle);
        qreal findAngle(const int& xDelta,const int& yDelta);
        int roundOff(qreal num);
        QRect findEnclosingBlock(QMouseEvent* ev);
        QWebHitTestResult getHitTestResult(QMouseEvent* ev);
        void calculateActualScrollDistance();
        void setNewScrollDistance(QPoint blockCanvasPoint, int thresholdCheckVal);
        void handleMousePressEvent(QMouseEvent* ev);
        void handleMouseReleaseEvent(QMouseEvent* ev);
        void handleDoubleClickEvent(QMouseEvent* ev);
        void cancelPressEvent();
        void cancelReleaseEvent();

        // Methods that can be overridden in subclasses.  Mainly here so that
        // we can create a subclass that can interact with GWebContentViewWidget
        // and ChromeWidget, and still handle generic QWebViews.
        virtual qreal slideView(qreal delta) { Q_UNUSED(delta) return 0; }
        virtual qreal shrinkView(qreal delta) { Q_UNUSED(delta) return 0; }
        virtual void setViewBlockElement(const QWebElement &el) { Q_UNUSED(el) }
        virtual qreal viewInitialScale() { return 1; }
        virtual void setViewZoomFactor(qreal zoom) { Q_UNUSED(zoom) }

    private:
        void highlightableElement(QMouseEvent* ev);
        QWebElement getClosestAnchorElement(QMouseEvent* ev);
        bool traverseNextNode(QWebElement parentNode,QWebElement& nextNode);

        void handleHighlightChange(QMouseEvent* ev);
        bool canDehighlight(QMouseEvent* ev);
        void dehighlight(QMouseEvent* ev);
        void emitFocusedElementChanged();
        void startLongPressTimer();
        void stopLongPressTimer();

        void stopScrolling();
        void startPanGesture(PanDirection);
        void panBy(const QPointF& delta);
        void scrollCurrentFrame (int dx, int dy);
        void setCurrentFrameScrollPosition (QPoint& pos);

        Qt::KeyboardModifier getEventModifier(const QPoint& pos);

    protected:
        QWebPage* m_webPage;
        QGraphicsWebView* m_view;
        QWebFrame* m_frame;
        bool m_scrolled;
        QPoint m_touchPosition;
        QPointF m_scrollDistance;
        QPointF m_actualScrollDistance;
        QPointF m_prevPoint;
        QList<DragPoint> m_dragPoints;
        Direction m_flickDirection;
        QTime m_lastMoveEventTime;
        QTimer* m_doubleClickTimer;
        QMouseEvent *m_pressEvent;
        QMouseEvent *m_releaseEvent;
        QPoint m_focusedBlockPt;
        QWebElement m_anchorElement;
        QPoint m_higlightedPos;
        bool m_ishighlighted;
        int m_offset;
        QTimer* m_longPressTimer;// long press timer
        QPoint m_longPressPosition;
        QPoint m_initialSpeed;
        qreal m_finalzoomfactor;
        QTimer* m_scrollTimer;
        QPoint  m_scrollDelta;
        QTimer* m_kineticTimer;
        QPointF m_kineticSpeed;
        QTime   m_actualTime;
        int     m_kineticScrollTime;
        QPoint  m_initialScrollPos;
        bool m_isPanning;
        QTime m_delayedPressMoment;
        QPointF m_dragStartPos;
        PanDirection m_panDirection;
        QPointF m_panModeResidue;
        bool m_enabled;
        bool m_textSelected; // whether the text of an input field is selected manually

        bool m_isLoading;
        int m_contentHeight;
        // m_isContextEvent is set to true when a context menu event is received
        // and then cleared when on mouse release events.  Mouse release events
        // are ignored when this flags is true to avoid activating links when
        // the context menu is activated over them.
        bool m_isContextEvent;
        // Flag that governs whether calls to slideView should be made or not.
        bool m_wantSlideViewCalls;
        
        bool m_doubleClickEnabled;
    };

}

#endif
