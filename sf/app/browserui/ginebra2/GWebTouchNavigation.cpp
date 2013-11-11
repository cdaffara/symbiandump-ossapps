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

#include "GWebTouchNavigation.h"
#include "wrtBrowserUtils.h"

#include <QWebPage>
#include <QWebFrame>
#include <QApplication>
#include <QWebElement>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <qmath.h>
#include <QDebug>
#include <QGraphicsView>


namespace GVA {

const int KFlickSpeed = 400;
const qreal PI = 3.1415926535897932;
const qreal KAngleTolerance = 30;
const int KAnimationTimeout = 40;
const qreal KDecelerationFactor = 0.8;
const int KMilliSecond = 1000;
const int KThresholdForRightToLeftMotion = 100;  // 100 pixels
const int KThresholdForLeftToRightMotion = -100; // 100 pixels
const int XAlignVal = 10;
const int KDecelerationTimer = 125; //Decelerate only if flicked/swiped after KDecelerationTimer milli seconds after last drag event

const int KDoubleClickTimeOut = 400;
const QPoint KFocussPoint(5, 50);
const int KTouchThresholdX = 20;
const int KTouchThresholdY = 20;
const int KPanThreshold = 2;
const int KThreshHoldValForLink = 10;
const qreal KDeccelaration = 1000.00;

static const int KStartPanDistance = 50;
static const int KWaitForClickTimeoutMS = 200;
static const int KLongPressDuration = 1000;
static const int KLongPressThreshold = 30;
//The amount of pixels to try to pan before pan mode unlocks
static const int KPanModeChangeDelta = 100;

/*!
    \class WebTouchNavigation
    \since cwrt 1.0
    \brief cwrt navigation.

    \sa WebNavigation, WebHtmlTabIndexedNavigation, WebCursorNavigation, WebDirectionalNavigation
*/
GWebTouchNavigation::GWebTouchNavigation(QWebPage* webPage, QGraphicsWebView* view)
: m_webPage(webPage)
, m_view(view)
, m_frame(0)
, m_doubleClickTimer(0)
, m_pressEvent(0)
, m_releaseEvent(0)
, m_focusedBlockPt(-1, -1)
, m_ishighlighted(false)
, m_offset(0)
, m_longPressTimer(0)
, m_finalzoomfactor(0)
, m_scrollTimer(0)
, m_kineticTimer(0)
, m_isPanning(false)
, m_enabled(true)
, m_textSelected(false)
, m_isLoading(false)
, m_contentHeight(0)
, m_isContextEvent(false)
, m_wantSlideViewCalls(true)
, m_doubleClickEnabled(true)
{
        install();
        connect(m_webPage, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
        connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
}

inline void GWebTouchNavigation::cancelPressEvent() {
    if (m_pressEvent) {
        delete m_pressEvent;
        m_pressEvent = 0;
    }
}

inline void GWebTouchNavigation::cancelReleaseEvent() {
    if (m_releaseEvent) {
        delete m_releaseEvent;
        m_releaseEvent = 0;
    }
}

/*!
*/
GWebTouchNavigation::~GWebTouchNavigation()
{
    delete m_doubleClickTimer;
    delete m_scrollTimer;
    delete m_kineticTimer;
    delete m_longPressTimer;
    uninstall();

    cancelPressEvent();
    cancelReleaseEvent();
}

void GWebTouchNavigation::install()
{
    m_view->installEventFilter(this);
}

void GWebTouchNavigation::BlockFocusChanged(QPoint pt)
{
    m_focusedBlockPt = pt;
}

void GWebTouchNavigation::uninstall()
{
    if (m_view)
        m_view->removeEventFilter(this);
}

bool GWebTouchNavigation::eventFilter(QObject *object, QEvent *event)
{
    if (object != m_view || !enabled())
        return false;

    switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (static_cast<QMouseEvent *>(event)->buttons() & Qt::LeftButton) {
                mousePressEvent(static_cast<QMouseEvent *>(event)->pos());
                return true;
            }
            break;
        case QEvent::MouseMove:
            if (static_cast<QMouseEvent *>(event)->buttons() & Qt::LeftButton) {
                QPoint diff = m_touchPosition - static_cast<QMouseEvent*>(event)->globalPos();
                mouseMoveEvent(static_cast<QMouseEvent*>(event)->pos(), diff);
                return true;
            }
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(static_cast<QMouseEvent*>(event)->pos());
            return true;
        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent(static_cast<QMouseEvent*>(event)->pos());
            return true;
        case QEvent::GraphicsSceneMousePress:
            if (static_cast<QGraphicsSceneMouseEvent*>(event)->buttons() & Qt::LeftButton) {
                mousePressEvent(static_cast<QGraphicsSceneMouseEvent*>(event)->pos().toPoint());
                emit mouseEvent(QEvent::GraphicsSceneMousePress);
                return true;
            }
            break;
        case QEvent::GraphicsSceneMouseMove:
            if (static_cast<QGraphicsSceneMouseEvent *>(event)->buttons() & Qt::LeftButton) {
                QPoint diff = static_cast<QGraphicsSceneMouseEvent*>(event)->lastScenePos().toPoint() - static_cast<QGraphicsSceneMouseEvent*>(event)->scenePos().toPoint();
                mouseMoveEvent(static_cast<QGraphicsSceneMouseEvent*>(event)->pos().toPoint(), diff);
                return true;
            }
            break;
        case QEvent::GraphicsSceneMouseRelease:
            mouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent*>(event)->pos().toPoint());
            emit mouseEvent(QEvent::GraphicsSceneMouseRelease);
            return true;
        case QEvent::GraphicsSceneMouseDoubleClick:
            mouseDoubleClickEvent(static_cast<QGraphicsSceneMouseEvent*>(event)->pos().toPoint());
            emit mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
            return true;
        case QEvent::GraphicsSceneContextMenu:
            contextMenuEvent();
            break;
        default:
            break;
    }
    return false;
}

void GWebTouchNavigation::stopScrolling()
{
    if (m_scrollTimer && m_scrollTimer->isActive()) {
        m_scrollTimer->stop();
        m_scrollDelta = QPoint(0,0);
    }

    if (m_kineticTimer && m_kineticTimer->isActive()) {
        m_kineticTimer->stop();
        m_kineticSpeed.setX(0.0);
        m_kineticSpeed.setY(0.0);
    }
}


