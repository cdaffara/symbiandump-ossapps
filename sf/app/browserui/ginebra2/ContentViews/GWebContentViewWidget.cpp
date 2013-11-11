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
#include "GWebContentViewWidget.h"
#include "GWebContentView.h"
#include "browserpagefactory.h"
#include "webpagecontroller.h"
#include "Utilities.h"
#include "ChromeEffect.h"

#include <QGraphicsSceneResizeEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QWebFrame>
#include <QWebHitTestResult>
#include <QPoint>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsView>

#ifdef ENABLE_PERF_TRACE
#include "wrtperftracer.h"
#endif

namespace GVA {

const QString KViewPortWidthTag("width");
const QString KViewPortHeightTag("height");
const QString KViewPortInitialScaleTag("initial-scale");
const QString KViewPortMinScaleTag("minimum-scale");
const QString KViewPortMaxScaleTag("maximum-scale");
const QString KViewPortUserScalableTag("user-scalable");
const QString KViewPortDeviceWidthTag("device-width");
const QString KViewPortDeviceHeightTag("device-height");


const int KDefaultViewportWidth = 980;
const int KDefaultPortraitScaleWidth = 540;
const int KMinViewportWidth = 200;
const int KMaxViewportWidth = 10000;
const int KMinViewportHeight = 200;
const int KMaxViewportHeight = 10000;
const int KMaxPageZoom = 10;
const qreal KDefaultMinScale = 0.25;
const qreal KDefaultMaxScale = 10.00;
const qreal KInitialZoomFactorValue = 0.653061;
const QPoint KFocussPoint(5, 50);
const int checkerSize = 16;
const unsigned checkerColor1 = 0xff555555;
const unsigned checkerColor2 = 0xffaaaaaa;

GWebContentViewWidget::GWebContentViewWidget(QObject* parent, GWebContentView* view,QWebPage* pg) : QGraphicsWebView(0)
, m_webContentView(view)
, m_dirtyZoomFactor(1)
, m_frozenPixmap(0)
, m_frozenCount(0)
, m_wrtPage(0)
, m_bitmapZoom(false)
, m_isResize(false)
, m_currentinitialScale(0)
, m_previousViewPortwidth(size().toSize().width())
, m_bitmapImage(NULL)
, m_checkeredBoxPixmap(NULL)
{
   setParent(parent);
  if ( pg ) {
    setPage(pg);
  }
#ifdef ENABLE_PERF_TRACE
  WrtPerfTracer::tracer()->initPage(pg);
#endif //ENABLE_PERF_TRACE

  m_currentinitialScale = zoomFactor();

#ifndef NO_QSTM_GESTURE
  m_touchNavigation = new WebTouchNavigation(this);
#endif
  // Fixes missing radio button problem with certain themes
  QPalette pal = palette();
  pal.setColor(QPalette::ButtonText,Qt::black);
  setPalette(pal);
}

GWebContentViewWidget::~GWebContentViewWidget()
{
    delete m_bitmapImage;
    delete m_checkeredBoxPixmap;
    delete m_frozenPixmap;
}

void GWebContentViewWidget::updateViewport()
{
    if (page() && size() != page()->viewportSize()) {
        page()->setViewportSize(size().toSize());
    }
    setViewportSize();
}

void GWebContentViewWidget::setBlockElement(const QWebElement &pt)
{
  m_BlockElement = pt;
}

void GWebContentViewWidget::setCheckeredPixmap()
{
    delete m_checkeredBoxPixmap;
    m_checkeredBoxPixmap = NULL;
    m_checkeredBoxPixmap = new QPixmap(checkerSize, checkerSize);
    QPainter painter(m_checkeredBoxPixmap);
    painter.fillRect(0, 0, checkerSize, checkerSize, QColor(checkerColor1));
    painter.fillRect(0, checkerSize/2, checkerSize/2, checkerSize/2, QColor(checkerColor2));
    painter.fillRect(checkerSize/2, 0, checkerSize/2, checkerSize/2, QColor(checkerColor2));
}

void GWebContentViewWidget::createPageSnapShot()
{
    bitmapZoomCleanup();
    QRegion clipRegion;
    QWebFrame *frame = page()->mainFrame();
    m_bitmapImage = new QImage(size().width() ,size().height(),QImage::Format_RGB32);
    clipRegion = QRect(QPoint(0,0),size().toSize());
    QPainter painterImage(m_bitmapImage);
    painterImage.fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
    frame->render(&painterImage,clipRegion);
}

void GWebContentViewWidget::bitmapZoomCleanup()
{
    m_bitmapZoom = false;
    if (m_bitmapImage) {
        delete m_bitmapImage;
        m_bitmapImage = NULL;
    }
}

void GWebContentViewWidget::updateFrozenImage() {
    // Take a snapshot to be displayed while in frozen state.

    QStyleOptionGraphicsItem option;
    option.levelOfDetail = 1.0;
    option.exposedRect = QRectF(QPointF(0,0), size());

    // Discard existing pixmap.
    delete m_frozenPixmap;
    m_frozenPixmap = 0;

    // Paint the window into a new pixmap (m_frozenPixmap must be 0 here for this to work).
    QPixmap *frozenPixmap = new QPixmap(option.exposedRect.size().toSize());
    QPainter painter(frozenPixmap);
    paint(&painter, &option);
    painter.end();

    // Now point m_frozenPixmap at the new pixmap.
    m_frozenPixmap = frozenPixmap;
}

QImage GWebContentViewWidget::getPageSnapshot()
{
  QImage img(size().toSize(), QImage::Format_RGB32);

  if (!page()) return QImage();

  QPainter painter(&img);
  QWebFrame *frame = page()->mainFrame();

  painter.fillRect(QRectF(0, 0, size().width(), size().height()), QColor(255, 255, 255));
//    QTransform transform;
//    transform.scale(d->m_pageZoomFactor, d->m_pageZoomFactor);
//    painter.translate(-transform.map(frame->scrollPosition()));

  QRegion clipRegion(QRect(QPoint(0,0),size().toSize()));
//    QTransform invert = transform.inverted();
//    clipRegion = invert.map(clipRegion);
//    clipRegion.translate(frame->scrollPosition());

//    painter.scale(d->m_pageZoomFactor, d->m_pageZoomFactor);
//    d->m_webPage->mainFrame()->renderContents(&painter, clipRegion);
  frame->render(&painter, clipRegion);

  return img;
}

void GWebContentViewWidget::updateViewportSize(::QGraphicsSceneResizeEvent* e)
{
    //if there is change in mode (like landscape, potraite relayout the content)
    if (e->newSize().width() == e->oldSize().width())
        return;

    //if page is empty do not update 
    if (page()->currentFrame()->url().isEmpty())
        return;

    m_isResize = true;
    setViewportSize();
    m_isResize = false;
}

bool GWebContentViewWidget::event(QEvent * e) {

    if (e->type() == QEvent::Gesture) {
#ifndef NO_QSTM_GESTURE
          QStm_Gesture* gesture = getQStmGesture(e, WebGestureHelper::getAssignedGestureType());
          if (gesture) {
              m_touchNavigation->handleQStmGesture(gesture);
              return true;
          }
#endif
    }

    else if (e->type() == WebPageControllerUpdateViewPortEvent::staticType()) {
        updateViewport();
    }
    return QGraphicsWebView::event(e);
}

void GWebContentViewWidget::resizeEvent(QGraphicsSceneResizeEvent* e)
{
  // set the fixed text layout size for text wrapping
#if defined CWRTINTERNALWEBKIT
  if (page()) {
    p->m_webPage->settings()->setMaximumTextColumnWidth(e->newSize().width() - 6);
  }
#endif

  m_previousViewPortwidth = page()->viewportSize().width();

  const QSize &s = e->newSize().toSize();
  if (page() && s != page()->viewportSize()) {
    if (m_BlockElement.isNull()) {
      QPoint pos = QPoint(0,0);
      QWebFrame* frame = page()->frameAt(pos);
      frame = (frame) ? frame : page()->currentFrame();
      QWebHitTestResult htr = frame->hitTestContent(pos);
      m_BlockInFocus = htr.element();

      if (m_BlockInFocus.tagName() != "IMG")
        m_BlockInFocus = htr.enclosingBlockElement();

      QPoint position = m_BlockInFocus.geometry().topLeft() - page()->currentFrame()->scrollPosition();
      m_Ratiox = (qreal) position.x() / m_BlockInFocus.geometry().width();
      m_Ratioy = (qreal) position.y() / m_BlockInFocus.geometry().height();
    }
    page()->setViewportSize(s);
  }

  updateViewportSize(e);

  if(frozen()) {
    updateFrozenImage();
//    if(m_frozenPixmap && (m_frozenPixmap->size() != this->size())) {
//        QTimer::singleShot(50, this, SLOT(updateFrozenImage()));
//    }
  }
}

void GWebContentViewWidget::onLongPressEvent(QPoint pos) {
    QWebHitTestResult hitTest = page()->currentFrame()->hitTestContent(pos);
    //qDebug() << "GWebContentViewWidget::contextMenuEvent:"
    //        << "\n\t pos=" << hitTest.pos()
    //        << "\n\t linkUrl=" << hitTest.linkUrl()
    //        << "\n\t imageUrl=" << hitTest.imageUrl();

    ::WebViewEventContext *context =
            new ::WebViewEventContext(view()->type(), hitTest);

    if (m_webContentView && m_webContentView->currentPageIsSuperPage()) {
        // Let the superpage handle the event.
        m_webContentView->currentSuperPage()->onContextEvent(context);
    }
    else {
        // Send the event directly.
        emit contextEvent(context);
    }
}

void GWebContentViewWidget::contextMenuEvent(::QGraphicsSceneContextMenuEvent *event) {
    // Ignore.  The touch navigation code handles long presses.
    event->accept();
}

void GWebContentViewWidget::setZoomFactor(qreal zoom)
{
    this->setFocus();
    if (!m_userScalable)
        return;

    setPageZoomFactor(zoom);
}

void GWebContentViewWidget::setPageZoomFactor(qreal zoom)
{
//not zooming if it's bookmark or historyview or not a page
  if (!page() ||
	  	(m_webContentView->type() == "webView" && 
          m_webContentView->currentPageIsSuperPage()) )
      {
      return;      
      }

  //qDebug() << __func__ << "Zoom " << zoom << "Max : " << m_maximumScale << "Min: " << m_minimumScale;

  if (zoom < m_minimumScale)
      zoom = m_minimumScale;
  else if (zoom > m_maximumScale)
      zoom = m_maximumScale;

  QPoint pos = QPoint(0,0);

  if (!m_isResize) {
    QWebFrame* frame = page()->frameAt(pos);
    frame = (frame) ? frame : page()->currentFrame();
    QWebHitTestResult htr = frame->hitTestContent(pos);
    m_BlockInFocus = htr.element();

    if (m_BlockInFocus.tagName() != "IMG")
      m_BlockInFocus = htr.enclosingBlockElement();

    QPoint position = m_BlockInFocus.geometry().topLeft() - page()->currentFrame()->scrollPosition();
    m_Ratiox = (qreal) position.x() / m_BlockInFocus.geometry().width();
    m_Ratioy = (qreal) position.y() / m_BlockInFocus.geometry().height();
  }

  if ( m_dirtyZoomFactor != zoom ) {
      m_dirtyZoomFactor = zoom;
      #if QT_VERSION < 0x040600
		  page()->setFixedContentsSize(QSize(m_viewportWidth, m_viewportHeight/zoom));
	  #else
          if(!m_webContentView->currentPageIsSuperPage()) 
            page()->setPreferredContentsSize(QSize((int)m_viewportWidth, (int)m_viewportHeight/zoom));
	  #endif
  }

  QGraphicsWebView::setZoomFactor( zoom );

  if (!m_BlockElement.isNull() && m_isResize) {
    QPoint imageFocusPoint;
    QPoint m_focusedBlockPt = QPoint(m_BlockElement.geometry().topLeft()) - page()->mainFrame()->scrollPosition();
    if (m_BlockElement.tagName() != "IMG" && (m_BlockElement.styleProperty(QString("background-image"),QWebElement::InlineStyle) == ""))
      page()->mainFrame()->scroll(m_focusedBlockPt.x() - KFocussPoint.x() , m_focusedBlockPt.y() - KFocussPoint.y());
    else {
      if ((page()->viewportSize().width() - m_BlockElement.geometry().width()) > 0)
        imageFocusPoint.setX((page()->viewportSize().width() - m_BlockElement.geometry().width())/2);
      else
        imageFocusPoint.setX(0);

      if ((page()->viewportSize().height() - m_BlockElement.geometry().height()) > 0)
        imageFocusPoint.setY((page()->viewportSize().height() - m_BlockElement.geometry().height())/2);
      else
        imageFocusPoint.setY(0);

      page()->mainFrame()->scroll(m_focusedBlockPt.x() - imageFocusPoint.x() ,
                    m_focusedBlockPt.y() - imageFocusPoint.y());
      }
      m_focusedBlockPt = QPoint(m_BlockElement.geometry().topLeft()) - page()->mainFrame()->scrollPosition();
      emit BlockFocusChanged(m_focusedBlockPt);
  } else {
    QPoint m_focusedBlockPt = QPoint(m_BlockInFocus.geometry().topLeft()) - page()->mainFrame()->scrollPosition();
    page()->currentFrame()->scroll(m_focusedBlockPt.x() - (m_Ratiox * m_BlockInFocus.geometry().width()),
                  m_focusedBlockPt.y() - (m_Ratioy * m_BlockInFocus.geometry().height()));
    m_BlockElement = QWebElement();
  }

    m_webContentView->changeZoomAction(zoom);

}

void GWebContentViewWidget::setDirtyZoomFactor(qreal zoom)
{
    if ( m_dirtyZoomFactor == zoom )
        return;

    m_dirtyZoomFactor = zoom;

    update();
}


void GWebContentViewWidget::onLoadStarted()
{
    m_inLoading = true;
    m_loadingTime.start();
}

void GWebContentViewWidget::onLoadFinished()
{
#ifdef NO_RESIZE_ON_LOAD
    m_inLoading = false;
    update();
#endif
}

#ifdef NO_RESIZE_ON_LOAD
void GWebContentViewWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{

    QRect clipRect;
    if (options && !options->exposedRect.isEmpty())
        clipRect = options->exposedRect.toRect();
    else
        {
            clipRect = geometry().toRect();
            clipRect.moveTo(0,0);

        }
#ifdef ENABLE_PERF_TRACE
    PERF_DEBUG() << "GWebContentViewWidget::paint: " << clipRect << "\n";
    unsigned int st = WrtPerfTracer::tracer()->startTimer();
#endif

    //painter->fillRect(clipRect, QColor(255, 255, 255));
    if (!m_inLoading || !(m_loadingTime.elapsed() < 750)) {
        QGraphicsWebView::paint(painter, options, widget);
    }
#ifdef ENABLE_PERF_TRACE
    PERF_DEBUG() << "GWebContentViewWidget::paint__end: " <<
        WrtPerfTracer::tracer()->elapsedTime(st) << "\n";
#endif


}

#else //NO_RESIZE_ON_LOAD
void GWebContentViewWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
//    if (!m_active) return;

#ifdef ENABLE_PERF_TRACE
    PERF_DEBUG() << "GWebContentViewWidget::paint: " << option->exposedRect << "\n";
    unsigned int st = WrtPerfTracer::tracer()->startTimer();
#endif
    //qDebug() << "GWebContentViewWidget::paint";
    if (frozen() && m_frozenPixmap) {
        // Frozen, paint the snapshot.
        painter->drawPixmap(option->exposedRect.topLeft(), *m_frozenPixmap, option->exposedRect);
    }
    else {
        if (m_bitmapZoom) {
            // Frozen ----

            qreal czf = 1;
            qreal zoomF = zoomFactor();

            if ( m_bitmapZoomFactor != zoomF )
                czf = m_bitmapZoomFactor/zoomF;

            painter->save();

            if (czf < 1) {
                if (!m_checkeredBoxPixmap) {
                    setCheckeredPixmap();
                }
                QBrush brush(*m_checkeredBoxPixmap);
                painter->fillRect(0, 0, size().width(), size().height(), brush);
            }
            painter->drawImage(QRectF(0,0,size().width() * czf,size().height() * czf), *m_bitmapImage);
            painter->restore();
        }
        else if ( qAbs(zoomFactor() - m_dirtyZoomFactor) < 0.0001 )
        {
            // Do normal paint. ----

            // Cannot use normal QGraphicsWebView paint because have to fill background with white for the phone build
            // QGraphicsWebView::paintEvent( event);

            // Commented the following line out to try to improve scrolling performance.  hab - 3/25/10
            //painter->fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));

            //painter->fillRect(clipRect, QColor(255, 255, 255));


            QGraphicsWebView::paint(painter, option, widget);
        } else {
            // Do zooming paint. ----

            qreal czf = m_dirtyZoomFactor / zoomFactor();

            QWebFrame* frame = page()->mainFrame();

            painter->save();

            // Commented the following line out to try to improve scrolling performance.  hab - 3/25/10
            //painter->fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));

            QTransform transform;
            transform.scale(czf, czf);
//            painter.translate(-transform.map(frame->scrollPosition()));

            QRegion clipRegion = QRect(QPoint(0,0), size().toSize());
            if (option && !option->exposedRect.isEmpty())
            {
                clipRegion.intersect( option->exposedRect.toRect());
            }
            QTransform invert = transform.inverted();
            clipRegion = invert.map(clipRegion);
//            clipRegion.translate(frame->scrollPosition());

            painter->scale(czf, czf);
//        p->m_webPage->mainFrame()->renderContents(&painter, clipRegion);
            frame->render(painter, clipRegion);
            painter->restore();
        }

    }

