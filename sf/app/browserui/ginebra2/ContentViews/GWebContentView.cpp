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
* Description :
*
*/

#include "GWebContentView.h"
#include "GWebContentViewWidget.h"
#include "GWebContentViewJSObject.h"
#include "SuperPageView.h"
#include "ChromeWidget.h"
#include "WebViewEventContext.h"
#include "browserpagefactory.h"
#include "wrtbrowsercontainer.h"

#ifdef BEDROCK_TILED_BACKING_STORE
#include "WebContentViewWidget.h"
#endif

#include "webpagecontroller.h"
#include "Utilities.h"
#include "GSuperWebPage.h"
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif

#include <QGraphicsWebView>
#include <QWebHistory>
#include <QWebPage>
#include <QWebFrame>
#include <QTimeLine>
#include <QDebug>
#include <QtGui>

namespace GVA {


  // ----------------------------------------------------------

  const qreal KZoomInStep = 1.05;
  const qreal KZoomOutStep = 0.95238;
  const qreal KInitialZoomFactorValue = 0.653061;
  GWebContentView::GWebContentView(ChromeWidget *chrome, QObject * parent, const QString &objectName)
    : ControllableViewBase(parent),
      m_networkMgr(0),
      m_chrome(chrome),
      m_timeLine(0),
      m_zoomIn(false),
      m_backEnabled(false),
      m_forwardEnabled(false),
      m_sharedPage(0),
      m_currentSuperPage(m_superPages.begin()),
      m_currentPageIsSuperPage(false),
      m_timer(NULL),
      m_enabled(true),
      m_savedZoomValueInView(KInitialZoomFactorValue) 
  {
      setObjectName(objectName);
      WRT::WrtBrowserContainer* page = BrowserPageFactory::openBrowserPage();

      setActions();

#ifdef BEDROCK_TILED_BACKING_STORE
      m_widget = new WebContentViewWidget(parent, page);
      connect(m_widget
              , SIGNAL(updateZoomActions(bool,bool))
              , this
              , SLOT(setZoomActions(bool, bool)));
#else
      m_widget = new GWebContentViewWidget(parent, this, page);
#endif

#ifndef __gva_no_chrome__
      setJSObject(objectName);
#endif
      m_networkMgr = webWidget()->page()->networkAccessManager();

      webWidget()->page()->currentFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
      webWidget()->page()->currentFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

#ifndef BEDROCK_TILED_BACKING_STORE
#ifdef NO_QSTM_GESTURE
      m_touchNavigation = new GContentViewTouchNavigation(page, m_widget);
      m_touchNavigation->setChromeWidget(m_chrome);
      m_touchNavigation->setParent(m_jsObject);
      m_touchNavigation->setObjectName("touchNav");
      safe_connect(m_touchNavigation, SIGNAL(mouseEvent(QEvent::Type)) , this, SIGNAL(contentViewMouseEvent(QEvent::Type)));
      safe_connect(m_touchNavigation, SIGNAL(longPressEvent(QPoint)),
                   webWidget(), SLOT(onLongPressEvent(QPoint)));
#endif
#endif
      WRT::WrtBrowserContainer * wrtPage = static_cast<WRT::WrtBrowserContainer *>(page);
      if (wrtPage) {
          wrtPage->setWebWidget(webWidget());
          pageCreated(wrtPage);
      }

      connectAll();
  }

