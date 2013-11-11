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
#ifndef GWebContentViewJSObject_H_
#define GWebContentViewJSObject_H_

#include <QObject>
#include "controllableviewimpl.h"
#include "GWebContentView.h"
#include "GSuperWebPage.h"

class WebViewEventContext;

namespace GVA {

/*! \ingroup JavascriptAPI
  \brief This class provides the javascript API for GWebContentView.
 */
class GWebContentViewJSObject : public ::ControllableViewJSObject {
    Q_OBJECT
  public:
    GWebContentViewJSObject(GWebContentView *contentView, ::QWebFrame *chromeFrame, const QString &objectName)
      : ::ControllableViewJSObject(contentView, chromeFrame, objectName)
    {
      //qDebug() << "GWebContentViewJSObject::GWebContentViewJSObject: " << this;
    }

    qreal getZoomFactor() const { return webContentViewConst()->getZoomFactor(); }
    void setZoomFactor(qreal factor)  { webContentView()->setZoomFactor(factor); }
    Q_PROPERTY(qreal zoomFactor READ getZoomFactor WRITE setZoomFactor)

    Q_PROPERTY(QObjectList superPages READ getSuperPages)
    QObjectList getSuperPages() { return webContentView()->getSuperPages(); }

    /*! This property holds whether touch navigation is enabled.
     */
    Q_PROPERTY(bool gesturesEnabled READ getGesturesEnabled WRITE setGesturesEnabled)
    bool getGesturesEnabled() const
    {
        return webContentViewConst()->gesturesEnabled();
    }
    void setGesturesEnabled(bool value)
    {
        webContentView()->setGesturesEnabled(value);
    }

    Q_PROPERTY(bool enabled WRITE setEnabled READ enabled)
    bool enabled() const { return webContentViewConst()->enabled(); }
    void setEnabled(bool value) { webContentView()->setEnabled(value); }

public slots:
    void loadUrlToCurrentPage(const QString & url)
        { webContentView()->loadUrlToCurrentPage(url); }
    QObject *currentPage() { return webContentView()->currentPage(); }
    void back() { webContentView()->back(); }
    void forward() { webContentView()->forward(); }
    void reload() { webContentView()->reload(); }
#ifndef BEDROCK_TILED_BACKING_STORE
    void zoomIn(qreal deltaPercent = 0.1) { webContentView()->zoomIn(deltaPercent); }
    void zoomOut(qreal deltaPercent = 0.1) { webContentView()->zoomOut(deltaPercent); }
    void zoomBy(qreal delta) { zoomIn(delta); }
#endif	
    void zoom(bool in) { webContentView()->zoom(in); }
    void toggleZoom() { webContentView()->toggleZoom(); }
    void stopZoom() { webContentView()->stopZoom(); }
    void scrollBy(int deltaX, int deltaY) { webContentView()->scrollBy(deltaX, deltaY); }
    void scrollTo(int x, int y) { webContentView()->scrollTo(x, y); }
    int scrollX() { return webContentView()->scrollX(); }
    int scrollY() { return webContentView()->scrollY(); }
    int contentWidth() { return webContentView()->contentWidth(); }
    int contentHeight() { return webContentView()->contentHeight(); }
    void showNormalPage() { return webContentView()->showNormalPage(); }
    bool currentPageIsSuperPage() { return webContentView()->currentPageIsSuperPage(); }
    void dump() { return webContentView()->dump(); }
    bool frozen() const { return webContentViewConst()->frozen(); }
    void freeze() { return webContentView()->freeze(); }
    void unfreeze() { return webContentView()->unfreeze(); }

    // Super page slots.
    QObject * createSuperPage(const QString &name, bool persist=false) { return webContentView()->createSuperPage(name, persist); }
    void destroySuperPage(const QString &name) { webContentView()->destroySuperPage(name); }
    void setCurrentSuperPage(const QString &name) { webContentView()->setCurrentSuperPage(name); }
    QObject * currentSuperPage() { return webContentView()->currentSuperPage(); }
    QString currentSuperPageName() { return webContentView()->currentSuperPage()->objectName(); }
    void showSuperPage(const QString &name) { webContentView()->showSuperPage(name); }
    QObject * superPage(const QString &name) { return webContentView()->superPage(name); }
    bool isSuperPage(const QString &name) { return webContentView()->isSuperPage(name); }

    bool bedrockTiledBackingStoreEnabled() 
    {
#ifdef BEDROCK_TILED_BACKING_STORE
        return true;
#else
        return false;
#endif
    }
    void setSavedZoomValueInView(qreal zoomValue){ webContentView()->setSavedZoomValueInView(zoomValue); }

signals:
    void ContextChanged();
    void iconChanged();
    void loadFinished(bool ok);
    void loadProgress(int progress);
    void loadStarted();
    void titleChanged(const QString & title);
    void urlChanged(const QString & url);
    void secureConnection(bool secure);
    void backEnabled(bool enabled);
    void forwardEnabled(bool enabled);
    void onStatusBarMessage( const QString & text );
    void onStatusBarVisibilityChangeRequested(bool visible);
    void startingPanGesture(int directionHint);

    // Sent when the display mode changes from landscape to protrait or vice versa.
    void onDisplayModeChanged(const QString &orientation);

    void contextEvent(QObject *context);
	void superPageShown(const QString &name);
    

private slots:
    void statusBarMessage( const QString & text );
    void statusBarVisibilityChangeRequested(bool visible);
    void onContextEvent(::WebViewEventContext *context);

protected:
    GWebContentView *webContentView() { return static_cast<GWebContentView *>(m_contentView); }
    GWebContentView *webContentViewConst() const { return static_cast<GWebContentView *>(m_contentView); }
};

}

#endif /* GWebContentViewJSObject_H_ */
