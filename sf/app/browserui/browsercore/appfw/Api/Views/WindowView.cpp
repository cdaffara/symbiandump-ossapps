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


#include "webpagecontroller.h"
#include "FlowInterface.h"
#include "WindowView_p.h"
#include "WindowView.h"

#include <QWebHistory>
#include <QWebFrame>
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

#include <QDebug>

#define WINDOWVIEW_TIME_TO_TRANSITION 200
#define WINDOWVIEW_TIME_TO_ADD_NEXTPAGE 100
#define WINDOWVIEW_MAX_NUM_WINDOWS 5
namespace WRT {


WindowViewPrivate::WindowViewPrivate(WebPageController * pageMgr,
                                       QWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(parent),
    m_graphicsWidgetParent(0),
    m_pageManager(pageMgr),
    m_mode(0),
    m_state(0)
{
    Q_ASSERT(m_pageManager);
    init();
}

WindowViewPrivate::WindowViewPrivate(WebPageController * pageMgr,
                                       QGraphicsWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(0),
    m_graphicsWidgetParent(parent),
    m_pageManager(pageMgr),
    m_mode(0),
    m_state(0)
{
    Q_ASSERT(m_pageManager);
    init();
}

WindowViewPrivate::~WindowViewPrivate()
{
    delete m_transTimer;
}

void WindowViewPrivate::init()
{
    // create the view's actions
    m_actionForward = new QAction("Forward", m_graphicsWidgetParent);
    m_actionForward->setObjectName("Forward");

    m_actionBack = new QAction("Back", m_graphicsWidgetParent);
    m_actionBack->setObjectName("Back");

    m_actionOK = new QAction("OK", m_graphicsWidgetParent);
    m_actionOK->setObjectName("OK");

    m_actionCancel = new QAction("Cancel", m_graphicsWidgetParent);
    m_actionCancel->setObjectName("Cancel");

    m_actionGoBack = new QAction("goBack", m_graphicsWidgetParent);
    m_actionGoBack->setObjectName("goBack");
    m_actionGoBack->setEnabled(true);

    m_actionAddWindow = new QAction("addWindow", m_graphicsWidgetParent);
    m_actionAddWindow->setObjectName("addWindow");

    m_actionDelWindow = new QAction("delWindow", m_graphicsWidgetParent);
    m_actionDelWindow->setObjectName("delWindow");

}

/*!
 * \class WindowView
 *
 * \brief The base class for the WindowViews
 *
 * This class provides the basic routines to enable navigation amoung multiple pages
 * known by WebPageController.
 *
 * Derived classes (such as WindowFlowView, and WindowLiteView) supply
 * the exact "FlowInterface" to be used, and rely on much of the base-class functionality
 * for signal/slots, extracting thumbnails for each page, etc.
 *
 */

/*!
  Basic WindowView constructor requires a PageManager to manage the pages
  and a parent QWidget

  @param mgr : WebPageController handle for this class
  @param parent : Widget parent for this class
*/
WindowView::WindowView(WebPageController * pageMgr,
                         QWidget* parent) :
    d(new WindowViewPrivate(pageMgr, parent))
{
    init();
}

/*!
  Basic WindowView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget

  Note: This functionality is not yet tested
  @param mgr : WebPageController handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WebPageController
*/
WindowView::WindowView(WebPageController * pageMgr,
                         QGraphicsWidget* parent) :
    d(new WindowViewPrivate(pageMgr, parent))
{
    init();
}

WindowView::~WindowView()
{
//TODO: restoreSlides();
    delete d;
}

/*!
  Retrieve the WebPageController assigned to this view
*/
WebPageController* WindowView::webPageController()
{
    return d->m_pageManager;
}

/*!
  Return the view's Forward QAction
  For scrolling the window view forwards
  @return forward public action
*/
QAction * WindowView::getActionForward()
{
    return d->m_actionForward;
}

/*!
  Return the view's Back QAction
  For scrolling the window view backwards
  @return  back public action
*/
QAction * WindowView::getActionBack()
{
    return d->m_actionBack;
}

/*!
  Return the view's OK QAction
  For invoking the view's OK
  @return OK public action handle
*/
QAction * WindowView::getActionOK()
{
    return d->m_actionOK;
}

/*!
  Return the view's Cancel QAction
  For invoking the view's Cancel
  @return cancel public action
*/
QAction * WindowView::getActionCancel()
{
    return d->m_actionCancel;
}

/*!
  Return the view's add window QAction
  For adding a new window
  @return handle to addWindow action
*/
QAction *  WindowView::getActionAddWindow()
{
    return d->m_actionAddWindow;
}

/*!
  Return the view's delete window QAction
  For deleting a window. This will be dimmed for the last window
  @return  handle to deleteWindow action
*/
QAction *  WindowView::getActionDelWindow()
{
    return d->m_actionDelWindow;
}

/*!
  Return the widget handle of this view
  @return widget handle
*/
QGraphicsWidget* WindowView::widget() const
{
      return d->m_flowInterface;
}

/*!
  Return the title of this view for display
  @return Title String
*/
QString WindowView::title() const
{
    QString title =  qtTrId("txt_browser_windows_windows");

    if(d->m_flowInterface && d->m_pageList)
    {
        int centerIndex = d->m_flowInterface->centerIndex();
        if(centerIndex >= 0 && centerIndex < d->m_pageList->count())
        {
            QString pagetitle(d->m_pageList->at(centerIndex)->mainFrame()->title());
            if(pagetitle.isEmpty())
                title += qtTrId("txt_browser_windows_new_window");
            else
                title += pagetitle;
        }
    }
    return title;
}

/*!
  Test if this  view is active or not
  @return true if view is active else false
*/
bool WindowView::isActive()
{
    return (d->m_state == WindowViewActive ? true : false);
}


/*!
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.
  @ return list of public actions for this view
*/
QList<QAction*> WindowView::getContext()
{
    // for now, all actions valid at all times
    // but there may be some logic here to determine context
    QList<QAction*> contextList;
    contextList <<
        d->m_actionForward <<
        d->m_actionBack <<
        d->m_actionGoBack <<
        d->m_actionOK <<
        d->m_actionCancel <<
        d->m_actionAddWindow <<
        d->m_actionDelWindow;
    return contextList;
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void WindowView::activate()
{
    Q_ASSERT(d->m_state == WindowViewNotActive);

    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->resize(d->m_windowViewSize);
    d->m_flowInterface->init();

    updateWindows();
    setCenterIndex(d->m_pageManager->currentPage());
    updateActions();

    // now forward flow interface's signals
    connect(d->m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    connect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));
    connect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(changedCenterIndex(int)));
    connect(d->m_flowInterface, SIGNAL(removed(int)), this, SLOT(delPage(int)));
    connect(d->m_transTimer, SIGNAL(timeout()), this, SLOT(endAnimation()));

    if (d->m_mode ==  WindowViewModeTransition ) {

        //qDebug() << "Connect to pageCreated " ;
        connect(d->m_pageManager, SIGNAL(creatingPage(WRT::WrtBrowserContainer*)), this, SLOT(onPageCreated(WRT::WrtBrowserContainer*)));
    }

    // FIXME: temporal fix the resize & performance issue caused by the new QGraphicsItem architecture
    widget()->installEventFilter(this);

    d->m_flowInterface->prepareStartAnimation();
    d->m_flowInterface->show();
    d->m_flowInterface->runStartAnimation();

    d->m_state = WindowViewActive;
}

/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void WindowView::deactivate()
{
    //qDebug() << "deactivate " << d->m_mode;
    Q_ASSERT(d->m_state == WindowViewActive);

    if(!d->m_flowInterface)
        return;

    // disconnect signals
    disconnect(d->m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    disconnect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));

    // internally process the index change signal as well
    disconnect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(changedCenterIndex(int)));
    disconnect(d->m_flowInterface, SIGNAL(removed(int)), this, SLOT(delPage(int)));
    disconnect(d->m_transTimer, SIGNAL(timeout()), this, SLOT(endAnimation()));

