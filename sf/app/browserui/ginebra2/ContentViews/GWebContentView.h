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

#ifndef GWebContentView_H
#define GWebContentView_H

#include <QObject>
#include <QVariant>
#include <QGraphicsWidget>
#include <QWebElement>
#include <QGraphicsWebView>
#include "controllableviewimpl.h"
#include "messageboxproxy.h"
#include "ZoomMetaData.h"
#include "GSuperWebPage.h"
#include "ContentViewDelegate.h"
#include "GContentViewTouchNavigation.h"

static const char KBOOKMARKURLFILE[]            = "file://";
static const char KBOOKMARKURLFILESLASH[]       = "file:///";
static const char KENCODEDBACKSLASH[]           = "%5C";
static const char KENCODEDDOUBLEQUOTE[]         = "%22";
class WebViewEventContext;
class QContextMenuEvent;
class QWebPage;
class QWebFrame;
class QTimeLine;
class GWebContentViewWidget;

namespace WRT {
    class WrtBrowserContainer;
}

namespace GVA {

  class GWebPage;
  class WebPageWrapper;
  class ChromeWidget;
  class ContentViewDelegate;
#ifdef BEDROCK_TILED_BACKING_STORE
  class WebContentViewWidget;
#endif

  class GWebContentView : public ControllableViewBase
  {
      Q_OBJECT
  public:
      GWebContentView(ChromeWidget *chrome, QObject * parent = 0, const QString &objectName = QString::null);
      virtual ~GWebContentView();

      static QString Type() { return "webView"; }
      virtual QString type() const { return Type(); }

// do we need both of these?
      QGraphicsWidget* widget() const;
#ifdef BEDROCK_TILED_BACKING_STORE
      QGraphicsWebView* webWidget() const;
#else
      GWebContentViewWidget *webWidget() const;
#endif
      // Returns the DOM 'window' object of the page.
      QVariant getContentWindowObject();

      virtual void connectAll();
      virtual void activate();
      virtual void deactivate();

      virtual void triggerAction(const QString & action);

      virtual QString title() const;

      QUrl url();
      QWebPage *currentPage();

#ifdef BEDROCK_TILED_BACKING_STORE
      qreal getZoomFactor();
#else
      qreal getZoomFactor() const;
      void setSavedZoomValueInView(qreal zoomValue);
      qreal getSavedZoomValueInView() const;
#endif
      static ControllableView* createNew(QWidget *parent);

      /*!
        Return the list of public QActions most relevant to the view's current context
        (most approptiate for contextual menus, etc.
      */
      virtual QList<QAction*> getContext();
      virtual QAction * getAction(const QString & action);

      QWebPage* wrtPage();
      void scrollViewBy(int dx, int dy);
      void scrollViewTo(int x, int y);

      void  changeZoomAction(qreal zoom);
      void deactivateZoomActions();

      // Super page methods.
      GSuperWebPage * createSuperPage(const QString &name, bool persist = false);
      void destroySuperPage(const QString &name);
      QObjectList getSuperPages();
      bool setCurrentSuperPage(const QString &name);
      GSuperWebPage * currentSuperPage() {return m_currentSuperPage.value();}
      void showSuperPage(const QString &name);
      GSuperWebPage * superPage(const QString &name);
      bool isSuperPage(const QString &name);
      bool currentPageIsSuperPage() const;

      void bitmapZoomStop();
      virtual void show() {
          widget()->show();
      }

      virtual void hide() {
           widget()->hide();
      }

      bool gesturesEnabled() const;
      void setGesturesEnabled(bool value);

      bool enabled() const;
      void setEnabled(bool value);


      bool frozen() const { 
#ifndef BEDROCK_TILED_BACKING_STORE		  
		  return webWidget()->frozen(); 
#else	  
		  return false;
#endif
	  }

      void freeze() { 
#ifndef BEDROCK_TILED_BACKING_STORE			  
		  return webWidget()->freeze(); 
#endif
	  }
      void unfreeze() { 
#ifndef BEDROCK_TILED_BACKING_STORE			  
		  return webWidget()->unfreeze(); 
#endif
		}