void GWebTouchNavigation::pan()
{
    if (m_scrollDelta.x() != 0 || m_scrollDelta.y() != 0) {
            scrollCurrentFrame(-m_scrollDelta.x(), m_scrollDelta.y());
        m_scrollDelta = QPoint(0,0);
    }
}

#define DECELERATION 0.0002

void GWebTouchNavigation::kineticScroll()
{
    qreal decelX = m_kineticSpeed.x() > 0 ? -DECELERATION : DECELERATION;
    qreal decelY = m_kineticSpeed.y() > 0 ? -DECELERATION : DECELERATION;
    qreal dx = 0;
    qreal dy = 0;
    qreal vx = 0;
    qreal vy = 0;

    m_kineticScrollTime += m_actualTime.elapsed();

    if (m_kineticSpeed.x()) {
        vx = m_kineticSpeed.x() + decelX * m_kineticScrollTime;
        if (vx * m_kineticSpeed.x() < 0) {
            dx = 0;
            vx = 0;
        }
        else {
            dx = m_kineticScrollTime * m_kineticSpeed.x() +
                0.5 * decelX * m_kineticScrollTime * m_kineticScrollTime;
        }
    }

    if (m_kineticSpeed.y()) {
        vy = m_kineticSpeed.y() + decelY * m_kineticScrollTime;
        if (vy * m_kineticSpeed.y() < 0) {
            dy = 0;
            vy = 0;
        }
        else {
            dy = m_kineticScrollTime * m_kineticSpeed.y() +
                 0.5 * decelY * m_kineticScrollTime * m_kineticScrollTime;
        }
    }

    QPoint scrollPos = m_frame->scrollPosition();
    QPoint distPos = m_initialScrollPos + QPointF(dx, dy).toPoint();

    if (vx != 0 || vy != 0) {
            setCurrentFrameScrollPosition(distPos);
    }

    if ((vx == 0 && vy == 0) || scrollPos == m_frame->scrollPosition()) {
        stopScrolling();
    }

}



/*!
    Scrolls QWebFrame to the given position
    \sa QWebFrame::scroll()
*/
void GWebTouchNavigation::scrollFrame(const QPoint& diff)
{
    if (qAbs(diff.x()) < KPanThreshold && qAbs(diff.y()) < KPanThreshold)
        return;
    else if (m_dragPoints.size() == 1) {
        setViewBlockElement(QWebElement());
    }

    if (!m_isPanning) {
        qreal dy = qAbs(diff.y());
        qreal dx = qAbs(diff.x());
        if (dy > KStartPanDistance || dx > KStartPanDistance
            || m_delayedPressMoment.elapsed() > KWaitForClickTimeoutMS) {
            //get the scroll direction
            Direction scrollDir = findDirectionWithAngle(diff);
            if (scrollDir == UP || scrollDir == DOWN)
                startPanGesture(VerticalPan);
            else if (scrollDir == LEFT || scrollDir == RIGHT)
                startPanGesture(HorizontalPan);
            else
                startPanGesture(RandomPan);
            m_isPanning = true;
        }
    }

    if (m_isPanning) {
        m_scrolled= false;
        m_frame = getNextScrollableFrame(diff);
        QPoint scrollPosition = m_frame->scrollPosition();
        if (diff.manhattanLength())
            panBy(diff);

        m_scrolled = (scrollPosition != m_frame->scrollPosition());
    }
}

void GWebTouchNavigation::startPanGesture(PanDirection directionHint) {
    emit startingPanGesture((int)directionHint);
    m_panDirection = directionHint;
    m_panModeResidue = QPointF(0., 0.);
}

void GWebTouchNavigation::panBy(const QPointF& delta) {

    m_panModeResidue += delta;

    if (qAbs(m_panModeResidue.x()) > KPanModeChangeDelta)
        m_panDirection = HorizontalPan;

    if (qAbs(m_panModeResidue.y()) > KPanModeChangeDelta)
        m_panDirection = VerticalPan;

    if (qAbs(m_panModeResidue.x()) > KPanModeChangeDelta
        && qAbs(m_panModeResidue.y()) > KPanModeChangeDelta)
        m_panDirection = RandomPan;

    QPointF p;
    if (m_panDirection == HorizontalPan)
        p.setX(delta.x());
    if (m_panDirection == VerticalPan)
        p.setY(delta.y());
    if (m_panDirection == RandomPan) {
        p.setX(delta.x());
        p.setY(delta.y());
    }

    scrollCurrentFrame(p.x(),p.y());
}

void GWebTouchNavigation::highlightableElement(QMouseEvent* ev) {
    m_anchorElement = getClosestAnchorElement(ev);
}

inline int xInRect(const QRect& r, int x)
{
    return std::min(std::max(x, r.x()), r.x() + r.width());
}

inline int yInRect(const QRect& r, int y)
{
    return std::min(std::max(y, r.y()), r.y() + r.height());
}

bool GWebTouchNavigation::traverseNextNode(QWebElement parentNode,QWebElement& nextNode)
{
    if (!parentNode.firstChild().isNull()) {
        nextNode = parentNode.firstChild();
        return true;
    }

    if (!parentNode.nextSibling().isNull()) {
        nextNode = parentNode.nextSibling();
        return true;
    }

    QWebElement n = parentNode;
    while (!n.isNull() && n.nextSibling().isNull())
        n = n.parent ();

    if (!n.isNull()) {
        nextNode = n.nextSibling();
        return true;
    }

    return false;
}