    if(!isEnabled()) {
        // Disabled, apply whitewash.
        ChromeEffect::paintDisabledRect(painter, option->exposedRect);
    }
#ifdef ENABLE_PERF_TRACE
    PERF_DEBUG() << "GWebContentViewWidget::paint__end: " <<
        WrtPerfTracer::tracer()->elapsedTime(st) << "\n";
#endif
}
#endif //NO_RESIZE_ON_LOAD

void GWebContentViewWidget::freeze() {
    if(m_frozenCount == 0) {
        updateFrozenImage();
        update();
    }
    m_frozenCount++;
}

void GWebContentViewWidget::unfreeze() {
    if(m_frozenCount > 0) {
        m_frozenCount--;

        if(m_frozenCount == 0){
            delete m_frozenPixmap;
            m_frozenPixmap = 0;
            update();
        }
    }
}

void GWebContentViewWidget::setPage(QWebPage* pg)
{
  if (m_wrtPage == pg) return;

  if (m_wrtPage) {
    disconnect(page()->mainFrame(), 0, this, 0);
    m_wrtPage->setView(0);
  }
  /* Reset the webview page as well - for its internal clean up */
  QGraphicsWebView::setPage(pg);

  m_wrtPage = pg;

  // Call setViewportSize when the initial layout finishes unless pg is a super page.
  // Otherwise the superpage's zoom factor get set to its initialScale (0.6667).
  // This should be updated/removed when we start saving zoom factors on each page.
  if (page() && !m_webContentView->currentPageIsSuperPage()) {
#ifndef NO_RESIZE_ON_LOAD
    connect(page()->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(setViewportSize()));
#endif
//    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  }

  // setPage() above doesn't seem to trigger an update, do it explicitly.
  update();

  emit titleChanged(title());
  emit urlChanged(url().toString());
}

void GWebContentViewWidget::showNormalPage()
{
    WRT::WrtBrowserContainer *wbc = WebPageController::getSingleton()->currentPage();
    setPage((QWebPage *)wbc);  // static_cast here gives compiler error
}

void GWebContentViewWidget::setBitmapZoom(qreal zoom)
{
    if (!m_userScalable || zoom == zoomFactor())
        return;

    if (zoom < m_minimumScale)
        zoom = m_minimumScale;
    else if (zoom > m_maximumScale)
        zoom = m_maximumScale;

    m_bitmapZoom = true;
    m_bitmapZoomFactor = zoom;
    update();
}

void GWebContentViewWidget::setPageCenterZoomFactor(qreal zoom)
{
    //calculating the center of the widget
    QPoint widgetCenter = rect().center().toPoint();
    //find the content size before applying zoom
    QSize docSizeBeforeZoom = page()->mainFrame()->contentsSize();

    setZoomFactor(zoom);
    //after applying zoom calculate the document size and document center point
    QSize docSizeAfterZoom = page()->mainFrame()->contentsSize();
    QPoint docPoint = widgetCenter + page()->mainFrame()->scrollPosition();

    //calculate the shift in center point after applying zoom
    int dx = docSizeAfterZoom.width() * docPoint.x() / docSizeBeforeZoom.width();
    int dy = docSizeAfterZoom.height() * docPoint.y() / docSizeBeforeZoom.height();

    //move back the shifted center
    page()->mainFrame()->scroll(dx-docPoint.x(), dy-docPoint.y());
}

void GWebContentViewWidget::initializeViewportParams()
{
    m_maximumScale = KDefaultMaxScale;
    m_userScalable = true;
    m_inferWidthHeight = true;

    m_aspectRation = size().width() / size().height();

#ifdef NO_RESIZE_ON_LOAD
    QSize sz = size().toSize();
    m_viewportWidth = sz.width();
    m_viewportHeight = sz.height();
    m_initialScale = 1.0;
#else
    m_viewportWidth = KDefaultViewportWidth;
    m_viewportHeight = (int)size().height();

    if ( size().width() < size().height())       //if Portrait
      m_initialScale = size().width() / KDefaultPortraitScaleWidth;
    else
      m_initialScale = size().width() / KDefaultViewportWidth;
#endif //NO_RESIZE_ON_LOAD

    m_minimumScale = m_initialScale;
}

/*!
 * Provides the default values - used when opening a new blank window
 */
ZoomMetaData GWebContentViewWidget::defaultZoomData()
{
    ZoomMetaData data;

    data.maxScale = KDefaultMaxScale;
    data.minScale =  KDefaultMinScale;
    data.userScalable = false;
    data.zoomValue = KInitialZoomFactorValue;

    return data;
}

/*!
 * Set the viewport Size
 */
void GWebContentViewWidget::setViewportSize()
{

    if (!page()) return;

    QWebFrame* frame = page()->mainFrame();

    initializeViewportParams();

    // TODO: INVESTIGATE: In the case of multiple windows loading pages simultaneously, it is possible
    // to be calling this slot on a signal from a frame that is not
    // the frame of the page saved here. It might be better to use 'sender' instead of
    // page->mainFrame() to get the metaData so that we use the meta data of the corresponding
    // frame
    QMap<QString, QString> metaData = frame->metaData();
    QString viewportTag = metaData.value("viewport");

    if (!viewportTag.isEmpty()) {
        QStringList paramList;

        if (viewportTag.contains(';')) {
            paramList = viewportTag.split(";", QString::SkipEmptyParts);
        } else {
            paramList = viewportTag.split(",", QString::SkipEmptyParts);
        }

        int paramCount = 0;
        while (paramCount < paramList.count()) {
            QStringList subParamList = paramList[paramCount].split ('=', QString::SkipEmptyParts);
            paramCount++;
            QString viewportProperty = subParamList.front();
            QString propertyValue = subParamList.back();
            parseViewPortParam(viewportProperty.trimmed(), propertyValue.trimmed());
        }
    }

    m_initialScale = qBound(m_minimumScale, m_initialScale, m_maximumScale);
#ifdef NO_RESIZE_ON_LOAD  
	#if QT_VERSION < 0x040600
		page()->setFixedContentsSize(QSize(m_viewportWidth, m_viewportHeight));
	#else
		page()->setPreferredContentsSize(QSize((int)m_viewportWidth, (int)m_viewportHeight));
	#endif
#endif //NO_RESIZE_ON_LOAD
   if((m_webContentView->currentPageIsSuperPage())){
        page()->setPreferredContentsSize(QSize((int)m_viewportWidth, (int)m_viewportHeight));
   }
#ifndef NO_RESIZE_ON_LOAD
  qreal zoomF = 0.0;
  QString str;
  if (m_isResize &&  (m_currentinitialScale != zoomFactor())) {
    zoomF = ((qreal)(page()->viewportSize().width()-10) * zoomFactor())/(m_previousViewPortwidth-10);
    str.setNum(zoomF,'f',2);
    zoomF = str.toDouble();
    setPageZoomFactor(zoomF);
  }
  else {
    setPageZoomFactor(m_initialScale);
  }
  m_BlockInFocus = QWebElement();
  m_currentinitialScale = m_initialScale;
#endif //NO_RESIZE_ON_LOAD

  // Let the page save the data. Even though it is part of the frame, it is easier to
  // save the info in the page to avoid parsing the meta data again.
  WrtBrowserContainer* pg = static_cast<WrtBrowserContainer*>(page());
  pg->setPageZoomMetaData(pageZoomMetaData());
  

}

qreal GWebContentViewWidget::initialScale()
{
  return  m_initialScale;
}

void GWebContentViewWidget::parseViewPortParam(const QString &propertyName, const QString &propertyValue)
{
    if (propertyName == KViewPortWidthTag) {
      if (propertyValue == KViewPortDeviceWidthTag) {
            m_viewportWidth = size().width();
        m_viewportHeight = m_viewportWidth * m_aspectRation;
      }
        else if (propertyValue == KViewPortDeviceHeightTag) {
            m_viewportWidth = size().height();
        m_viewportHeight = m_viewportWidth * m_aspectRation;
        }
        else {
        m_viewportWidth = propertyValue.toInt();

            if (m_viewportWidth < KMinViewportWidth)
          m_viewportWidth = KMinViewportWidth;
        else if (m_viewportWidth > KMaxViewportWidth)
          m_viewportWidth = KMaxViewportWidth;

            m_viewportHeight = m_viewportWidth * m_aspectRation;
      }
        m_initialScale = size().width() / m_viewportWidth;
        if (m_initialScale < KDefaultMinScale || m_initialScale > KDefaultMaxScale)
            m_initialScale = KDefaultMinScale;
        m_minimumScale = m_initialScale;
        m_inferWidthHeight = false;
    }
    else if (propertyName == KViewPortHeightTag) {
      if (propertyValue == KViewPortDeviceWidthTag) {
            m_viewportHeight = (int)size().width();
        m_viewportWidth = m_viewportHeight * m_aspectRation;
      }
        else if (propertyValue == KViewPortDeviceHeightTag) {
            m_viewportHeight = (int)size().height();
        m_viewportWidth = m_viewportHeight * m_aspectRation;
        }
        else {
        m_viewportHeight = propertyValue.toInt();

            if (m_viewportHeight < KMinViewportHeight)
          m_viewportHeight = KMinViewportHeight;
        else if (m_viewportHeight > KMaxViewportHeight)
          m_viewportHeight = KMaxViewportHeight;

            m_viewportWidth = m_viewportHeight * m_aspectRation;
      }
        m_initialScale = size().height() / m_viewportHeight;
        if (m_initialScale < KDefaultMinScale || m_initialScale > KDefaultMaxScale)
            m_initialScale = KDefaultMinScale;
        m_minimumScale = m_initialScale;
        m_inferWidthHeight = false;
    }
    else if (propertyName == KViewPortInitialScaleTag) {
        m_initialScale = propertyValue.toDouble();
        if (m_inferWidthHeight) {
            m_viewportWidth = (int)size().width();
            m_viewportHeight = m_viewportWidth * m_aspectRation;
        }
    }
    else if (propertyName == KViewPortMinScaleTag) {
        m_minimumScale = propertyValue.toDouble();
        if (m_minimumScale < 0
            || m_minimumScale > KMaxPageZoom
            || m_minimumScale > m_maximumScale)
            m_minimumScale = KDefaultMinScale;
    }
    else if (propertyName == KViewPortMaxScaleTag) {
        m_maximumScale = propertyValue.toDouble();
        if (m_maximumScale < 0
            || m_maximumScale > KMaxPageZoom
            || m_maximumScale < m_minimumScale)

            m_maximumScale = KDefaultMaxScale;
    }
    else if (propertyName == KViewPortUserScalableTag) {
        if (propertyValue =="no" || propertyValue =="0")
    {
      m_userScalable = false;
      view()->deactivateZoomActions();
    }
        else
            m_userScalable = true;
    }
}


bool GWebContentViewWidget::isUserScalable()
{
    return m_userScalable;
}

qreal GWebContentViewWidget::minimumScale()
{
    return m_minimumScale;
}

qreal GWebContentViewWidget::maximumScale()
{
    return m_maximumScale;
}

ZoomMetaData GWebContentViewWidget::pageZoomMetaData() {

    ZoomMetaData data;

    data.minScale = m_minimumScale;
    data.maxScale = m_maximumScale;
    data.userScalable = m_userScalable;
    data.zoomValue = view()->getSavedZoomValueInView();

    return data;
}

void GWebContentViewWidget::setPageZoomMetaData(ZoomMetaData data) {

    m_minimumScale = data.minScale ;
    m_maximumScale = data.maxScale ;
    m_userScalable = data.userScalable;
    view()->setSavedZoomValueInView(data.zoomValue);
}

QWebPage* GWebContentViewWidget::page() const
{
    if (!m_wrtPage) {
        GWebContentViewWidget* that = const_cast<GWebContentViewWidget*>(this);
        that->setPage(BrowserPageFactory::openBrowserPage());
    }
    return m_wrtPage;
}

QPointF GWebContentViewWidget::mapToGlobal(const QPointF& p)
{
    QList<QGraphicsView*> gvList = scene()->views();
    QList<QGraphicsView*>::iterator it;
    for (it = gvList.begin(); it != gvList.end(); it++)
        {
            if (static_cast<QGraphicsView*>(*it)->hasFocus())
                {
                    QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
                    return viewport->mapToGlobal(mapToScene(p).toPoint());
                }
        }

    return QPoint(0, 0);
}

QPointF GWebContentViewWidget::mapFromGlobal(const QPointF& p)
{
    QList<QGraphicsView*> gvList = scene()->views();
    QList<QGraphicsView*>::iterator it;
    for (it = gvList.begin(); it != gvList.end(); it++)
        {
            if (static_cast<QGraphicsView*>(*it)->hasFocus())
                {
                    QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
                    return mapFromScene(viewport->mapFromGlobal(p.toPoint()));
                }
        }

    return QPoint(0, 0);
}

void GWebContentViewWidget::setTextSizeMultiplier(qreal factor)
{
    page()->mainFrame()->setTextSizeMultiplier(factor);
}


void GWebContentViewWidget::onInitLayout()
{
  update();
}

}
