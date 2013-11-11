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

#include "ScrollableWebContentView.h"

#include "Gestures/GestureRecognizer.h"
#include "Kinetics/KineticScroller.h"
#include "ScrollableViewBase.h"
#include "ViewportMetaDataParser.h"
#include "WebContentAnimationItem.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWebView>
#include <QWebElement>
#include <QWebHitTestResult>

//Kinetic scroll constants
static const int ScrollsPerSecond = 30;
static const int MinimumScrollVelocity = 10;
static const qreal AxisLockThreshold = .8;


//Zooming constants
static const int ZoomAnimationDuration = 300;   //ms. Zooming transition duration
static const qreal ZoomStep = .5;               //Incremental zoom step
const int TileUpdateEnableDelay = 500;         //Wait duration before tiling updates are enabled.

namespace GVA {

ScrollableWebContentView::ScrollableWebContentView(WebContentAnimationItem* webAnimationItem, QGraphicsItem* parent)
    : ScrollableViewBase(parent)
    , m_gestureRecognizer(this)
{
    m_viewportMetaData = new ViewportMetaData();

    //Kinetic scroller settings
    //Sets the number of scrolls (frames) per second to sps.
    m_kineticScroller->setScrollsPerSecond(ScrollsPerSecond);
    //For elastic scroll in page edges
    m_kineticScroller->setOvershootPolicy(KineticScroller::OvershootWhenScrollable);

    //Gesture settings
    //For detecting scroll direction
    m_gestureRecognizer.setAxisLockThreshold(AxisLockThreshold);
    //To enable touch and drag scrolling
    m_gestureRecognizer.setMinimumVelocity(MinimumScrollVelocity);

    setWidget(webAnimationItem);
    //FIX ME : Revisit this code. Duplicate info sharing!
    webAnimationItem->setViewportMetaData(m_viewportMetaData);


    m_tileUpdateEnableTimer.setSingleShot(true);
    connect(&m_tileUpdateEnableTimer, SIGNAL(timeout()), webAnimationItem, SLOT(enableContentUpdates()));

    //Setup zooming animator
    m_zoomAnimator = new QPropertyAnimation(webAnimationItem, "geometry");
    m_zoomAnimator->setDuration(ZoomAnimationDuration);
    connect(m_zoomAnimator, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)), this, SLOT(zoomAnimationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    
    m_gesturesEnabled = true;
}

ScrollableWebContentView::~ScrollableWebContentView()
{
    delete m_viewportMetaData;
    delete m_kineticScroller;

    if(m_zoomAnimator) {
        m_zoomAnimator->stop();
        delete m_zoomAnimator;
    }
}

WebContentAnimationItem* ScrollableWebContentView::viewportWidget() const
{
    return qobject_cast<WebContentAnimationItem*>(scrollWidget());
}

void ScrollableWebContentView::zoomToScreenCenter(bool zoomIn)
{
    //If viewport metadata has user scalable false.
    //Do not zoom.
    if (!m_viewportMetaData->m_userScalable)
        return;

    qreal scale = 1;
    scale += ZoomStep;

    if (!zoomIn)
        scale = 1/scale;

    qreal curScale =  viewportWidget()->zoomScale();

    if (zoomIn && (curScale * scale > m_viewportMetaData->m_maximumScale))
        scale = m_viewportMetaData->m_maximumScale / curScale;
    else if (!zoomIn && (curScale * scale < m_viewportMetaData->m_minimumScale))
        scale = m_viewportMetaData->m_minimumScale / curScale;

    if(scale == 1.)
        return;

    //Screen center
    QPointF scrCenter(size().width()/2, size().height()/2);
    //Map screen center to document
    QPointF docPoint(viewportWidget()->mapFromScene(scrCenter));
    //Maintain that spot in the same point on the viewport
    QPointF docPointInScr(viewportWidget()->mapToParent(docPoint));
    startZoomAnimToItemHotspot(docPoint, docPointInScr, scale);
}

ZoomMetaData ScrollableWebContentView::currentPageInfo()
{
    ZoomMetaData data;

    data.initialScale = m_viewportMetaData->m_initialScale;
    data.minScale = m_viewportMetaData->m_minimumScale;
    data.maxScale = m_viewportMetaData->m_maximumScale;
    data.userScalable = m_viewportMetaData->m_userScalable;
    data.m_specifiedWidth = m_viewportMetaData->m_specifiedData.m_width;
    data.m_specifiedHeight= m_viewportMetaData->m_specifiedData.m_height;

    data.rect = viewportWidget()->geometry();
    data.scale = viewportWidget()->zoomScale();
    data.webViewSize = viewportWidget()->webView()->geometry();
    data.viewportSize = size();

    return data;
}

void ScrollableWebContentView::setCurrentPageInfo(ZoomMetaData data)
{
    m_viewportMetaData->m_initialScale = data.initialScale;
    m_viewportMetaData->m_minimumScale = data.minScale;
    m_viewportMetaData->m_maximumScale = data.maxScale;
    m_viewportMetaData->m_userScalable = data.userScalable;
    m_viewportMetaData->m_specifiedData.m_width = data.m_specifiedWidth;
    m_viewportMetaData->m_specifiedData.m_height = data.m_specifiedHeight;
    m_viewportMetaData->m_isValid = true;

    m_viewportMetaData->m_width = data.webViewSize.width();
    m_viewportMetaData->m_height = data.webViewSize.height();

    viewportWidget()->webView()->setGeometry(data.webViewSize);
    viewportWidget()->setZoomScale(data.scale, true);
    viewportWidget()->setGeometry(data.rect);

    if (data.viewportSize.width() != size().width())
        adjustViewportSize(data.viewportSize, size());
}

ZoomMetaData ScrollableWebContentView::defaultZoomData()
{
    ZoomMetaData data;

    data.initialScale = m_viewportMetaData->m_initialScale;
    data.minScale = m_viewportMetaData->m_minimumScale;
    data.maxScale = m_viewportMetaData->m_maximumScale;
    data.userScalable = m_viewportMetaData->m_userScalable;

    data.scale = 1.0;
    data.rect = QRectF();
    data.webViewSize = QRectF();
    data.viewportSize = QSizeF();

    return data;
}

void ScrollableWebContentView::updatePreferredContentSize()
{
    viewportWidget()->updatePreferredContentSize(QSize(m_viewportMetaData->m_width
                                                       , m_viewportMetaData->m_height));
}

void ScrollableWebContentView::setSuperPage()
{
    m_viewportMetaData->m_initialScale = 1.;
    m_viewportMetaData->m_minimumScale = 1.;
    m_viewportMetaData->m_maximumScale = 1.;
    m_viewportMetaData->m_specifiedData.m_width = "device-width";
    m_viewportMetaData->m_specifiedData.m_height = "device-height";
    m_viewportMetaData->m_userScalable = false;

    QSize contentSize = viewportWidget()->contentsSize();
    QRect webViewRect(0, 0, size().width(), contentSize.height());
    viewportWidget()->webView()->setGeometry(webViewRect);
    viewportWidget()->setZoomScale(1., true);
    viewportWidget()->setGeometry(webViewRect);

    m_viewportMetaData->m_width = size().width();
    m_viewportMetaData->m_height = size().height();
    m_viewportMetaData->m_isValid = true;

    updatePreferredContentSize();
}

void ScrollableWebContentView::reset()
{
    // TODO: INVESTIGATE: In the case of multiple windows loading pages simultaneously, it is possible
    // to be calling this slot on a signal from a frame that is not
    // the frame of the page saved here. It might be better to use 'sender' instead of
    // page->mainFrame() to get the metaData so that we use the meta data of the corresponding
    // frame

    QWebPage* page = viewportWidget()->webView()->page();
    if (!page)
        return;

    //Initialize viewport metadata
    m_viewportMetaData->reset();

    QWebFrame* frame = page->mainFrame();
    QMap<QString, QString> metaData = frame->metaData();
    QString viewportTag = metaData.value("viewport");

    QRect clientRect = geometry().toAlignedRect();
    ViewportMetaDataParser parser(clientRect);
    *m_viewportMetaData = parser.parse(viewportTag);

    updatePreferredContentSize();
    setViewportWidgetGeometry(QRectF(QPointF(),
                                     QSize(m_viewportMetaData->m_width, m_viewportMetaData->m_height)
                                     * m_viewportMetaData->m_initialScale));
}

void ScrollableWebContentView::contentsSizeChanged(const QSize& newContentSize)
{
    QRect clientRect = geometry().toAlignedRect();
    m_viewportMetaData->updateViewportData(newContentSize, clientRect);
    viewportWidget()->resize(QSize(m_viewportMetaData->m_width, m_viewportMetaData->m_height)
                             * m_viewportMetaData->m_initialScale);
}

void ScrollableWebContentView::pageLoadFinished(bool ok)
{
    Q_UNUSED(ok);
    QSize contentSize = viewportWidget()->contentsSize();
    QRect clientRect = geometry().toAlignedRect();
    m_viewportMetaData->updateViewportData(contentSize, clientRect);

    viewportWidget()->resize(QSize(m_viewportMetaData->m_width, m_viewportMetaData->m_height)
                             * m_viewportMetaData->m_initialScale);
    viewportWidget()->setZoomScale(m_viewportMetaData->m_initialScale, true);
}

bool ScrollableWebContentView::sceneEventFilter(QGraphicsItem* item, QEvent* event)
{
    Q_UNUSED(item);

    bool handled = false;
    
    if (!isVisible() || !m_gesturesEnabled) {
        if (event->type() == QEvent::GraphicsSceneContextMenu)
            return true;
        else
            return handled;
    }

    //Pass all events to recognizer
    handled  = m_gestureRecognizer.mouseEventFilter(static_cast<QGraphicsSceneMouseEvent *>(event));
    return handled;
}

void ScrollableWebContentView::handleGesture(GestureEvent* gestureEvent)
{
    switch (gestureEvent->type()) {
    case GestureEvent::Touch:
        handlePress(gestureEvent);
        break;
    case GestureEvent::Release:
        handleRelease(gestureEvent);
        break;
    case GestureEvent::Pan:
        handlePan(gestureEvent);
        break;
    case GestureEvent::Flick:
        handleFlick(gestureEvent);
        break;
    case GestureEvent::DoubleTap:
        handleDoubleTap(gestureEvent);
        break;
    case GestureEvent::LongTap:
        handleLongTap(gestureEvent);
        break;
    default:
        break;
    }

}

void ScrollableWebContentView::handlePress(GestureEvent* gestureEvent)
{
    m_kineticScroller->stop();
    QPointF pos = gestureEvent->position();
    sendEventToWebKit(QEvent::GraphicsSceneMousePress, pos);
}

void ScrollableWebContentView::handleRelease(GestureEvent* gestureEvent)
{
    //FIX ME:
    emit mouseEvent(QEvent::GraphicsSceneMousePress);
    //Cache release event to send on release
    QPointF pos = gestureEvent->position();
    sendEventToWebKit(QEvent::GraphicsSceneMouseRelease, pos);
    emit mouseEvent(QEvent::GraphicsSceneMouseRelease);
}

void ScrollableWebContentView::handleDoubleTap(GestureEvent* gestureEvent)
{
    if (!m_viewportMetaData->m_userScalable)
        return;

    QRectF target;
    WebContentAnimationItem* webViewProxy = viewportWidget();

    // Contentview center is the focus hotspot
    QPointF viewTargetHotspot(size().width() / 2, size().height() / 2);

    //Get the focussable element rect from current touch position
    QPointF touchPoint = webViewProxy->mapFromScene(gestureEvent->position());
    QRectF zoomRect = webViewProxy->findZoomableRectForPoint(touchPoint);

    if (!zoomRect.isValid()) {
        //FIX ME: Add an event ignore animation
        return;
    }

    // target is the center of the identified rect x-wise
    // y-wise it's the place user touched
    QPointF hotspot(zoomRect.center().x(), touchPoint.y());
    qreal scale = size().width() / zoomRect.size().width();
    startZoomAnimToItemHotspot(hotspot, viewTargetHotspot, scale, zoomRect);
}

void ScrollableWebContentView::handlePan(GestureEvent* gestureEvent)
{
    QPoint scrollPos = ScrollableViewBase::scrollPosition();
    m_kineticScroller->doPan(gestureEvent->delta());
    QPoint delta;
    delta.setX(-gestureEvent->delta().x());
    delta.setY(-gestureEvent->delta().y());
    emit viewScrolled(scrollPos, delta);
}

void ScrollableWebContentView::handleFlick(GestureEvent* gestureEvent)
{
    QPoint scrollPos = ScrollableViewBase::scrollPosition();
    m_kineticScroller->doFlick(gestureEvent->velocity());
}

void ScrollableWebContentView::handleLongTap(GestureEvent* gestureEvent)
{
    QWebPage* page = viewportWidget()->webView()->page();
    QPointF contextPt = viewportWidget()->webView()->mapFromScene(gestureEvent->position());
    QWebHitTestResult result = page->currentFrame()->hitTestContent(contextPt.toPoint());

    //Notify context menu observers
    emit contextEventObject(&result);
}

void ScrollableWebContentView::setViewportWidgetGeometry(const QRectF& r)
{
    ScrollableViewBase::setScrollWidgetGeometry(r);
}

void ScrollableWebContentView::startZoomAnimToItemHotspot(const QPointF& hotspot, const QPointF& viewTargetHotspot, qreal scale,  QRectF target)
{
    WebContentAnimationItem* animWidget = viewportWidget();

    QPointF newHotspot = hotspot * scale;
    QPointF newViewportOrigon = newHotspot - viewTargetHotspot;
    QRectF zoomedRect(-newViewportOrigon, animWidget->size() * scale);

    QRectF temp = adjustScrollWidgetRect(zoomedRect);
    qreal diff = qAbs(scrollWidget()->geometry().y() - temp.y());

    //FIX ME : Seperate the logic for centerzoom and block-focus zoom
    if (qFuzzyCompare(scrollWidget()->geometry().topLeft().x(), temp.topLeft().x())
        && qFuzzyCompare(scrollWidget()->geometry().width(), temp.width())
        && qFuzzyCompare(scrollWidget()->geometry().height(), temp.height())
        && !target.isEmpty() && (diff <= target.height())) {

            scale = size().width() / animWidget->size().width();
            newHotspot = QPointF(0, -animWidget->pos().y()) * scale;
            newViewportOrigon = newHotspot - viewTargetHotspot;
            zoomedRect = QRectF(-newViewportOrigon, animWidget->size() * scale);
    }

    startZoomAnimation(zoomedRect);
}

bool ScrollableWebContentView::isZoomedIn() const
{
    return size().width() < viewportWidget()->size().width();
}

void ScrollableWebContentView::stateChanged(KineticScrollable::State oldState
                                            , KineticScrollable::State newState)
{
    ScrollableViewBase::stateChanged(oldState, newState);

    if (newState == KineticScrollable::Pushing) {
        m_tileUpdateEnableTimer.stop();
        viewportWidget()->disableContentUpdates();
    }
    else if (newState == KineticScrollable::AutoScrolling) {
        m_tileUpdateEnableTimer.stop();
        viewportWidget()->disableContentUpdates();
    }
    else if (newState == KineticScrollable::Inactive) {
        m_tileUpdateEnableTimer.start(TileUpdateEnableDelay);
    }
}

void ScrollableWebContentView::startZoomAnimation(const QRectF& destRect)
{
    QAbstractAnimation::State animState = m_zoomAnimator->state();
    if (animState == QAbstractAnimation::Running)
        return;

    m_zoomAnimator->setStartValue(scrollWidget()->geometry());
    m_animationEndRect = adjustScrollWidgetRect(destRect);
    m_zoomAnimator->setEndValue(m_animationEndRect);
    m_zoomAnimator->start();
}

void ScrollableWebContentView::stopZoomAnimation()
{
    m_animationEndRect = QRectF();
    m_zoomAnimator->stop();
}

void ScrollableWebContentView::updateZoomEndRect()
{
    if (m_animationEndRect.isValid())
        scrollWidget()->setGeometry(m_animationEndRect);
}

void ScrollableWebContentView::zoomAnimationStateChanged(QAbstractAnimation::State newState,QAbstractAnimation::State)
{
    switch (newState) {
    case QAbstractAnimation::Stopped:
        updateZoomEndRect();
        break;
    default:
        break;
    }
}

void ScrollableWebContentView::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    QGraphicsWidget::resizeEvent(event);