  GWebContentView::~GWebContentView() {
      disconnect(m_jsObject);
      disconnect(webWidget());
#ifndef BEDROCK_TILED_BACKING_STORE
      delete m_touchNavigation;
#endif
      foreach(QAction * action, m_actions) {
          delete action;
      }

      delete m_widget;
      foreach(GSuperWebPage *page, m_superPages) {
          delete page;
      }
  }

#ifdef BEDROCK_TILED_BACKING_STORE
void GWebContentView::zoomIn(qreal deltaPercent)
{
    //FIX ME : Do we need deltaPercent?
    Q_UNUSED(deltaPercent);
    m_widget->setPageZoom(true);
}

void GWebContentView::zoomOut(qreal deltaPercent)
{
    //FIX ME : Do we need deltaPercent?
    Q_UNUSED(deltaPercent);
    m_widget->setPageZoom(false);
}
#endif

#ifndef BEDROCK_TILED_BACKING_STORE
void GWebContentView::bitmapZoomStop()
{
    if (m_timer) {
        m_timer->stop();
        disconnect(m_timer,SIGNAL(timeout()));
        delete m_timer;
        m_timer = NULL;
    }
    qreal zoomFactor = m_value * webWidget()->zoomFactor();
    ((zoomFactor+0.001) >= webWidget()->maximumScale() )? webWidget()->setZoomFactor(webWidget()->maximumScale()):webWidget()->setZoomFactor(zoomFactor);
    webWidget()->bitmapZoomCleanup();
}

void GWebContentView::zoomP()
{
    if ((m_value * webWidget()->zoomFactor()) >  webWidget()->maximumScale()) {
        if (m_timer && m_timer->isActive())
            bitmapZoomStop();
    }else {
        if (m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
        webWidget()->setBitmapZoom(m_value * webWidget()->zoomFactor());
        m_value *= KZoomInStep;
    }
}

void GWebContentView::zoomN()
{
    if ((m_value * webWidget()->zoomFactor()) <  webWidget()->minimumScale()){
       // if (m_timer && m_timer->isActive())
            bitmapZoomStop();
    }else {
        if (m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
        webWidget()->setBitmapZoom(m_value * webWidget()->zoomFactor());
        m_value *= KZoomOutStep;
    }
}

void GWebContentView::zoomIn(qreal deltaPercent)
{
    Q_UNUSED(deltaPercent)
    if (webWidget() && webWidget()->isUserScalable()) {
        if (m_timer && m_timer->isActive()) {
            if (!m_timer->isSingleShot())
                m_value /= KZoomInStep;
            bitmapZoomStop();
            return;
        }else if (!m_timer)
            m_timer = new QTimer(this);

        m_value = KZoomInStep;

        if ((m_value * webWidget()->zoomFactor()) <  webWidget()->maximumScale()) {
            webWidget()->createPageSnapShot();
            connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomP()));
            zoomP();
            m_timer->setSingleShot(true);
            m_timer->start(500);
        }else {
            delete m_timer;
            m_timer = NULL;
            webWidget()->setZoomFactor(m_value * webWidget()->zoomFactor());
        }
        setSavedZoomValueInView(m_value * webWidget()->zoomFactor());
    }
}

void GWebContentView::zoomOut(qreal deltaPercent)
{
    Q_UNUSED(deltaPercent)
    if (webWidget() && webWidget()->isUserScalable()) {
        if (m_timer && m_timer->isActive()) {
            if (!m_timer->isSingleShot())
                m_value /= KZoomOutStep;
            bitmapZoomStop();
            return;
        }else if (!m_timer)
            m_timer = new QTimer(this);

        m_value = KZoomOutStep;

        if ((m_value * webWidget()->zoomFactor()) >  webWidget()->minimumScale()) {
            webWidget()->createPageSnapShot();
            connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomN()));
            zoomN();
            m_timer->setSingleShot(true);
            m_timer->start(500);
        }else {
            delete m_timer;
            m_timer = NULL;
            webWidget()->setZoomFactor(m_value * webWidget()->zoomFactor());
        }
        setSavedZoomValueInView(m_value * webWidget()->zoomFactor());
    }
}

#endif


bool GWebContentView::gesturesEnabled() const 
{ 
#ifndef BEDROCK_TILED_BACKING_STORE
    return m_touchNavigation->enabled(); 
#else	
    return m_widget->gesturesEnabled();
#endif
}