    if (d->m_mode ==  WindowViewModeTransition ) {
        //qDebug() << "========Disconnect pageCreated ";
        disconnect(d->m_pageManager, SIGNAL(creatingPage(WRT::WrtBrowserContainer*)), this, SLOT(onPageCreated(WRT::WrtBrowserContainer*)));
        // Check if new pages list is empty  - assert  d->m_newPages.count()
    }
    widget()->removeEventFilter(this);

    // Hide and delete flowinterface later when told
    d->m_pageList = NULL;

    // Only needed when using QWidget based view
    //m_proxyWidget->setWidget(0);
    //delete m_proxyWidget;
    //m_proxyWidget = 0;

    // Hide and delete flowinterface
    d->m_flowInterface->hide();
    d->m_flowInterface->deleteLater();
    d->m_flowInterface = NULL;

    d->m_mode =  WindowViewModeNormal;
    d->m_state = WindowViewNotActive;

}

void WindowView::init()
{
    d->m_transTimer = new QTimer(this);

    // auto-link relevant actions to slots
    connect(d->m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    connect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));

    // connect creation signals
    connect(d->m_actionAddWindow, SIGNAL(triggered()), this, SLOT(addPage()));
    connect(d->m_actionDelWindow, SIGNAL(triggered()), this, SLOT(delPage()));
}