    //Ignore resize when chrome is being still setup
    if (!event->oldSize().width())
        return;

    adjustViewportSize(event->oldSize(), event->newSize());
}

void ScrollableWebContentView::adjustViewportSize(QSizeF oldSize, QSizeF newSize)
{
    //FIX ME : Check this
    if (m_viewportMetaData->m_isValid) {

        QRect clientRect = geometry().toAlignedRect();
        if (m_viewportMetaData->isLayoutNeeded())  {
            m_viewportMetaData->orientationChanged(clientRect);
            updatePreferredContentSize();
            return;
        } else
            m_viewportMetaData->updateViewportData(viewportWidget()->contentsSize(), clientRect);
    }

    qreal scale = newSize.width() / oldSize.width();
    QPointF middleLeft(0, oldSize.height()/2);
    QPointF docPoint(viewportWidget()->mapFromScene(middleLeft));

    QPointF resizedMiddleLeft(0, newSize.height()/2);
    QPointF resizedDocPoint(viewportWidget()->mapFromScene(resizedMiddleLeft));
    QPointF docPointInScr(viewportWidget()->mapToParent(resizedDocPoint));

    //FIX ME : Should be handled with only following function call
    //Since its not working, work-around is added. Plz fix it
    //startZoomAnimToItemHotspot(docPoint, docPointInScr, scale);

    QPointF newHotspot = docPoint * scale;
    QPointF newViewportOrigon = newHotspot - docPointInScr;
    QRectF zoomedRect(-newViewportOrigon,  viewportWidget()->size() * scale);
    QRectF adjustRect = adjustScrollWidgetRect(zoomedRect);

    setScrollWidgetGeometry(zoomedRect);
}

void ScrollableWebContentView::sendEventToWebKit(QEvent::Type type, QPointF& scenPos)
{
    //Setup event and send it to webkit
    QGraphicsSceneMouseEvent event(type);
    event.setScenePos(scenPos);
    event.setPos(viewportWidget()->webView()->mapFromScene(event.scenePos()));
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    event.setModifiers(Qt::NoModifier);
    viewportWidget()->webView()->page()->event(&event);
}

} //namespace GVA