void GWebContentView::setGesturesEnabled(bool value) 
{ 
#ifndef BEDROCK_TILED_BACKING_STORE		  
    m_touchNavigation->setEnabled(value); 
#else
    m_widget->setGesturesEnabled(value);
#endif
}

  void GWebContentView::connectAll() {
    //qDebug() << "GWebContentView::connectAll: " << widget();

#ifndef __gva_no_chrome__
#ifndef BEDROCK_TILED_BACKING_STORE
    safe_connect(widget(), SIGNAL(contextEvent(::WebViewEventContext *)),
                 m_jsObject, SLOT(onContextEvent(::WebViewEventContext *)));
#else
    safe_connect(this
                 , SIGNAL(contextEvent(::WebViewEventContext*))
                 ,  m_jsObject
                 , SLOT(onContextEvent(::WebViewEventContext*)));

    safe_connect(m_widget
                , SIGNAL(contextEventObject(QWebHitTestResult*))
                , this
                , SLOT(handleContextEventObject(QWebHitTestResult*)));

    safe_connect(m_widget
                 , SIGNAL(viewScrolled(QPoint&, QPoint&))
                 , this
                 , SLOT(handleViewScrolled(QPoint&, QPoint&)));

    safe_connect(m_widget
                 , SIGNAL(mouseEvent(QEvent::Type)) 
                 , this
                 , SIGNAL(contentViewMouseEvent(QEvent::Type)));

    
#endif
    QObject::connect(webWidget(), SIGNAL(titleChanged(const QString &)), m_jsObject, SIGNAL(titleChanged(const QString &)));
    QObject::connect(webWidget(), SIGNAL(loadStarted()), m_jsObject, SIGNAL(loadStarted()));
    QObject::connect(webWidget(), SIGNAL(loadProgress(int)), m_jsObject, SIGNAL(loadProgress(int)));
    QObject::connect(webWidget(), SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
#endif
    QObject::connect(webWidget(), SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    QObject::connect(webWidget(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    QObject::connect(webWidget(), SIGNAL(urlChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));
#ifndef __gva_no_chrome__
    QObject::connect(this, SIGNAL(urlChanged(const QString&)), m_jsObject, SIGNAL(urlChanged(const QString&)));
    QObject::connect(this, SIGNAL(backEnabled(bool)), m_jsObject, SIGNAL(backEnabled(bool)));
    QObject::connect(this, SIGNAL(forwardEnabled(bool)), m_jsObject, SIGNAL(forwardEnabled(bool)));
    QObject::connect(this, SIGNAL(loadFinished(bool)), m_jsObject, SIGNAL(loadFinished(bool)));
    QObject::connect(this, SIGNAL(secureConnection(bool)), m_jsObject, SIGNAL(secureConnection(bool)));

#ifndef BEDROCK_TILED_BACKING_STORE
    connect(m_touchNavigation, SIGNAL(startingPanGesture(int)), m_jsObject, SIGNAL(startingPanGesture(int)));
#else
    connect(this, SIGNAL(startingPanGesture(int)), m_jsObject, SIGNAL(startingPanGesture(int)));
#endif

	QObject::connect(this, SIGNAL(superPageShown(const QString&)), m_jsObject, SIGNAL(superPageShown(const QString&)));         
#endif
    connect(WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer*)),
            this, SLOT(pageCreated(WRT::WrtBrowserContainer*)));
    connect(WebPageController::getSingleton(), SIGNAL(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)),
            this, SLOT(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)));
    connect(WebPageController::getSingleton(), SIGNAL(showMessageBox(WRT::MessageBoxProxy*)),
            this, SLOT(showMessageBox(WRT::MessageBoxProxy*)));
  }

  QVariant GWebContentView::getContentWindowObject() {
    try {
        return webWidget()->page()->mainFrame()->evaluateJavaScript("window");
    }
    catch(...) {
        qDebug() << "GWebContentView::getContentWindowObject: caught expection";
        return QVariant();
    }
  }

  QGraphicsWidget* GWebContentView::widget() const {
    return static_cast<QGraphicsWidget*>(m_widget);
  }

#ifdef BEDROCK_TILED_BACKING_STORE
  QGraphicsWebView* GWebContentView::webWidget() const {
    return m_widget->webView();
  }
#else
  GWebContentViewWidget *GWebContentView::webWidget() const {
    return m_widget;
  }