void WindowView::setMode(Mode m)
{
    d->m_mode = m;
}

void WindowView::setSize(QSize& size)
{
    d->m_windowViewSize = size;
}

void WindowView::setCenterIndex()
{
    WrtBrowserContainer * p  = d->m_pageManager->currentPage();
    setCenterIndex(p);
}

void WindowView::setCenterIndex(int i)
{
    if(d->m_flowInterface)
        d->m_flowInterface->setCenterIndex(i);
}

void WindowView::setCenterIndex(WrtBrowserContainer * p)
{
    int currIndex = d->m_pageList->indexOf(p);
    if(currIndex >= 0)
        setCenterIndex(currIndex);
}

void WindowView::displayModeChanged(QString& newMode)
{
    //qDebug() << "WindowView::displayModeChanged:::" << newMode;
    // update page thumbnails
    QSize s = d->m_flowInterface->size().toSize();

    d->m_pageManager->resizeAndUpdatePageThumbnails(s);

    // update all the window images
    updateImages();

    // update the flow interface
    d->m_flowInterface->displayModeChanged(newMode);
}

void WindowView::updateImages()
{
    Q_ASSERT(d && d->m_flowInterface);

    // clear PictureFlow
    if (d->m_flowInterface->slideCount() != 0)
        d->m_flowInterface->clear();

    d->m_pageList = d->m_pageManager->allPages();
    for (int i = 0; i < d->m_pageList->count(); i++) {
        WrtBrowserContainer* window = d->m_pageList->at(i);
        QString title = window->pageTitle();
        if (title.isEmpty())
            title =  qtTrId("txt_browser_windows_new_window");

         QWebHistoryItem item = window->history()->currentItem();
         WebPageData data = item.userData().value<WebPageData>();
         QImage img = data.m_thumbnail;

         QSize size = window->webWidget()->size().toSize();
         QSize imgSize = img.size();
         float ratio = (float)size.width() / (float)size.height();
         float imgRatio = (float)imgSize.width() / (float)imgSize.height();
         if (imgRatio != ratio) {
             size.scale(imgSize, Qt::KeepAspectRatio);
             img = img.copy(0, 0, size.width(), size.height());
         }
#ifdef BROWSER_LAYOUT_TENONE
         d->m_flowInterface->addSlide(img);
#else
         d->m_flowInterface->addSlide(img, title);
#endif
     }
     setCenterIndex(d->m_pageManager->currentPage());
}

void WindowView::updateWindows()
{
    if (!d->m_flowInterface)
        return;

    // update page thumbnails
    d->m_pageManager->updatePageThumbnails();

    // update all the images
    updateImages();
}

/*!
  scroll forward in the window view
*/
void WindowView::forward()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showNext();
}

/*!
  scroll back in the window view
*/
void WindowView::back()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showPrevious();
}

/*!
  animate from page "from" to page "to"
  @param from : from page handle
  @param to   : to page handle
*/
void  WindowView::animate(WrtBrowserContainer * from, WrtBrowserContainer * to)
{
    // fetch the indices for the given pages
    qDebug() << "This function is not implemented.";
}

