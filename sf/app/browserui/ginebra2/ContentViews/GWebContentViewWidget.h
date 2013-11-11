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
*/


#ifndef GWebContentViewWidget_H_
#define GWebContentViewWidget_H_

#include <QObject>
#include <QWebElement>
#include <QImage>
#include <QGraphicsWebView>
#include <QWebFrame>
#include <QTime>

#include "WebViewEventContext.h"
#include "ZoomMetaData.h"

class QGraphicsSceneContextMenuEvent;

namespace GVA {
#ifndef NO_QSTM_GESTURE
class WebTouchNavigation;
#endif

class GWebContentView;

class GWebContentViewWidget : public QGraphicsWebView
{
    Q_OBJECT
public:

    GWebContentViewWidget(QObject* parent, GWebContentView* view, QWebPage* pg);
    virtual ~GWebContentViewWidget();

    GWebContentView* view() const { return m_webContentView; }

    /*!
     * \brief Returns zoom factor value
     */
    qreal zoomFactor() { return m_wrtPage ? m_wrtPage->mainFrame()->zoomFactor() : 0; }

    /*!
     * \brief Returns dirty zoom factor value
     */
    qreal dirtyZoomFactor() { return m_dirtyZoomFactor; }

    /*!
     * \brief Set zoom factor value
     * \param zoom
     */
    void setZoomFactor(qreal zoom);

    /*!
     * \brief Set dirty zoom factor value
     * \param zoom
     */
    void setDirtyZoomFactor(qreal zoom);

    void setTextSizeMultiplier(qreal factor);

    QImage getPageSnapshot();

    void bitmapZoomCleanup();
    void createPageSnapShot();

    void resizeEvent(QGraphicsSceneResizeEvent* e);
    virtual bool event(QEvent * e);

    QWebPage* page() const;
    void setPage(QWebPage* pg);
    QWebPage* wrtPage() { return m_wrtPage; }
    void showNormalPage();

    void setBitmapZoom(qreal zoom);
    void setPageCenterZoomFactor(qreal zoom);

    void initializeViewportParams();
    void parseViewPortParam(const QString &propertyName, const QString &propertyValue);
    bool isUserScalable();
    qreal minimumScale();
    qreal maximumScale();
    qreal initialScale();
    void updateViewport();
    void setBlockElement(const QWebElement &pt);
    QPointF mapToGlobal(const QPointF& p);
    QPointF mapFromGlobal(const QPointF& p);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) ;
    void setPageZoomFactor(qreal zoom);

    ZoomMetaData pageZoomMetaData();
    void setPageZoomMetaData(ZoomMetaData params);
    ZoomMetaData defaultZoomData();
    void setCheckeredPixmap();

    void onLoadStarted();
    void onLoadFinished();

    /*!
     * \brief Freeze the on-screen state of the window.
     * Takes a snapshot of the window in its current state.  The paint() method will then use
     * this snapshot for all further paints, until unfreeze() is called.  Freeze() increments
     * a counter and unfreeze() decrements the counter so that nested calls to them are handled
     * correctly.
     * \sa unfreeze
     */
    void freeze();

    /*!
     * \brief Unfreeze the on-screen state of the window.
     * \sa freeze
     */
    void unfreeze();

    /*!
     * \brief Returns true if the window is currently frozen.
     * \sa freeze
     * \sa unfreeze
     */
    bool frozen() const { return m_frozenCount > 0; }

public slots:
    void setViewportSize();

    void onInitLayout();
    void onLongPressEvent(QPoint pos);

signals:
    void contextEvent(::WebViewEventContext *context);
    void BlockFocusChanged(QPoint element);

    void pageZoomMetaDataChange(QWebFrame* frame, ZoomMetaData data);protected:
    void paintEvent(QPaintEvent *event);
    void longPressEvent(::WebViewEventContext *context);

private slots:
    /*!
     * \brief Update the "frozen" image of this window.
     */
    void updateFrozenImage();

private:
    void updateViewportSize(QGraphicsSceneResizeEvent *e);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    GVA::GWebContentView* m_webContentView;
    qreal       m_dirtyZoomFactor;
    QPixmap     *m_frozenPixmap;
    int         m_frozenCount;
    QWebPage   *m_wrtPage;
    bool        m_bitmapZoom;
    qreal       m_aspectRation;
    qreal       m_viewportWidth;
    qreal       m_viewportHeight;
    bool        m_userScalable;
    qreal       m_minimumScale;
    qreal       m_maximumScale;
    qreal       m_initialScale;
    bool        m_inferWidthHeight;
    bool        m_isResize;
    qreal       m_currentinitialScale;
    QWebElement m_BlockElement;
    QWebElement m_BlockInFocus;
    int         m_previousViewPortwidth;
    qreal       m_Ratiox;
    qreal       m_Ratioy;


    QImage*     m_bitmapImage;
    qreal       m_bitmapZoomFactor;
    QPixmap*    m_checkeredBoxPixmap;

    bool        m_inLoading;
    QTime       m_loadingTime;
#ifndef NO_QSTM_GESTURE
    WebTouchNavigation* m_touchNavigation;
#endif
};

}

#endif /* GWebContentViewWidget_H_ */