QWebElement GWebTouchNavigation::getClosestAnchorElement(QMouseEvent* ev)
{
    QWebElement webElement;
    QWebHitTestResult htRes = getHitTestResult(ev);
    QWebElement hitElement = htRes.element();
    //check whether hit test returns a link element
    if (!htRes.linkElement().isNull()) {
        webElement = htRes.linkElement();
        m_higlightedPos = ev->pos();
        m_ishighlighted = true;
    }
    //get the closet anchor element
    else {
        QPoint docPoint = (m_touchPosition + m_frame->scrollPosition());
        int dist = 99999999;
        QWebElement result;
        QWebElement ele = m_webPage->currentFrame()->documentElement();
        do {
            if (ele.tagName().compare("a",Qt::CaseInsensitive) == 0 ) {
                QRect r = ele.geometry();
                if (r.contains(docPoint)) {
                    dist = 0;
                    result = ele;
                    break;
                }

                int x = xInRect(r, docPoint.x());
                int y = yInRect(r, docPoint.y());
                int d = (docPoint.x() - x) * (docPoint.x() - x) + (docPoint.y() - y) * (docPoint.y() - y);
                if (dist > d) {
                    dist = d;
                    result = ele;
                }
            }
        }while(traverseNextNode(ele,ele));

        // check if we are close enough and calculate with zoom factor.
        if (dist< (KThreshHoldValForLink/m_view->zoomFactor())) {
            QRect r = result.geometry();
            r.translate(2,2);
            r.setWidth(2+2);
            r.setHeight(2+2);
            webElement = result;
            m_higlightedPos = QPoint(xInRect(r, docPoint.x()), yInRect(r, docPoint.y())) - m_frame->scrollPosition();
            m_ishighlighted = true;
            QPoint centerpt = webElement.geometry().center();
            m_offset = (docPoint.x()- centerpt.x())*(docPoint.x()- centerpt.x()) + (docPoint.y()- centerpt.y())*(docPoint.y()- centerpt.y());

        }
    }
    return webElement;
}

/*!
    Sends a mouse press event to QWebPage
*/
void GWebTouchNavigation::mousePressEvent(const QPoint& pos)
{
    m_delayedPressMoment.start();
    // stop deceleration and don't send further events to engine if scroll timer is active
    if (m_scrollTimer && m_scrollTimer->isActive()) {
        m_prevPoint.setX(0);
        m_prevPoint.setY(0);
        m_scrollDistance.setX(0);
        m_scrollDistance.setY(0);

        m_lastMoveEventTime.setHMS(0,0,0,0); //H, M, S, mS
        m_scrolled = false;
        m_ishighlighted = false;
        m_higlightedPos = m_touchPosition = pos;
        m_frame = m_webPage->frameAt(pos);
        if (!m_frame)
            m_frame = m_webPage->currentFrame();

        m_dragPoints.clear();

        DragPoint dragPoint;
        dragPoint.iPoint = m_touchPosition;
        dragPoint.iTime = QTime::currentTime();
        m_dragPoints.append(dragPoint);
        m_offset = 0;

        m_pressEvent = new QMouseEvent(QEvent::MouseButtonPress, m_touchPosition, Qt::LeftButton, Qt::LeftButton, getEventModifier(m_touchPosition));
        m_scrollTimer->stop();
        return;
    }

    if (m_doubleClickTimer) {
        // If there is another click event almost on the same region
        // as the previous click before doubleClick timeout
        // Consider it as double click.
        if (m_doubleClickTimer->isActive() && m_pressEvent) {
            QPoint diff = m_pressEvent->pos() - pos;
            if (qAbs(diff.x()) < KTouchThresholdX && qAbs(diff.y()) < KTouchThresholdY) {
                mouseDoubleClickEvent(pos);
                return;
            }
        }
        m_doubleClickTimer->stop();
    } else {
        m_doubleClickTimer = new QTimer(this);
        connect(m_doubleClickTimer, SIGNAL(timeout()), this, SLOT(doubleClickTimerExpired()));
    }

    //Clear the previous press and release events.
    cancelPressEvent();
    cancelReleaseEvent();

    m_doubleClickTimer->start(KDoubleClickTimeOut);
    m_pressEvent = new QMouseEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, getEventModifier(pos));
    m_longPressPosition = pos;
    startLongPressTimer();
}

void GWebTouchNavigation::handleMousePressEvent(QMouseEvent* ev)
{
    if (!ev) return;
    m_lastMoveEventTime.setHMS(0,0,0,0); //H, M, S, mS
    m_scrolled = false;
    m_ishighlighted = false;
    m_higlightedPos = m_touchPosition = ev->pos();
    m_frame = m_webPage->frameAt(ev->pos());
    if (!m_frame)
        m_frame = m_webPage->currentFrame();

    m_dragPoints.clear();

    DragPoint dragPoint;
    dragPoint.iPoint = m_touchPosition;
    dragPoint.iTime = QTime::currentTime();
    m_dragPoints.append(dragPoint);

    m_offset = 0;

    highlightableElement(ev);
    emitFocusedElementChanged();

    //send a mouse press
    QMouseEvent iev(ev->type(), m_touchPosition, ev->button(), ev->buttons(), getEventModifier(m_higlightedPos));
    m_webPage->event(&iev);
    m_textSelected = false;
}

void GWebTouchNavigation::doubleClickTimerExpired()
{
    handleMousePressEvent(m_pressEvent);

    cancelPressEvent();

    if (m_releaseEvent) {
        handleMouseReleaseEvent(m_releaseEvent);

        //clear release event
        cancelReleaseEvent();
    }
    if (m_doubleClickTimer && m_doubleClickTimer->isActive())
        m_doubleClickTimer->stop();
}

bool GWebTouchNavigation::canDehighlight(QMouseEvent* ev)
{
    bool checkDehighlight = false;
    QPoint pt = ev->pos() + m_frame->scrollPosition();

    if (m_scrolled && m_ishighlighted) {
        checkDehighlight = true;
    }
    else if (!m_scrolled && m_ishighlighted) {
        QRect rect = m_anchorElement.geometry();

        if (m_anchorElement.geometry().contains(pt)) {
            checkDehighlight = false;
        }
        else if (m_offset){
            QPoint centerpt = m_anchorElement.geometry().center();
            int newoffset = (pt.x()- centerpt.x())*(pt.x()- centerpt.x()) + (pt.y()- centerpt.y())*(pt.y()- centerpt.y());

            if (newoffset <= m_offset ) {
                m_offset = newoffset;
                checkDehighlight = false;
            }
            else {
                m_offset =0;
                checkDehighlight = true;
            }
        }
        else {
            checkDehighlight = true;
        }
    }
    return checkDehighlight;
}
void  GWebTouchNavigation::dehighlight(QMouseEvent* ev)
{
    m_higlightedPos = QPoint(0,0);
    m_ishighlighted = false;
    QMouseEvent iev(QEvent::MouseButtonPress,m_higlightedPos,ev->button(), ev->buttons(), ev->modifiers());
    m_webPage->event(&iev);
}

void GWebTouchNavigation::handleHighlightChange(QMouseEvent* ev)
{
    if (!canDehighlight(ev)) return;

    dehighlight(ev);
}