void WindowView::changedCenterIndex(int index)
{
    switch (d->m_state) {
        case WindowViewActive:
            indexChangeInActiveState(index);
            break;
        case WindowViewAddPage:
            addPageCplt(index);
            break;
        case WindowViewDeletePage:
            delPageCplt(index);
            break;
        default:
            /* Shouldn't get here */
            break;
    }
}

void WindowView::indexChangeInActiveState(int index)
{
    if (d->m_mode ==  WindowViewModeNormal ) {
        WrtBrowserContainer* page = d->m_pageList->at(index);
        d->m_pageManager->setCurrentPage(page);

        /* Set the new page as the center page */
        emit centerIndexChanged(index);

        updateActions();
    }
    else {
        //qDebug() << "Start Adding pages " << "Pages in list" <<  d->m_newPages.count() << "Page added " << d->m_newPages.at(0);
        addPage(d->m_newPages.takeFirst());
    }
}


void WindowView::updateActions()
{
    int centerIndex = d->m_flowInterface->centerIndex();
    bool animActive = d->m_flowInterface->slideAnimationOngoing();

    d->m_actionForward->setEnabled( !animActive && (centerIndex < (d->m_pageManager->pageCount() - 1)) );
    d->m_actionBack->setEnabled( !animActive && (centerIndex != 0) );

    d->m_actionOK->setEnabled(!animActive);
    d->m_actionCancel->setEnabled(!animActive);

    d->m_actionDelWindow->setEnabled( !animActive && (d->m_pageManager->pageCount() > 1) );
    d->m_actionAddWindow->setEnabled(!animActive && (d->m_pageManager->pageCount() < WINDOWVIEW_MAX_NUM_WINDOWS));
}

void WindowView::okTriggered(int index)
{
    Q_UNUSED(index);
    Q_ASSERT(d->m_flowInterface);
    connect(d->m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(okTriggeredCplt()));
    d->m_flowInterface->runEndAnimation();
}

void WindowView::okTriggeredCplt()
{
    Q_ASSERT(d->m_flowInterface);

    disconnect(d->m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(okTriggeredCplt()));
    int index = d->m_flowInterface->centerIndex();

    if(d->m_flowInterface && d->m_pageList)
    {
        if(index >= 0 && index < d->m_pageList->count())
        {
        	  WrtBrowserContainer* page = d->m_pageList->at(index);
        	  // If mainframe URL is empty, we are restoring page
        	  // Page needs to be reloaded
        	  if (page->mainFrame()->url().isEmpty()){
        	  	QWebHistoryItem item = page->history()->currentItem();
        	  	if (item.isValid()) page->history()->goToItem(item);
        	  }
            emit ok(d->m_pageList->at(index));
        }
    }
}

void WindowView::addPage(WrtBrowserContainer* pg)
{
    Q_ASSERT(d->m_flowInterface);

    //qDebug() << "WindowView::addPage: COUNT "<< d->m_newPages.count();
    if (d->m_flowInterface->slideAnimationOngoing() || (d->m_mode ==  WindowViewModeNormal && d->m_state == WindowViewAddPage))
        return;

    d->m_state = WindowViewAddPage;

    // insert an empty image after index
    // the insert function will activate the add-page animation which is built-in in FilmstripFlow
    QImage img;
    QString title= "";
    if (pg && !pg->mainFrame()->requestedUrl().isEmpty() ) {
         if (!pg->mainFrame()->title().isEmpty())
             title = pg->mainFrame()->title();
         else  if (!pg->mainFrame()->url().isEmpty() )
             title = d->m_pageManager->partialUrl(pg->mainFrame()->url());
         else
             title = d->m_pageManager->partialUrl(pg->mainFrame()->requestedUrl());

         //qDebug() << "WindowView::addPage - Title " << pg->mainFrame()->title() << "Url : "<<  pg->mainFrame()->url().toString()<< "Requested Url : " << pg->mainFrame()->requestedUrl().toString()  ;
    }

    int index = d->m_flowInterface->centerIndex();
    d->m_flowInterface->insert(index + 1, img, title);

    updateActions();
}

