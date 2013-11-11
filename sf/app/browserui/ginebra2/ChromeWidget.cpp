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

#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include <QList>
#include <QKeyEvent>
#include <QDebug>
#include <QDesktopServices>

#include "bedrockprovisioning.h"
#include "ChromeWidgetJSObject.h"
#include "ChromeLayout.h"
#include "ChromeRenderer.h"
#include "ChromeDOM.h"
#include "Snippets.h"
#include "ChromeEffect.h"
#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "WebChromeContainerSnippet.h"
#include "Application.h"
#include "ViewController.h"
#include "ViewStack.h"
//#include "CollapsingWidget.h"
#include "SlidingWidget.h"
#include "GWebPage.h"
#include "webpagecontroller.h"
//#include "ViewStack.h"
#include "HistoryManager.h"
#include "bookmarkscontroller.h"
#include "ScriptObjects.h"
#include "LocaleDelegate.h"
#include "DeviceDelegate.h"
#include "NetworkDelegate.h"
#include "ObjectCharm.h"
#include "bedrockprovisioning.h"
#include "Utilities.h"
#include "PopupWebChromeItem.h"
#ifdef QT_MOBILITY_SYSINFO
#include "SystemDeviceImpl.h"
#include "SystemNetworkImpl.h"
#endif

#ifdef QT_MOBILITY_SERVICE_FRAMEWORK
#include "hsbookmarkpublishclient.h"
#endif // QT_MOBILITY_SERVICE_FRAMEWORK

#include "Downloads.h"

#include "wrtbrowsercontainer.h"
#include "webpagecontroller.h"

#include "GAlternateFileChooser.h"

namespace GVA {

// -----------------------------

  ChromeWidget::ChromeWidget(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QObject(0),
      m_renderer(0),
      m_dom(0),
      m_viewController(new ViewController()),
      m_jsObject(new ChromeWidgetJSObject(0, this)),
      m_localeDelegate(new LocaleDelegate(this)),
      m_downloads(new Downloads())
  {
    m_layout = new ChromeLayout(parent, wFlags);
    QObject::connect(m_layout, SIGNAL(resizing(QSizeF)), this, SLOT(onResize(QSizeF)));
    QObject::connect(m_layout, SIGNAL(aspectChanged(int)), this, SLOT(onAspectChanged(int)));

    DeviceImpl *deviceImpl = new DEVICEIMPL();
    NetworkImpl *networkImpl = new NETWORKIMPL();
    m_deviceDelegate = new DeviceDelegate(deviceImpl);
    m_networkDelegate = new NetworkDelegate(networkImpl);

    BEDROCK_PROVISIONING::BedrockProvisioning *provisioning = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning();
    ChromeEffect::disabledColor.setNamedColor(provisioning->valueAsString("DisabledColor", "#FFFFFF"));
    ChromeEffect::disabledOpacity = static_cast<qreal>(provisioning->valueAsString("DisabledOpacity", "0.65").toFloat());

#ifndef __gva_no_chrome__
    m_snippets = new Snippets(this, this);
#endif
#ifndef __gva_no_chrome__
    m_page =  static_cast<QWebPage *>(new GVA::WebPageWrapper(this, "Chrome Javascript error"));
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
#endif
    m_viewController->setObjectName("views");

    connect(m_viewController, SIGNAL(currentViewChanged()), this, SLOT(onCurrentViewChanged()));
#ifndef __gva_no_chrome__

    m_jsObject->setObjectName("chrome");

    // Pass some signals from this object to the Javascript object.
    QObject::connect(this, SIGNAL(chromeComplete()), m_jsObject, SIGNAL(chromeComplete()));
    QObject::connect(this, SIGNAL(aspectChanged(int)), m_jsObject, SIGNAL(aspectChanged(int)));
    QObject::connect(this, SIGNAL(prepareForGeometryChange()), m_jsObject, SIGNAL(prepareForGeometryChange()));
    QObject::connect(this, SIGNAL(symbianCarriageReturn()), m_jsObject, SIGNAL(symbianCarriageReturn()));
    QObject::connect(this, SIGNAL(popupShown(const QString &)), m_jsObject, SIGNAL(popupShown(const QString &)));
    QObject::connect(this, SIGNAL(popupHidden(const QString &)), m_jsObject, SIGNAL(popupHidden(const QString &)));

    //addJSObjectToEngine(this);

    m_app = new GinebraApplication();
    QObject::connect(this, SIGNAL(goToBackground()), m_app, SLOT(sendToBackground()));

    //addJSObjectToEngine(m_app);

    QObject::connect(
            WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer*)),
            this, SLOT(pageCreated(WRT::WrtBrowserContainer*)));