#endif

  QString GWebContentView::title() const {
    return webWidget()->title();
  }

  QUrl GWebContentView::url() {
    return webWidget()->url();
  }

  QWebPage* GWebContentView::wrtPage() {
#ifdef BEDROCK_TILED_BACKING_STORE
    return m_widget->page();
#else
    return m_widget->wrtPage();
#endif
  }

  void GWebContentView::setActions(){

      QAction* action;
      action = m_actions["zoomIn"] =  new QAction("zoomIn", this);
      action->setObjectName("zoomIn");
      connect(action, SIGNAL(triggered()), this, SLOT(zoomIn()));

      action = m_actions["zoomOut"] = new QAction("zoomOut", this);
      action->setObjectName("zoomOut");
      /* Disable zoomOut action initially as we are the the minimum scale */
      /* Revisit this to determine whether we can use the change signal to
       * set the zoomOut button image initially as well
       */
      action->setEnabled(false);
      connect(action, SIGNAL(triggered()), this, SLOT(zoomOut()));

      action = m_actions["zoom"] = new QAction("zoom", this);
      action->setObjectName("zoom");


      action =  m_actions["winview"]=  new QAction("winview", this);
      action->setObjectName("winview");


      action = m_actions["menu"] =  new QAction("menu", this);
      action->setObjectName("menu");

      action = m_actions["mostvisited"] =  new QAction("mostvisited", this);
      action->setObjectName("mostvisited");

      action = m_actions["toggletb"] = new QAction("toggletb", this);
      action->setObjectName("toggletb");

  }

  void GWebContentView::triggerAction(const QString & action)
  {
    QWebPage::WebAction webAction;
    if (action=="Stop")
      webAction = QWebPage::Stop;
    else if (action=="Back")
      webAction = QWebPage::Back;
    else if (action=="Reload")
      webAction = QWebPage::Reload;
    else if (action=="Forward")
      webAction = QWebPage::Forward;
    else
      return;
    webWidget()->triggerPageAction(webAction);
  }

  void GWebContentView::deactivateZoomActions()
  {
    m_actions.value("zoomIn")->setEnabled(false);
    m_actions.value("zoomOut")->setEnabled(false);
  }

  void GWebContentView::changeZoomAction(qreal zoom){
  #ifndef BEDROCK_TILED_BACKING_STORE
    if (!webWidget()) return;
    if (!(webWidget()->isUserScalable() ) ){
        deactivateZoomActions();
    }
    else {
      QAction * zoomOutAction = m_actions.value("zoomOut");
      QAction * zoomInAction = m_actions.value("zoomIn");
      if (zoom <= webWidget()->minimumScale() ) {
         zoomOutAction->setEnabled(false);
      }
      else {
         zoomOutAction->setEnabled(true);
      }

      if (zoom >= webWidget()->maximumScale()  ){
         zoomInAction->setEnabled(false);
      }
      else {
         zoomInAction->setEnabled(true);
      }
    }
#endif
  }

  void GWebContentView::setZoomFactor(qreal factor){
    if (webWidget())
#ifdef BEDROCK_TILED_BACKING_STORE
        webWidget()->setScale(factor);
#else
        webWidget()->setZoomFactor(factor);
#endif
  }

#ifdef BEDROCK_TILED_BACKING_STORE
QGraphicsWidget* GWebContentView::webWidgetConst()  {
    return m_widget;
}

void GWebContentView::setZoomActions(bool enableZoomIn, bool enableZoomOut)
{
    m_actions.value("zoomIn")->setEnabled(enableZoomIn);
    m_actions.value("zoomOut")->setEnabled(enableZoomOut);
}
  qreal GWebContentView::getZoomFactor(){
    return webWidgetConst() ? webWidgetConst()->scale() : 0.0;
  }

#else
  qreal GWebContentView::getZoomFactor() const {
    return webWidgetConst() ? webWidgetConst()->zoomFactor() : 0.0;
  }
#endif
  
void GWebContentView::activate() {
#ifdef BEDROCK_TILED_BACKING_STORE
    WRT::WrtBrowserContainer* newPage = WebPageController::getSingleton()->currentPage();
    changeContentViewZoomInfo(newPage);
#endif
    ControllableViewBase::activate();
  }

  void GWebContentView::deactivate() {
#ifdef BEDROCK_TILED_BACKING_STORE
    if (!currentPageIsSuperPage()) {
        WRT::WrtBrowserContainer* currentPage = WebPageController::getSingleton()->currentPage();
        currentPage->setPageZoomMetaData(m_widget->currentPageInfo());
    }
#endif
      ControllableViewBase::deactivate();
  }