void WindowView::addPageCplt(int index)
{
    /* Adding a new page is completed when the index reaches the newly added index*/
    Q_ASSERT(d->m_state == WindowViewAddPage);


    //qDebug() << " WindowView::addPageCplt: index " << index << "add new page" << d->m_newPages.count();
    /* If new pages were added, show them before transitioning back to content view */
    if (d->m_newPages.count() ) {

        //qDebug() << " WindowView::addPageCplt: index " << index << "add new page" << d->m_newPages.count();
        QTimer::singleShot(WINDOWVIEW_TIME_TO_ADD_NEXTPAGE, this, SLOT(addNextPage()));

    }
    else {
        connect(d->m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(addPageCplt()));

        // Start a timer so that the new blank window is shown before transition starts
        d->m_transTimer->start(WINDOWVIEW_TIME_TO_TRANSITION);

    }

    emit pageAdded();
}

void WindowView::addNextPage()
{
    //qDebug() << " WindowView::addNextPage " << d->m_newPages.count() << "Added Page " << d->m_newPages.at(0);
    addPage(d->m_newPages.takeFirst());
}

void WindowView::endAnimation()
{

    //qDebug() << "WindowView::endAnimation ";
    d->m_transTimer->stop();
    d->m_flowInterface->runEndAnimation();

}

void WindowView::addPageCplt()
{
    //qDebug() << "WindowView::addPageCplt() " << d->m_newPages.count();
    if (d->m_mode ==  WindowViewModeNormal ) {
        // open a new page
        WRT::WrtBrowserContainer* pg = d->m_pageManager->openPage();
    }
    int index = d->m_flowInterface->centerIndex();
    emit centerIndexChanged(index);

    disconnect(d->m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(addPageCplt()));
    d->m_state = WindowViewActive;
    updateActions();

    if (d->m_mode ==  WindowViewModeTransition ) {
        emit newWindowTransitionComplete();
    }

    emit addPageComplete();
}

void WindowView::delPage()
{
    Q_ASSERT(d->m_flowInterface);

    if (d->m_flowInterface->slideAnimationOngoing())
        return;

    d->m_flowInterface->removeAt(d->m_flowInterface->centerIndex());
}

void WindowView::delPage(int index)
{
    Q_ASSERT(d->m_flowInterface);
    Q_ASSERT(index >= 0 && index < d->m_pageList->count());

    if (d->m_flowInterface->slideAnimationOngoing())
        return;

    d->m_state = WindowViewDeletePage;
    WrtBrowserContainer * p = d->m_pageList->at(index);
    d->m_pageManager->closePage(p);
    updateActions();
}

void WindowView::delPageCplt(int index)
{
    Q_ASSERT(d->m_state == WindowViewDeletePage);

    /* Update the actions */
    updateActions();

    /* Reset the state and newCenterPage */
    d->m_state = WindowViewActive;

    /* Emit centerIndexChanged signal */
    emit centerIndexChanged(d->m_flowInterface->centerIndex());
}


void WindowView::onPageCreated(WRT::WrtBrowserContainer *pg) {

    //qDebug() << "WindowView::onPageCreated" << pg << "Trans Timer active " <<  d->m_transTimer->isActive();
    if (d->m_transTimer->isActive()) {
        // Stop that timer and add the new page
        d->m_transTimer->stop();
        addPage(pg);
    }
    else {

        d->m_newPages.append(pg);
    }
}
int WindowView::pageCount() {

    return (d->m_flowInterface->slideCount());
}

int WindowView::currentPageIndex() {

    return (d->m_flowInterface->centerIndex());
}

WindowViewJSObject:: WindowViewJSObject(WindowView* view, QWebFrame* webFrame, const QString& objectName)
  : ControllableViewJSObject(view, webFrame, objectName)
{
    connect(view,SIGNAL(pageAdded()),this,SIGNAL(pageAdded()));

}

/*!
  \fn void WindowView::centerIndexChanged(int index);
  emitted when the center index changed
  @param index :  newly changed index
*/

/*!
  \fn void WindowView::ok(int item);
  emitted when the ok action has occured, contains window index at the time of close
  @item   : index of the  where ok action was triggred
*/

/*!
  \fn void WindowView::cancel();
  emitted when the cancel action has occured
*/

} // namespace WRT