    QObject::connect(m_page, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    QObject::connect(m_page, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    QObject::connect(m_page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(exportJSObjects()));

#endif

    ViewStack * vs = ViewStack::getSingleton();
    vs->setViewController(m_viewController);
    vs->setChromeWidget(this);

    // TO DO: need a better home for this.
    qMetaTypeId<QObjectList>();
    qRegisterMetaType<QObjectList>("QObjectList");
	
	//for QA Automation test tool purpose
#if !defined(QT_NO_LIBRARY)
    QLibrary testLib("qttestability");
    if(testLib.load()){
        typedef void (*TasInitialize)(void);
        TasInitialize initFunction = (TasInitialize)testLib.resolve("qt_testability_init");
#ifdef Q_OS_SYMBIAN
        //not found so use ordinal
        if(!initFunction){
            initFunction = (TasInitialize)testLib.resolve("1");
        }
#endif
        if(initFunction){
            initFunction();
        }
    }
#endif
  }

  ChromeWidget::~ChromeWidget()
  {
    delete m_viewController;
    delete m_jsObject;
    delete m_layout;
    delete m_renderer;
    delete m_dom;
    delete m_page;
    delete m_snippets;
    delete m_localeDelegate;
    delete m_deviceDelegate;
    delete m_networkDelegate;
    delete m_app;
    delete m_downloads;
  }

  //Handle resizing signal from layout

  void ChromeWidget::onResize(QSizeF size)
  {
#ifndef __gva_no_chrome__
    if (m_dom && m_renderer) {
      emit prepareForGeometryChange();
      m_renderer->resize(size);
    }
#endif
  }

  //Handle aspectChanged signal from layout

  void ChromeWidget::onAspectChanged(int aspect)
  {
    QString mode = (aspect == landscape ? "Landscape" : "Portrait");
    ControllableViewBase* cview = m_viewController->currentView();
    if (cview)
      cview->displayModeChanged(mode);
    emit aspectChanged(aspect);
  }

  void ChromeWidget::updateChromeLayout() {
    m_renderer->updateChromeLayout();
  }


  void ChromeWidget::loadUrlToCurrentPage(const QUrl & url)
  {
    WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();

    if (activePage) {
      activePage->mainFrame()->load(url.toString());
    }
  }

  void ChromeWidget::pageCreated(WRT::WrtBrowserContainer * page)
  {
#ifdef Q_OS_SYMBIAN
    QString path = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
    GAlternateFileChooser * chooser = new GAlternateFileChooser(path);
    page->setFileChooser(chooser); // chooser is now owned by page
#endif
    m_downloads->handlePage(page);
  }

  void ChromeWidget::setChromeBaseDirectory(const QString dir) {
    m_baseDirectory = dir;
    if(m_baseDirectory.left(2) == ":/") // resource path have to be converted to resource url
        m_baseDirectory = "qrc:///" + m_baseDirectory.mid(2);
  }

  void ChromeWidget::setChromeFile(const QString filePath)
  {
#ifndef __gva_no_chrome__
    m_page->mainFrame()->load(QUrl(m_baseDirectory + filePath));
#else
    Q_UNUSED(filePath)
#endif
  }

  void ChromeWidget::reloadChrome()
  {
    clearChrome();
    m_page->triggerAction(QWebPage::Reload);
  }

  // TODO: needed?
  void ChromeWidget::addViewToLayout(ControllableViewBase * controllableView){
    m_layout->addView(controllableView);
  }

  void ChromeWidget::addView(ControllableViewBase * controllableView) {
    //qDebug() << "ChromeWidget::addView: " << controllableView->widget();
    m_viewController->addView(controllableView);
  }
  
  void ChromeWidget::anchorTogether(ChromeSnippet* first, const QString& secondId, qreal x, qreal y)
  {
    ChromeSnippet* second = getSnippet(secondId);
    if (second){
      m_layout->anchorTogether(first, second, x, y);
    }
    else {
      qDebug() << "Chrome::anchorTogether: error, not found: " << secondId;
    }
  }

  ControllableViewBase * ChromeWidget::getView(const QString& view)
  {
    return m_viewController->view(view);
  }

  void ChromeWidget::showView(const QString &name) {
    m_viewController->showView(name);
  }

  void ChromeWidget::onCurrentViewChanged() {
    m_layout->addView(m_viewController->currentView());
  }

  // Clean up all existing snippets;

  void ChromeWidget::clearChrome()
  {
    m_snippets->clear();
  }

  void ChromeWidget::loadStarted() // slot
  {
    clearChrome();
  }

  void ChromeWidget::loadFinished(bool ok)  // slot
  {
    if (!ok) {
      return;
    }
    if (!m_renderer)
      m_renderer = new ChromeRenderer(m_page, this);
    m_renderer->resize(m_layout->size());
    if (m_dom)
      delete m_dom; // NB: This may need some further investigation
    m_dom = new ChromeDOM(m_page, this);
    getInitialSnippets();
    m_renderer->resize(QSizeF(m_layout->size().width(), m_dom->height()));
    //qDebug() << m_dom->getCacheableScript();
    // Let internal objects know that the chrome is complete.
    emit internalChromeComplete();
    // Now let the javascript world know that it is complete.
    emit chromeComplete();
    // connect ViewStack to creatingPage signal
    connect( WebPageController::getSingleton(), SIGNAL(creatingPage(WRT::WrtBrowserContainer*)),
             ViewStack::getSingleton(), SLOT(creatingPage(WRT::WrtBrowserContainer*)));
  }

  void ChromeWidget::chromeInitialized()
  {
    //NB: Don't want to implement this, but just in case
  }

  void ChromeWidget::exportJSObjects()
  {
    exportJSObjectsToPage(m_page);
  }

  void ChromeWidget::exportJSObjectsToPage(QWebPage *page) {
    addJSObjectToPage(m_jsObject, page);
    addJSObjectToPage(m_snippets, page);
    addJSObjectToPage(m_app, page);
    addJSObjectToPage(m_viewController, page);
    addJSObjectToPage(WebPageController::getSingleton(), page);
    addJSObjectToPage(BookmarksController::getSingleton(), page);
    addJSObjectToPage(WRT::HistoryManager::getSingleton(), page);
    addJSObjectToPage(ViewStack::getSingleton(), page);
    addJSObjectToPage(m_localeDelegate, page);
    addJSObjectToPage(m_deviceDelegate, page);
    addJSObjectToPage(m_networkDelegate, page);
#ifdef QT_MOBILITY_SERVICE_FRAMEWORK
    addJSObjectToPage(&(WRT::HsBookmarkPublishClientSingleton::Instance()), page);
#endif // QT_MOBILITY_SERVICE_FRAMEWORK
    // Dynamically added objects
    //foreach(QObject * jsObj, m_jsObjects) {
    //  addJSObjectToPage(jsObj, page);
    // }
    addJSObjectToPage(m_downloads, page);
  }

  void ChromeWidget::getInitialSnippets()
  {
    //TODO: get the list of containers form m_dom (via new method to be added).
    QList <QWebElement> initialSnippets = m_dom->getInitialElements();
    foreach(QWebElement element, initialSnippets) {
      ChromeSnippet * s = getSnippet(element.attribute("id"));
      if (s && s->initiallyVisible())
        s->setVisible(true);
    }
  }

  //TODO: check new logic
 
  void ChromeWidget:: addSnippet(ChromeSnippet * snippet, const QString & docElementId )
  {
    m_snippets->addSnippet(snippet, docElementId);
    ChromeSnippet * container = 0;
    if (!snippet->parentId().isNull()){
      container = getSnippet(snippet->parentId());
    }
    m_layout->addSnippet(snippet, container);
  }

  //NB: This really shouldn't be necessary: anchor bars should be implemented as an expanding
  //widget class !!!!!!

  void ChromeWidget::adjustAnchorOffset(ChromeSnippet * snippet, qreal delta)
  {
    m_layout->adjustAnchorOffset(snippet, delta);
  }

  ChromeSnippet *ChromeWidget::getSnippet(const QString & docElementId, QGraphicsItem * parent) {
    ChromeSnippet *result = m_snippets->getSnippet(docElementId);
    if (!result){
      result = m_dom->getSnippet(docElementId, parent);
      if (result) {
        result->setParent(m_snippets); // Exports to "Snippets" JS object
        addSnippet(result, docElementId);
      }
      else{
        qDebug() << "Snippet not found: " << docElementId;
        return 0;
      }
    }else{
      //qDebug() << "Found existing snippet: " << docElementId;
    }

    return result;
  }

  QRect ChromeWidget::getSnippetRect(const QString &docElementId)
  {
    return m_dom->getElementRect(docElementId);
  }

  void ChromeWidget::addJSObjectToWindow(QObject *object)
  {
    m_page->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
  }

  void ChromeWidget::addJSObjectToPage(QObject *object, QWebPage *page)
  {
    page->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
  }

  QObjectList ChromeWidget::getSnippets() {
    return m_snippets->getList();
  }

  void ChromeWidget::alert(const QString & msg) {
    // To do: open a dialog box showing msg.
    qDebug() << msg;
  }

  void ChromeWidget::onViewInstantiated(ControllableViewBase *view) {   // slot
    addViewToLayout(view);
  }

  QObject*  ChromeWidget::getDisplaySize() const
  {
    ScriptSize * sz = new ScriptSize(m_layout->size().toSize());
    m_page->mainFrame()->addToJavaScriptWindowObject("size", sz, QScriptEngine::ScriptOwnership);
    return sz;

  }

  /*
  void ChromeWidget::addJSObjectToEngine(QObject *object)
  {
    QScriptValue val = m_engine.newQObject(object);
    m_engine.globalObject().setProperty(object->objectName(), val);
  }

  QScriptValue ChromeWidget::evalWithEngineContext(const QString& program)
  {
    return m_engine.evaluate(program);
  }
  */

  void ChromeWidget::emitPopupShown(const QString &popupId) 
  {
      emit popupShown(popupId);
  }

  void ChromeWidget::emitPopupHidden(const QString &popupId) 
  {
      emit popupHidden(popupId);
  }


  void ChromeWidget::dump() {
    qDebug() << "---------------------";
    qDebug() << "ChromeWidget::dump";
    m_snippets->dump();
    m_viewController->dump();
    //WebPageController::getSingleton()->dump();
    qDebug() << "---------------------";
  }
} // endof namespace GVA