//  static void appendAction(QWebPage* page, QList<QAction*> &list, enum QWebPage::WebAction webAction, const QString &name) {
//      QAction *action = page->action(webAction);
//      if (action) {
//          action->setObjectName(name);
//          list.append(action);
//      }
//  }

  /*!
    Return the list of public QActions most relevant to the view's current context.
    @return  List of public actions
  */
  QList<QAction *> GWebContentView::getContext()
  {
      // Get some of the actions from the page (there are many more available) and build a list
      // list of them.

      QList<QAction*> actions;
      QAction * action = m_actions.value("zoomIn");


      /* Add zoomIn and zoomOut actions created earlier*/
      if (action) actions.append(action);

      action = m_actions.value("zoomOut");
      if (action) actions.append(action);

      return actions;
  }


  QAction * GWebContentView::getAction(const QString & action)
  {
    QAction * act = NULL;
    if (m_actions.contains(action) )
        act = m_actions.value(action);

    if (!act ) {
        WebPageController * pageController =  WebPageController::getSingleton();
        if (action == "Back")
          act = pageController->getActionBack();
        else if (action == "Forward")
          act = pageController->getActionForward();
        else if (action == "Stop")
          act = pageController->getActionStop();
        else if (action == "Reload")
          act = pageController->getActionReload();

    }
    return act;
  }

  void GWebContentView::scrollViewBy(int dx, int dy)
  {
      wrtPage()->mainFrame()->scroll(dx, dy);
  }

  void GWebContentView::scrollViewTo(int x, int y)
  {
      wrtPage()->mainFrame()->setScrollPosition(QPoint(x, y));
  }


  void GWebContentView::showMessageBox(WRT::MessageBoxProxy* proxy)
  {
  /*
      QMessageBox msgBox(this);
      msgBox.setText(proxy->m_text);
      msgBox.setInformativeText(proxy->m_informativeText);
      msgBox.setDetailedText(proxy->m_detailedText);
      msgBox.setStandardButtons(proxy->m_buttons);
      msgBox.setDefaultButton(proxy->m_defaultButton);
      msgBox.setIcon(proxy->m_icon);
      int ret = msgBox.exec();
      */
      QString displayText = proxy->m_text + QLatin1String("\n") + QLatin1String("\n")+ proxy->m_detailedText + QLatin1String("\n") + QLatin1String("\n") + proxy->m_informativeText;
      int ret = QMessageBox::warning(0/* TODO: find appropriate widget if required or just remove this widget()*/,
                                     proxy->m_text, displayText, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
      proxy->onMessageBoxResponse(ret);
  }

  void GWebContentView::loadUrlToCurrentPage(const QString & url)
  {
      WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
      QString bookmark_url = url;
      if(!bookmark_url.contains(KBOOKMARKURLFILESLASH)){
         if(bookmark_url.contains(KBOOKMARKURLFILE))
            bookmark_url.replace(QString(KBOOKMARKURLFILE), QString(KBOOKMARKURLFILESLASH));
      }
      //--DeCoding URL for DoubleQuote and BackSlash--
      if (bookmark_url.contains(KENCODEDBACKSLASH, Qt::CaseInsensitive))
          bookmark_url.replace(QString(KENCODEDBACKSLASH), QString("\\"));
      if (bookmark_url.contains(KENCODEDDOUBLEQUOTE, Qt::CaseInsensitive))
          bookmark_url.replace(QString(KENCODEDDOUBLEQUOTE), QString("\""));
      
      if (activePage) {
          activePage->mainFrame()->load(bookmark_url);
      }
  }

  QWebPage *GWebContentView::currentPage() {
      WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
      return activePage;
  }

  int GWebContentView::contentWidth()
  {
    return currentPage()->mainFrame()->contentsSize().width();
  }

  int GWebContentView::contentHeight()
  {
    return currentPage()->mainFrame()->contentsSize().height();
  }

  void GWebContentView::stop()
  {
    webWidget()->stop();
  }

  void GWebContentView::back()
  {
    webWidget()->back();
  }

  void GWebContentView::forward()
  {
    webWidget()->forward();
  }

  void GWebContentView::reload()
  {
    webWidget()->reload();
  }

  int GWebContentView::scrollX()
  {
    return currentPage()->mainFrame()->scrollPosition().x();
  }

  int GWebContentView::scrollY()
  {
    return currentPage()->mainFrame()->scrollPosition().y();
  }

  void GWebContentView::zoom(bool in)
  {
    m_zoomIn = in;
    if (!m_timeLine) {
      m_timeLine = new QTimeLine();
      connect(m_timeLine, SIGNAL(valueChanged(qreal)),
        this, SLOT(updateZoom(qreal)));
    }
    else {
      m_timeLine->stop();
    }
    m_timeLine->start();
  }

  void GWebContentView::toggleZoom(){
    zoom(!m_zoomIn);
  }
  void GWebContentView::stopZoom() {
    m_timeLine->stop();
  }

  void GWebContentView::updateZoom(qreal delta){
    Q_UNUSED(delta)
#ifndef BEDROCK_TILED_BACKING_STORE
    if (m_zoomIn)
      zoomBy(0.1);
    else
      zoomBy(-0.1);
#endif
  }

  void GWebContentView::onUrlChanged(const QUrl& url)
  {
    emit urlChanged(url.toString());
    //NB: Brief hack
    if (url.scheme()=="https")
      emit secureConnection(true);
    else
      emit secureConnection(false);
  }

  GSuperWebPage *GWebContentView::createSuperPage(const QString &name, bool persist) {
    GSuperWebPage *page = 0;
    PageMap::iterator it = m_superPages.find(name);
    // If page doesn't exist create a new one.
    if (it == m_superPages.end()) {

      if (persist || !m_sharedPage){
          if (persist)
              //Implicitly creates a new QWebPage that is owned by the superpage object
              page = new GSuperWebPage(0, chrome());

          else {
              //Create a shared QWebPage that is owned by this view
              m_sharedPage = new WebPageWrapper(this, "Superpage javascript error");
              page = new GSuperWebPage(m_sharedPage, chrome());
          }
          //No scroll bars
          page->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
          page->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
          //No history for super pages
          page->page()->history()->setMaximumItemCount(0);
          //Limit page cache usage by super pages

          //NB: needed?
          QPalette viewPalette = widget()->palette();
          viewPalette.setBrush(QPalette::Base, Qt::white);
          page->page()->setPalette(viewPalette);
      } else {
          //Reuse the shared QWebPage
          page = new GSuperWebPage(m_sharedPage, chrome());
      }
      //Export the superpage to JS
      page->setParent(jsObject());
      page->setObjectName(name);
      // Add it to the superpage list.
      m_superPages[name] = page;
      if (m_superPages.count() == 1) {
        // This is the only superpage, make it current.
        m_currentSuperPage = m_superPages.find(name);
      }
    }
    else {
      qDebug() << "GWebContentView::createPage: page already exists: " << name;
    }
    //Create a controllable view onto the super page so that it can be controlled by the view controller.
    m_chrome->addView(new SuperPageView(this, page, name));
    return page;
  }

  void GWebContentView::destroySuperPage(const QString &name) {
    if (!m_superPages.contains(name)) return;

    GSuperWebPage *page = m_superPages.take(name);
    //This will destroy any QWebPage owned by the super page. The shared page is not
    //owned by the super page and will be deleted when the GWebContentView is deleted.
    if (page){
        delete page;
    }
  }

  bool GWebContentView::setCurrentSuperPage(const QString &name) {
    PageMap::iterator it = m_superPages.find(name);
    if (it != m_superPages.end()) {
      m_currentSuperPage = it;
      return true;
    }
    else {
      qDebug() << "GWebContentView::setCurrentSuperPage: not found: " << name;
      return false;
    }
  }

  bool GWebContentView::isSuperPage(const QString &name){
      PageMap::iterator it = m_superPages.find(name);
      if (it != m_superPages.end()) {
        return true;
      }
      qDebug() << "GWebContentView::superPage: not found: " << name;
      return false;
 }


  GSuperWebPage * GWebContentView::superPage(const QString &name) {
      PageMap::iterator it = m_superPages.find(name);
      if (it != m_superPages.end()) {
          return it.value();
      }
      qDebug() << "GWebContentView::superPage: not found: " << name;
      return 0;
  }

  void GWebContentView::showSuperPage(const QString &name) {
      if(isSuperPage(name)) {
          if(currentPageIsSuperPage()) {
              currentSuperPage()->onHidden();
          }

          setCurrentSuperPage(name);
          m_currentPageIsSuperPage = true;
#ifndef BEDROCK_TILED_BACKING_STORE
          webWidget()->setPage(currentSuperPage()->page());
          m_touchNavigation->setPage(webWidget()->page());
          m_touchNavigation->setWantSlideViewCalls(false);
          webWidget()->page()->setPreferredContentsSize(webWidget()->size().toSize());
#else
          WRT::WrtBrowserContainer *pg = WebPageController::getSingleton()->currentPage();
          pg->setPageZoomMetaData(m_widget->currentPageInfo());
          m_widget->setPage(currentSuperPage()->page());
          m_widget->showPage(true);
#endif  
          emit superPageShown(name);
          currentSuperPage()->onShown();
      }
      else {
          qDebug() << "GWebContentView::showSuperPage: error, not found: " << name;
      }
  }

  void GWebContentView::showNormalPage() {
    if (webWidget()) {
      //TODO: Further testing is needed to show if the following is a net benefit.
      if (currentPageIsSuperPage()) {
          if(currentSuperPage()->page() == m_sharedPage){
              //Clear contents of shared page to free resources. This should save
              //memory for big pages, but at the performance cost of loading an empty page.
              //An alternative is to delete the shared page every time and reallocate it when
              //a super page is loaded.
              currentSuperPage()->page()->mainFrame()->setHtml(QString());
          }

          currentSuperPage()->onHidden();
          m_currentPageIsSuperPage =false;
      }
#ifndef BEDROCK_TILED_BACKING_STORE
    webWidget()->showNormalPage();
    webWidget()->setViewportSize();
    m_touchNavigation->setPage(currentPage());
    m_touchNavigation->setWantSlideViewCalls(true);
#else
    m_widget->showPage(false);
#endif
    }
	WRT::WrtBrowserContainer * currPage = WebPageController::getSingleton()->currentPage();
			if(currPage){
			  webWidget()->initializeViewportParams();
			  webWidget()->setZoomFactor(currPage->pageZoomMetaData().zoomValue);
			}
  }

  QObjectList GWebContentView::getSuperPages() {
    QObjectList *result = new QObjectList;
    foreach(GVA::GSuperWebPage *page, m_superPages) {
      result->append(page);
    }
    // TO DO: Need to add result to JS engine so it can be cleaned up.
    return *result;
  }

  bool GWebContentView::currentPageIsSuperPage() const {
    //qDebug() << "GWebContentView::currentPageIsSuperPage: " << webWidget()->page();
    return m_currentPageIsSuperPage;
  }

  void GWebContentView::onLoadStarted()
  {
    //qDebug() << "GWebContentView::onLoadStarted: " << QTime::currentTime();
#if defined(__gva_no_chrome__) || defined(NO_RESIZE_ON_LOAD)
      m_widget->onLoadStarted();
#endif
    emit loadStarted();
  }

  void GWebContentView::onLoadFinished(bool ok)
  {
    //qDebug() << "GWebContentView::onLoadFinished: " << QTime::currentTime();
    emit loadFinished(ok);
    //NB: page manager functions really
    if (!m_backEnabled && currentPage()->history()->canGoBack()){
      m_backEnabled = true;
      emit backEnabled(true);
    }
    else if (m_backEnabled && !currentPage()->history()->canGoBack()){
      m_backEnabled = false;
      emit backEnabled(false);
    }
    if (!m_forwardEnabled && currentPage()->history()->canGoForward()){
      m_forwardEnabled = true;
      emit forwardEnabled(true);
    }
    else if (m_forwardEnabled && !currentPage()->history()->canGoForward()){
      m_forwardEnabled = false;
      emit forwardEnabled(false);
    }

    // Set focus to the Web View so that text boxes have the focus (BR-994) if user is not editing the
    // url field
    if (!WebPageController::getSingleton()->editMode())
        m_widget->setFocus();

#if defined(__gva_no_chrome__) || defined(NO_RESIZE_ON_LOAD)
    m_widget->onLoadFinished();
#endif
  }

  void GWebContentView::dump() {
    qDebug() << "-------------";
    qDebug() << "GWebContentView::dump: " << this;
    qDebug() << "\tcurrent page=: " << currentPage() << " title=" << currentPage()->mainFrame()->title();
    qDebug() << "\tcurrent superpage=: " << currentSuperPage();
    qDebug() << "\tsuperpage count=: " << m_superPages.count();
    foreach(GVA::GSuperWebPage *page, m_superPages) {
      page->dump();
    }
    qDebug() << "GWebContentView::dump: finished";
    qDebug() << "-------------";
  }

//  ControllableView* GWebContentView::createNew(QWidget *parent)
//  {
//      QWebPage* page = BrowserPageFactory::openBrowserPage();
//      return new GWebContentView(page, parent);
//  }


  void GWebContentView::pageCreated(WRT::WrtBrowserContainer* newPage) {

      /* save the page snapshot before changing the current page to the new page*/
      WRT::WrtBrowserContainer * currPage = WebPageController::getSingleton()->currentPage();
      if (currPage) {
          QWebHistoryItem item = currPage->history()->currentItem();
          currPage->savePageDataToHistoryItem(currPage->mainFrame(), &item);
      }

      // When new windows are created from window view, as the content is empty, we should disable
      // zoom actions. Set the user-scalable to false and also init the other zoom params
      // so that even if we change to windows view again without loading a page we are safe.
      // In the code-driven window usecase, this will be overwritten when the page is loaded and setViewportSize is invoked
#ifndef BEDROCK_TILED_BACKING_STORE
      newPage->setPageZoomMetaData(webWidget()->defaultZoomData());
#else
	newPage->setPageZoomMetaData(m_widget->defaultZoomData());
#endif
      /* Set the new page as the current page */
      WebPageController::getSingleton()->setCurrentPage(newPage);

      /* Set the web widget- this one is responsible for webnavigation etc */
      newPage->setWebWidget(webWidget());

#ifndef BEDROCK_TILED_BACKING_STORE
      //connect new page main frame's initialLayoutCompleted with WebContentWidget' setViewportSize SLOT
      connect(newPage->mainFrame(), SIGNAL(initialLayoutCompleted()), webWidget(), SLOT(setViewportSize()));
#endif
  }

  void GWebContentView::updateWebPage(WRT::WrtBrowserContainer* pg)
  {
#ifdef BEDROCK_TILED_BACKING_STORE
      QGraphicsWebView* w = webWidget();
#else
      GWebContentViewWidget* w = webWidget();
#endif
	  
#ifndef BEDROCK_TILED_BACKING_STORE
      w->setPage(pg);
#else	  
	  m_widget->setPage(pg);
#endif
      if (pg)
      {
          pg->setWebWidget(w);

#ifndef BEDROCK_TILED_BACKING_STORE
          // Change navigation also to the current page
          m_touchNavigation->setPage(pg);
          m_touchNavigation->setWantSlideViewCalls(true);
#endif

      }
  }

  void GWebContentView::pageChanged(WRT::WrtBrowserContainer* oldPage, WRT::WrtBrowserContainer* newPage) {
      Q_UNUSED(oldPage)
      updateWebPage(newPage);

#ifndef BEDROCK_TILED_BACKING_STORE
      // Set new page zoom info
      changeContentViewZoomInfo(newPage);
#endif
  }

#ifdef BEDROCK_TILED_BACKING_STORE
  void GWebContentView::handleContextEventObject(QWebHitTestResult* eventTarget)
  {
      WebViewEventContext context(type(), *eventTarget);
      if (currentPageIsSuperPage()) {
          // Let the superpage handle the event.
          currentSuperPage()->onContextEvent(&context);
      }
      else {
          // Send the event directly.
          emit contextEvent(&context);
      }
  }
void GWebContentView::handleViewScrolled(QPoint& scrollPos, QPoint& delta)
{
    if (delta.manhattanLength() && (delta.y() > 0  || scrollPos.y() + delta.y() <= 40) 
        && !currentPageIsSuperPage())
        m_chrome->layout()->slideView(-delta.y());
    
    emit startingPanGesture(1);
}

#endif
  void GWebContentView::changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage){
     // Copy the new page zoom info into cv
#ifndef BEDROCK_TILED_BACKING_STORE
     webWidget()->setPageZoomMetaData(newPage->pageZoomMetaData());

     if (webWidget()->isUserScalable()) {
         webWidget()->setZoomFactor(newPage->mainFrame()->zoomFactor());
     }
     else {
         // Need to call setPageZoomFactor instead of setZoomFactor because setZoomFactor
         // will not do anything if user-scalable is false. But we need to
         // ensure that the correct zoom factor is applied as there is a possibility
         // that we might have been on another page earlier
         webWidget()->setPageZoomFactor(newPage->mainFrame()->zoomFactor());
      }
#else
	 m_widget->setCurrentPageInfo(newPage->pageZoomMetaData());
#endif
  }

  void GWebContentView::setJSObject(const QString &objectName) {
      m_jsObject = new GWebContentViewJSObject(this, m_chrome->page()->mainFrame(), objectName);
  }


  bool GWebContentView::enabled() const {
      return m_enabled;
  }

  void GWebContentView::setEnabled(bool value) {
      if(m_enabled == value)
          return;

      m_enabled = value;
      widget()->setEnabled(value);
      widget()->update();
  }
  void GWebContentView::setSavedZoomValueInView(qreal zoomValue){
	  m_savedZoomValueInView = zoomValue;
  }
  
  qreal GWebContentView::getSavedZoomValueInView() const{
      return m_savedZoomValueInView;
  }
} // end of namespace GVA