/*!
    Scrolls the frame
    \sa scrollFrame()
*/
void GWebTouchNavigation::mouseMoveEvent(const QPoint& pos, const QPoint& diff)
{
    if (m_longPressTimer && m_longPressTimer->isActive()){
        QPoint diff2 = m_longPressPosition - pos;
        if (qAbs(diff2.x()) > KLongPressThreshold || qAbs(diff2.y()) > KLongPressThreshold) {
            stopLongPressTimer();
        }
    }

    if (m_pressEvent){
        QPoint diff2 = m_pressEvent->pos() - pos;
        if (qAbs(diff2.x()) < KTouchThresholdX && qAbs(diff2.y()) < KTouchThresholdY) {
            return;
        }
    }

    if (m_doubleClickTimer && m_doubleClickTimer->isActive()) {
        //send mousePressEvent
        m_doubleClickTimer->stop();
        handleMousePressEvent(m_pressEvent);
        cancelPressEvent();
    }

    QMouseEvent tmpEv(QEvent::MouseMove, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    handleHighlightChange(&tmpEv);

    //the viewport hasn't been scrolled
    if (!m_scrolled) {
        QWebHitTestResult hitTestResult = m_frame->hitTestContent(m_touchPosition);
        QWebElement oriElement = hitTestResult.element();
        QWebElement curElement = m_frame->hitTestContent(pos).element();
        // the mousepress event and the mousemove event have the same target
        if (oriElement == curElement) {
            // the mousemove event's position is in an input field
            if (hitTestResult.isContentEditable()) {
                QMouseEvent moveEvt(QEvent::MouseMove, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                m_webPage->event(&moveEvt);
                m_textSelected = true;
            }
        }
    }
    if (!m_textSelected)
        scrollFrame(diff);

    //m_touchPosition = pos;
    DragPoint dragPoint;
    dragPoint.iPoint = pos;
    dragPoint.iTime = QTime::currentTime();
    m_dragPoints.append(dragPoint);

    QTime now(QTime::currentTime());
    m_lastMoveEventTime.setHMS(now.hour(),now.minute(), now.second(), now.msec());

    while (m_dragPoints.size() > 4)
        m_dragPoints.removeFirst();

}

/*!
    If WebCanvas or QWebFrame were scrolled starts the deceleration alogrithm
    Otherwise sends the mouse release event to QWebPage
    \sa startDeceleration()
*/
void GWebTouchNavigation::mouseReleaseEvent(const QPoint& pos)
{
    stopLongPressTimer();

    if (m_isPanning)
        m_isPanning = false;
    if (m_doubleClickTimer && m_doubleClickTimer->isActive()) {
        m_releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        return;
    }

    if (m_isContextEvent) {
        // A context event just occurred, ignore the release event so that links are not activated.
        m_isContextEvent = false;
        return;
    }
    //mouseReleaseEvent is called in two cases. 1. Double click, 2. Mouse Drag
    //m_releaseEvent is not null only in Mouse double click
    //So delete m_releaseEvent only in double click case.
    //Send release event in mouse move case
    if (m_releaseEvent) {
        cancelReleaseEvent();
    } else {
        QMouseEvent tmpEv(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        handleMouseReleaseEvent(&tmpEv);
    }
}
Qt::KeyboardModifier GWebTouchNavigation::getEventModifier(const QPoint& pos)
{
    QWebFrame *frame = m_webPage->frameAt(pos);
    if (!frame)
          frame = m_webPage->currentFrame();
    QWebHitTestResult htr = frame->hitTestContent(pos);
    if (htr.element().tagName().toLower().compare("select")==0  && htr.element().hasAttribute("multiple"))
        return Qt::ControlModifier;
    return Qt::NoModifier;
}

void GWebTouchNavigation::handleMouseReleaseEvent(QMouseEvent* ev)
{
    m_frame = m_webPage->frameAt(ev->pos());
    if (!m_frame)
        m_frame = m_webPage->currentFrame();
    Q_ASSERT(m_frame);

    stopLongPressTimer();
    if (m_scrolled) {
        int msecs = 0;
        if (!m_lastMoveEventTime.isNull()) {
            //Start deceleration only if the delta since last drag event is less than threshold
            QTime now(QTime::currentTime());
            msecs = m_lastMoveEventTime.msecsTo(now);
            m_lastMoveEventTime.setHMS(0,0,0,0);
        }
        if (msecs < KDecelerationTimer) {
            if ( isFlick()) {
                updateFlickScrollDistance();
            }
            else {
                QRect enclosingRect = findEnclosingBlock(ev);
                QPoint blockDocPt(enclosingRect.x(), enclosingRect.y());
                QPoint blockCanvasPoint(blockDocPt - m_frame->scrollPosition());
                calculateActualScrollDistance();
                int thresholdCheckVal = blockCanvasPoint.x() - m_actualScrollDistance.x();
                setNewScrollDistance(blockCanvasPoint, thresholdCheckVal);
            }
            startScrollTimer();
        }
    } else {
        // we don't want to automatically display the SIP (Software input panel) by qtwebkit
        bool sipEnabled = qApp->autoSipEnabled();
        qApp->setAutoSipEnabled(false);
        QMouseEvent iev = m_ishighlighted ?
        QMouseEvent(ev->type(), m_higlightedPos, ev->button(), ev->buttons(), getEventModifier(m_higlightedPos)) :
        QMouseEvent(QEvent::MouseButtonRelease, m_touchPosition, ev->button(), ev->buttons(), getEventModifier(m_touchPosition));
        m_webPage->event(&iev);
        qApp->setAutoSipEnabled(sipEnabled);
#if defined __SYMBIAN32__
// FIXME Remove this, it will be fixed Qt 4.6.3 ?
        if (ev->button() == Qt::LeftButton) {
            QWebHitTestResult htr = m_frame->hitTestContent(ev->pos());
            if (htr.isContentEditable()) {
                QEvent vkbEvent(QEvent::RequestSoftwareInputPanel);
                QList<QGraphicsView*> views = m_view->scene()->views();
                QWidget* view = qobject_cast<QWidget*>(views.value(0));
                if (view)
                    QApplication::sendEvent(view, &vkbEvent);
            }
        }
#endif
    }
}

void GWebTouchNavigation::mouseDoubleClickEvent(const QPoint& pos)
{

    if (m_doubleClickTimer && !m_doubleClickTimer->isActive())
        return;
    else if (m_doubleClickTimer)
        m_doubleClickTimer->stop();

    //If the page is already scrolling(because of a previous doubletap)
    //we need to stop the timer before we start scrolling the new block again.
    if (m_scrollTimer && m_scrollTimer->isActive())
        m_scrollTimer->stop();

    QMouseEvent tmpEv(QEvent::MouseButtonDblClick, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    handleDoubleClickEvent(&tmpEv);
}

void GWebTouchNavigation::handleDoubleClickEvent(QMouseEvent* ev)
{
    if (!m_doubleClickEnabled)
        return;

    QPoint imageFocusPoint;
    QWebHitTestResult hitTest = getHitTestResult(ev);

    QWebElement block = hitTest.element();

    if (block.tagName() != "IMG" && (block.styleProperty(QString("background-image"),QWebElement::InlineStyle) == ""))
        block = hitTest.enclosingBlockElement();

    QWebFrame* frame = m_webPage->frameAt(ev->pos());
    m_frame = (frame) ? frame : m_webPage->mainFrame();

    QRect enclosingRect = block.geometry();

    QPoint blockCanvasPt = QPoint(enclosingRect.topLeft()) - m_frame->scrollPosition();
    setViewBlockElement(QWebElement());
    if (m_focusedBlockPt == blockCanvasPt) {
        m_focusedBlockPt.setX(-1);
        m_focusedBlockPt.setY(-1);
        qreal m_Ratiox = (qreal) blockCanvasPt.x() / block.geometry().width();
        qreal m_Ratioy = (qreal) blockCanvasPt.y() / block.geometry().height();
        setViewZoomFactor(viewInitialScale());
        QPoint m_InfocusBlockPt = QPoint(block.geometry().topLeft()) - m_webPage->mainFrame()->scrollPosition();
        m_webPage->currentFrame()->scroll(m_InfocusBlockPt.x() - (m_Ratiox * block.geometry().width()),
                                                                                    m_InfocusBlockPt.y() - (m_Ratioy * block.geometry().height()));
        m_finalzoomfactor = 0;
    }else {
        if (block.tagName() != "IMG" && (block.styleProperty(QString("background-image"),QWebElement::InlineStyle) == "")) {
            m_finalzoomfactor   = (qreal) (m_webPage->viewportSize().width() - 10) * m_view->zoomFactor();
            m_finalzoomfactor = (qreal) m_finalzoomfactor / (enclosingRect.width());
            QString str;
            str.setNum(m_finalzoomfactor,'f',2);
            m_finalzoomfactor = str.toDouble();
        }else {
            qreal factor;
            factor = 1/viewInitialScale();
            int boundW = block.geometry().width() * factor/m_view->zoomFactor();
            int boundH = block.geometry().height() * factor/m_view->zoomFactor();
            qreal factorw = 0.0,factorh = 0.0 ;

            if ( boundW > m_webPage->viewportSize().width())
                factorw = (qreal)(m_webPage->viewportSize().width()-5)/ block.geometry().width();

            if (boundH > m_webPage->viewportSize().height())
                factorh = (qreal)(m_webPage->viewportSize().height()-5)/ block.geometry().height();

            if ( (factorw == 0.0) && (factorh == 0.0))
                ;
            else if (factorw == 0.0)
                factor = factorh * m_view->zoomFactor();
            else if (factorh == 0.0)
                factor = factorw * m_view->zoomFactor();
            else {
                factor = ((factorh < factorw) ? factorh : factorw) * m_view->zoomFactor();
            }

            QString str;
            str.setNum(factor,'f',2);
            factor = str.toDouble();

            if (m_finalzoomfactor != factor)
                m_finalzoomfactor = factor;
        }


        if (m_finalzoomfactor != m_view->zoomFactor()) {
            setViewZoomFactor(m_finalzoomfactor);
            m_focusedBlockPt = QPoint(block.geometry().topLeft()) - m_frame->scrollPosition();

            if (block.tagName() != "IMG" && (block.styleProperty(QString("background-image"),QWebElement::InlineStyle) == ""))
                scrollCurrentFrame(m_focusedBlockPt.x() - KFocussPoint.x() , m_focusedBlockPt.y() - KFocussPoint.y());
            else {
                if ((m_webPage->viewportSize().width() - block.geometry().width()) > 0)
                    imageFocusPoint.setX((m_webPage->viewportSize().width() - block.geometry().width())/2);
                else
                    imageFocusPoint.setX(0);

                if ((m_webPage->viewportSize().height() - block.geometry().height()) > 0)
                    imageFocusPoint.setY((m_webPage->viewportSize().height() - block.geometry().height())/2);
                else
                    imageFocusPoint.setY(0);

                scrollCurrentFrame(m_focusedBlockPt.x() - imageFocusPoint.x() , m_focusedBlockPt.y() - imageFocusPoint.y());
            }

            m_focusedBlockPt = QPoint(block.geometry().topLeft()) - m_frame->scrollPosition();
        } else {
            //Get doc size and current bottom right view corner point in document
            QSize viewSize = m_webPage->viewportSize();
            QSize contentSize = m_frame->contentsSize();
            QPoint documentViewPoint = QPoint(viewSize.width(),viewSize.height()) + m_frame->scrollPosition();
            QPoint docFocusPoint;

            if (block.tagName() != "IMG" && (block.styleProperty(QString("background-image"),QWebElement::InlineStyle) == ""))
                docFocusPoint = KFocussPoint + m_frame->scrollPosition();
            else {
                if ((m_webPage->viewportSize().width() - block.geometry().width()) > 0)
                     imageFocusPoint.setX((m_webPage->viewportSize().width() - block.geometry().width())/2);
                else
                    imageFocusPoint.setX(0);

                if ((m_webPage->viewportSize().height() - block.geometry().height()) > 0)
                    imageFocusPoint.setY((m_webPage->viewportSize().height()- block.geometry().height())/2);
                else
                    imageFocusPoint.setY(0);

                docFocusPoint = imageFocusPoint + m_frame->scrollPosition();
            }

            m_focusedBlockPt = QPoint(block.geometry().x(), block.geometry().y());
            m_scrollDistance.setX(m_focusedBlockPt.x() - docFocusPoint.x());
            m_scrollDistance.setY(m_focusedBlockPt.y() - docFocusPoint.y());

            QPoint scrollableDistance(0, 0);
            QPoint viewStartPoint = QPoint(0,0) + m_frame->scrollPosition();

            if (m_scrollDistance.x() < 0)
                scrollableDistance.setX(viewStartPoint.x());
             else
                scrollableDistance.setX(contentSize.width() - documentViewPoint.x());


            if (m_scrollDistance.y() < 0)
                scrollableDistance.setY(viewStartPoint.y());
             else
                scrollableDistance.setY(contentSize.height() - documentViewPoint.y());

            if (abs(m_scrollDistance.x()) > abs(scrollableDistance.x())){
                //m_scrollDistance.x() >= 0 means scroll from right to left
                if (m_scrollDistance.x() >= 0)
                    m_focusedBlockPt.setX(m_focusedBlockPt.x() - abs(scrollableDistance.x()));
                //m_scrollDistance.x() < 0 means scroll from left to right
                else
                    m_focusedBlockPt.setX(m_focusedBlockPt.x() + abs(scrollableDistance.x()));
             } else
                m_focusedBlockPt.setX(docFocusPoint.x());

            if (abs(m_scrollDistance.y()) > abs(scrollableDistance.y())){
                if (m_scrollDistance.y() >= 0)
                    m_focusedBlockPt.setY(m_focusedBlockPt.y() - abs(scrollableDistance.y()));
                else
                    m_focusedBlockPt.setY(m_focusedBlockPt.y() + abs(scrollableDistance.y()));
            }
            else
                m_focusedBlockPt.setY(docFocusPoint.y());

            m_focusedBlockPt = m_focusedBlockPt - m_frame->scrollPosition();
            startScrollTimer();
        }
    setViewBlockElement(block);
    }
}

void GWebTouchNavigation::contextMenuEvent() {
    m_isContextEvent = true;
    cancelPressEvent();
}


/*!
    Returns the next scrollable frame in the frame tree give the x,y position
*/
QWebFrame* GWebTouchNavigation::getNextScrollableFrame(const QPoint& pos)
{
    QWebFrame* frame = m_frame;
    while (frame) {
        if (pos.x() > 0) {
            if (frame->scrollBarValue(Qt::Horizontal) < frame->scrollBarMaximum(Qt::Horizontal))
                break;
        }
        else if (pos.x() < 0) {
            if (frame->scrollBarValue(Qt::Horizontal) > frame->scrollBarMinimum(Qt::Horizontal))
                break;
        }

        if (pos.y() > 0) {
            if (frame->scrollBarValue(Qt::Vertical) < frame->scrollBarMaximum(Qt::Vertical))
                break;
        }
        else if (pos.y() < 0) {
            if (frame->scrollBarValue(Qt::Vertical) > frame->scrollBarMinimum(Qt::Vertical))
                break;
        }
        frame = frame->parentFrame();
    }
    return (frame)?frame:m_webPage->mainFrame();
}

/*
Starts the timer for scrolling smoothly to the destination location .
The timer will do the decelaration while scrolling
*/
void GWebTouchNavigation::startScrollTimer()
{
    if (!m_scrollTimer) {
        m_scrollTimer = new QTimer(this);
        connect(m_scrollTimer,SIGNAL(timeout()),this,SLOT(scrollToEdge()));
    }
    m_scrollTimer->stop();
    m_scrollTimer->start(KAnimationTimeout);
}

/*
Update the scroll distance for flick gesture. Update the scroll distance upto the edge of the page
*/
void GWebTouchNavigation::updateFlickScrollDistance()
{
    m_initialSpeed = speed();
    m_initialSpeed.setX(qAbs(m_initialSpeed.x()));
    m_initialSpeed.setY(qAbs(m_initialSpeed.y()));
    m_flickDirection = findDirectionWithAngle(m_dragPoints.first().iPoint - m_dragPoints.last().iPoint);
    m_scrollDistance.setX((m_initialSpeed.x() * m_initialSpeed.x())/( 2 * KDeccelaration));
    m_scrollDistance.setY((m_initialSpeed.y() * m_initialSpeed.y())/( 2 * KDeccelaration));

    m_scrollDistance.setX(m_view->zoomFactor() * m_scrollDistance.x() / viewInitialScale());
    m_scrollDistance.setY(m_view->zoomFactor() * m_scrollDistance.y() / viewInitialScale());

    QSize viewSize = m_webPage->viewportSize();
    QSize contentSize = m_frame->contentsSize();
    QPoint documentViewPoint = QPoint(viewSize.width(),viewSize.height()) + m_frame->scrollPosition();
    QPoint documentTouchPoint = m_touchPosition + m_frame->scrollPosition();;

    switch (m_flickDirection)
    {
        case DOWN :
                    m_scrollDistance.setX(0);
                    m_scrollDistance.setY(-(m_scrollDistance.y()));

                    if (m_scrollDistance.y() < -(documentTouchPoint.y() - m_touchPosition.y()))
                        m_scrollDistance.setY(-(documentTouchPoint.y() - m_touchPosition.y()));
                    break;
        case UP :
                    m_scrollDistance.setX(0);
                    if (m_scrollDistance.y() > (contentSize.height() - documentViewPoint.y()))
                        m_scrollDistance.setY(contentSize.height() - documentViewPoint.y());
                    break;
        case RIGHT:
                    m_scrollDistance.setX(-(m_scrollDistance.x()));
                    if (m_scrollDistance.x() < -(documentTouchPoint.x() - m_touchPosition.x()))
                        m_scrollDistance.setX(-(documentTouchPoint.x() - m_touchPosition.x()));
                    m_scrollDistance.setY(0);
                    break;
        case LEFT:
                    m_scrollDistance.setY(0);
                    if (m_scrollDistance.x() > (contentSize.width() - documentViewPoint.x()))
                        m_scrollDistance.setX(contentSize.width() - documentViewPoint.x());
                    break;
        case BOTTOMRIGHT:
                    m_scrollDistance.setX(-(m_scrollDistance.x()));
                    m_scrollDistance.setY(-(m_scrollDistance.y()));
                    if (m_scrollDistance.x() <  -(documentTouchPoint.x()-m_touchPosition.x()))
                        m_scrollDistance.setX(-(documentTouchPoint.x()-m_touchPosition.x()));
                    if (m_scrollDistance.y() < -((documentTouchPoint.y()-m_touchPosition.y())/2))
                        m_scrollDistance.setY(-((documentTouchPoint.y()-m_touchPosition.y())/2));
                    break;
        case BOTTOMLEFT:
                    m_scrollDistance.setY(-(m_scrollDistance.y()));
                    if (m_scrollDistance.x() > contentSize.width()-documentViewPoint.x())
                        m_scrollDistance.setX(contentSize.width()-documentViewPoint.x());
                    if (m_scrollDistance.y() < (-((documentTouchPoint.y()-m_touchPosition.y())/2)))
                        m_scrollDistance.setY(-((documentTouchPoint.y()-m_touchPosition.y())/2));
                    break;
        case TOPLEFT:
                    if (m_scrollDistance.x() > (contentSize.width()-documentViewPoint.x()))
                        m_scrollDistance.setX(contentSize.width()-documentViewPoint.x());
                    if (m_scrollDistance.y() > ((contentSize.height()-documentViewPoint.y())/2))
                        m_scrollDistance.setY((contentSize.height()-documentViewPoint.y())/2);
                    break;
        case TOPRIGHT:
                    m_scrollDistance.setX(-(m_scrollDistance.x()));
                    if (m_scrollDistance.x() < (-(documentTouchPoint.x()-m_touchPosition.x())))
                        m_scrollDistance.setX(-(documentTouchPoint.x()-m_touchPosition.x()));
                    if (m_scrollDistance.y() > ((contentSize.height()-documentViewPoint.y())/2))
                        m_scrollDistance.setY((contentSize.height()-documentViewPoint.y())/2);
                    break;
        default: break;
    }
}

/*
Distinguishes b/w swipe and flick
*/
bool GWebTouchNavigation::isFlick()
{
    bool flick = false;
    QPoint moveSpeed = speed();
    int xSpeed = moveSpeed.x();
    int ySpeed = moveSpeed.y();

    flick = (qAbs(xSpeed) > KFlickSpeed ||
             qAbs(ySpeed) > KFlickSpeed);

    return flick;
}

/*
Calculates the speed of te scroll along x-axis and y-axis
*/
QPoint GWebTouchNavigation::speed()
{
    // Speed is only evaluated at the end of the swipe
    QPoint dragSpeed(0,0);
    qreal time =  dragTime() / KMilliSecond;
    if (time > 0) {
        QPoint distance = currentPos() - previousPos();
        dragSpeed.setX((distance.x()) / time);
        dragSpeed.setY((distance.y()) / time);
    }
    return dragSpeed;
}

/*
Returns the last point in the m_dragPoints list
*/
QPoint GWebTouchNavigation::currentPos()
{
    return m_dragPoints[m_dragPoints.size()-1].iPoint;
}

/*
Returns the first point in the m_dragPoints list
*/
QPoint GWebTouchNavigation::previousPos()
{
    return m_dragPoints[0].iPoint;
}

/*
Finds the time difference b/w the first and last dragpoint
*/
qreal GWebTouchNavigation::dragTime() const
{
    if (m_dragPoints.isEmpty())
        return 0.0;
    else
        return  m_dragPoints[0].iTime.msecsTo(m_dragPoints[m_dragPoints.size()-1].iTime);
}

/*!
Find the flick direction with respect to angle of flick
*/
GWebTouchNavigation::Direction GWebTouchNavigation::findDirectionWithAngle(const QPoint& diff)
{
    Direction direction = NONE;
    int xDelta = -diff.x();
    int yDelta = -diff.y();

    qreal angle = findAngle(xDelta, yDelta);
    if (isNear(angle, 60.0, 120.0)) {
         direction = DOWN;
     }
    else if (isNear(angle, 150.0, 210.0)) {
        direction = LEFT;
    }
    else if (isNear(angle, 240.0, 300.0)) {
        direction = UP;
    }
    else if (360.0 - KAngleTolerance <= angle || angle <= KAngleTolerance) {
        direction = RIGHT;
    }
    else if (isNear(angle, 30.0, 60.0)) {
        direction = BOTTOMRIGHT;
    }
    else if (isNear(angle, 120.0, 150.0)) {
        direction = BOTTOMLEFT;
    }
    else if (isNear(angle, 210.0, 240.0)) {
        direction = TOPLEFT;
    }
    else if (isNear(angle, 300.0, 330.0)) {
        direction = TOPRIGHT;
    }
    Q_ASSERT(direction != NONE);

    return direction;
}

/*
Check the angle is in the range of aMinAngle and aMaxAngle
*/
bool GWebTouchNavigation::isNear(qreal angleUnderTest, qreal minAngle, qreal maxAngle)
{
    return (minAngle < angleUnderTest) &&  (angleUnderTest <= maxAngle);
}

/*
Find the angle from x and y displacement
*/
qreal GWebTouchNavigation::findAngle(const int& xDelta,const int& yDelta)
{
    qreal angle = 0;
    qreal hypotenuse = qSqrt(xDelta*xDelta + yDelta*yDelta);

    if (hypotenuse != 0) {
        angle = qAcos(xDelta / hypotenuse);

        if (yDelta < 0) {
            angle = (2 * PI) - angle;
        }
    }

    return (angle * 180) / PI;
}

int GWebTouchNavigation::roundOff(qreal num)
{
    return (num + 0.5);
}

/*
Finds out the enclosing block
*/
QRect GWebTouchNavigation::findEnclosingBlock(QMouseEvent* ev)
{
    QWebHitTestResult htr = getHitTestResult(ev);
    QRect rect = htr.enclosingBlockElement().geometry();
    return rect;
}

/*
Gets the hitTestResult for a particular event
*/
QWebHitTestResult GWebTouchNavigation::getHitTestResult(QMouseEvent* ev)
{
    QPoint pt = ev->pos();
    QWebFrame* frame = m_webPage->frameAt(pt);
    frame = (frame) ? frame : m_webPage->mainFrame();
    QWebHitTestResult htr = frame->hitTestContent(pt);
    return htr;
}

/*
Finds out the scroll distance associated with a swipe
*/
void GWebTouchNavigation::calculateActualScrollDistance()
{
    m_actualScrollDistance.setX(0);
    m_actualScrollDistance.setY(0);
    if (!m_dragPoints.isEmpty()) {
        QPoint distance = previousPos() - currentPos();
        if (qAbs(distance.x()) > 0 || qAbs(distance.y()) > 0) {
            m_actualScrollDistance = distance / m_dragPoints.size() * 2;
        }
    }
}

/*
In the case of slow swipe, if the destination location lies within the threshold area, then
we need to align the block to te left edge of the mobile screen. This method finds out the
new scroll distance
*/
void GWebTouchNavigation::setNewScrollDistance(QPoint blockCanvasPoint, int thresholdCheckVal)
{
    m_scrollDistance.setX(m_actualScrollDistance.x());
    m_scrollDistance.setY(m_actualScrollDistance.y());
    if (blockCanvasPoint.x() > 0) {
        //Checks whether the block falls within the threshold after right to left swipe
        if ( (thresholdCheckVal <  KThresholdForRightToLeftMotion) && (thresholdCheckVal > 0)) {
            //if the block is within the threshold range already and the user tries
            //to swipe from left to right , then let it move to right. Dont try to
            //snap to the left edge .
            if (m_actualScrollDistance.x() > 0) {
                m_scrollDistance.setX(blockCanvasPoint.x() - XAlignVal);
            }
        }
    }
    else {
        //Checks whether the block falls within the threshold after left to right swipe
        if ( (thresholdCheckVal >  KThresholdForLeftToRightMotion) && (thresholdCheckVal < 0)) {
            //if the block is within the threshold range already and the user tries
            //to swipe from right to left , then let it move to left. Dont try to
            //snap to the left edge .
            if (m_actualScrollDistance.x() < 0) {
                m_scrollDistance.setX(blockCanvasPoint.x() - XAlignVal);
            }
        }

    }
}


/*
SLOT associated with the timer to adjust the scroll to the edge
*/
void GWebTouchNavigation::scrollToEdge()
{
    m_prevPoint.setX(m_scrollDistance.x());
    m_prevPoint.setY(m_scrollDistance.y());

    m_scrollDistance.setX(m_scrollDistance.x() * KDecelerationFactor);
    m_scrollDistance.setY(m_scrollDistance.y() * KDecelerationFactor);

    //round off the values
    m_scrollDistance.setX(roundOff(m_scrollDistance.x()));
    m_scrollDistance.setY(roundOff(m_scrollDistance.y()));

    int diffX = m_prevPoint.x() - m_scrollDistance.x();
    int diffY = m_prevPoint.y() - m_scrollDistance.y();

    if (((m_scrollDistance.x() == 0) && (m_scrollDistance.y() == 0))
         || ((diffX == 0) && (diffY == 0))) {
        scrollCurrentFrame(m_prevPoint.x(), m_prevPoint.y());
        m_scrollTimer->stop();
        return;
    }
    scrollCurrentFrame(diffX, diffY);
}

void GWebTouchNavigation::onLongPressTimer()
{
    stopLongPressTimer();// stop timer as soon as timeout
    emit longPressEvent(m_touchPosition);
}

void GWebTouchNavigation::startLongPressTimer()
{
    if(!m_longPressTimer) {
        m_longPressTimer = new QTimer(this);
        connect(m_longPressTimer, SIGNAL(timeout()), this, SLOT(onLongPressTimer()));
    }
    else {
        m_longPressTimer->stop();
    }
    m_longPressTimer->start(KLongPressDuration);
}

void GWebTouchNavigation::stopLongPressTimer()
{
    if (m_longPressTimer){
        m_longPressTimer->stop();
        delete m_longPressTimer;
        m_longPressTimer = 0;
    }
}

void GWebTouchNavigation::emitFocusedElementChanged()
{
    QWebHitTestResult htRes = m_frame->hitTestContent(m_touchPosition);
    wrtBrowserDefs::BrowserElementType elType = wrtBrowserUtils::getTypeFromElement(htRes);
    emit focusElementChanged(elType);
}

void GWebTouchNavigation::scrollCurrentFrame (int dx, int dy)
{
    QPoint scrollPosition = m_frame->scrollPosition();
    /* slide the url bar up if content is scrolled to top */
    if (m_frame == m_webPage->mainFrame() &&
         scrollPosition.y() <=0  && !m_isLoading && m_wantSlideViewCalls) {
         qreal slide = slideView(-dy);
         dy+=slide;
     }

     /* shrink the viewport size at bottom if content is scrolled to bottom,
        so that use can click the link under the toolbar */
    /*
     if (m_frame == m_webPage->mainFrame()) {
        if (scrollPosition.y() + m_webPage->viewportSize().height() >= m_frame->contentsSize().height()) {
            qreal slide = shrinkView(-dy);
            dy+=slide;
        }
     }
     */
     m_frame->scroll(dx, dy);

}

void GWebTouchNavigation::setCurrentFrameScrollPosition (QPoint& pos)
{
    QPoint scrollPosition = m_frame->scrollPosition();
    QPoint finalPos = pos;
    int dy = scrollPosition.y() - pos.y();

    if (m_frame == m_webPage->mainFrame() &&
        scrollPosition.y() <=0 &&
        !m_isLoading &&
        m_wantSlideViewCalls) {

          qreal slide = slideView(-dy);
          dy+=slide;
          finalPos.setY(scrollPosition.y() - dy);
    }
    /*
    if (m_frame == m_webPage->mainFrame()) {
       if (scrollPosition.y() + m_webPage->viewportSize().height() == m_frame->contentsSize().height()) {
           qreal slide = shrinkView(-dy);
           dy-=slide;
       }
       finalPos.setY(scrollPosition.y() - dy);
    }
    */
    m_frame->setScrollPosition(finalPos);
}

void GWebTouchNavigation::onLoadStarted()
{
    m_isLoading = true;
}

void GWebTouchNavigation::onLoadFinished(bool ok)
{
    Q_UNUSED(ok)
    m_isLoading = false;
}
void GWebTouchNavigation::setPage( QWebPage * page)
{
    if (m_webPage) {
        disconnect(m_webPage, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
        disconnect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
        //disconnect(m_webPage->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(onContentsSizeChanged(const QSize &)));
    }
    m_webPage = page;
    if (m_webPage) {
        connect(m_webPage, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
        connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
        //connect(m_webPage->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(onContentsSizeChanged(const QSize &)));
    }
}

void GWebTouchNavigation::onContentsSizeChanged(const QSize & size)
{
    Q_UNUSED(size)
    /* get the new content height, and resize the viewport size */
    if (m_webPage->mainFrame()->contentsSize().height() != m_contentHeight) {
        m_contentHeight = m_webPage->mainFrame()->contentsSize().height();
        shrinkView(100);
    }
}

void GWebTouchNavigation::enableDClick(bool aValue)
{
    m_doubleClickEnabled = aValue;
}
}