  signals:
      void ContextChanged();
      void iconChanged();
      void loadFinished(bool ok);
      void loadProgress(int progress);
      void loadStarted();
      void statusBarMessage(const QString & text);
      void titleChanged(const QString & title);
      void urlChanged(const QString & url);
      void secureConnection(bool secure);
      void backEnabled(bool enabled);
      void forwardEnabled(bool enabled);
      void startingPanGesture(int);
      void contentViewMouseEvent(QEvent::Type type);
	  void superPageShown(const QString &name);
#ifdef BEDROCK_TILED_BACKING_STORE
      void contextEvent(::WebViewEventContext* context);
#endif      

  public slots:
      void loadUrlToCurrentPage(const QString & url);
      void zoomIn(qreal factor = 0.1);
      void zoomOut(qreal factor = 0.1);
#ifndef BEDROCK_TILED_BACKING_STORE
      void zoomP();
      void zoomN();
      void zoomBy(qreal delta) { zoomIn(delta); }
#else
      void setZoomActions(bool enableZoomIn, bool enableZoomOut);
#endif
      void setZoomFactor(qreal factor);
      void showMessageBox(WRT::MessageBoxProxy* data);
      void stop();
      void back();
      void forward();
      void reload();
      void zoom(bool in);
      void toggleZoom();
      void stopZoom();
      void scrollBy(int deltaX, int deltaY) { scrollViewBy(deltaX, deltaY); }
      void scrollTo(int x, int y) { scrollViewTo(x,y);}
      int scrollX();
      int scrollY();
      int contentWidth();
      int contentHeight();

      // Show the current normal web page, ie. not a super page.
      void showNormalPage();

      void dump();

  private slots:
    void updateZoom(qreal delta);
    void onLoadStarted();
    void onLoadFinished(bool ok);
    void onUrlChanged(const QUrl& url);

    // Called by the page controller when it creates a page.
    void pageCreated(WRT::WrtBrowserContainer * pg);
    // Called by the page controller when changes a page.
    void pageChanged(WRT::WrtBrowserContainer * , WRT::WrtBrowserContainer *);

#ifdef BEDROCK_TILED_BACKING_STORE
    void handleContextEventObject(QWebHitTestResult* eventTarget);
    void handleViewScrolled(QPoint& scrollPos, QPoint& delta);
#endif
  protected:
#ifdef BEDROCK_TILED_BACKING_STORE
    QGraphicsWidget* webWidgetConst();
#else
    GWebContentViewWidget *webWidgetConst() const { return m_widget; }
#endif
    ChromeWidget *chrome() { return m_chrome; }
    void updateWebPage(WRT::WrtBrowserContainer * pg);
    void changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage);

  protected:
#ifdef BEDROCK_TILED_BACKING_STORE
    WebContentViewWidget* m_widget;
#else
    GWebContentViewWidget *m_widget;
#endif
    QNetworkAccessManager *m_networkMgr; //Owned
    ChromeWidget *m_chrome;  // not owned

  private:
    void setActions();
    virtual void setJSObject(const QString &objectName);
    QMap<QString, QAction*>  m_actions;
    QTimeLine * m_timeLine;
    bool m_zoomIn;

#ifndef BEDROCK_TILED_BACKING_STORE
    GContentViewTouchNavigation* m_touchNavigation;
#endif
    bool m_backEnabled;
    bool m_forwardEnabled;

    ChromeWidget *m_chromeWidget;  // not owned
    WebPageWrapper* m_sharedPage;
    typedef QMap<QString, GSuperWebPage *> PageMap;
    PageMap m_superPages;
    PageMap::iterator m_currentSuperPage;
    bool m_currentPageIsSuperPage;
    QTimer *m_timer;
    qreal m_value;
    bool m_gesturesEnabled;
    bool m_enabled;
    qreal m_savedZoomValueInView;
  };

}

#endif // GWebContentView_H
