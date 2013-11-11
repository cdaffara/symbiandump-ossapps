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
#include "HistoryView_p.h"
#include "HistoryView.h"
#include "HistoryViewEventContext.h"

#include <QWebHistory>
#include <QWebFrame>
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"
#include "scriptobjects.h"

#include <QDebug>

namespace WRT {

HistoryViewPrivate::HistoryViewPrivate(WebPageController * pageMgr,
                                       QWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(parent),
    m_graphicsWidgetParent(0),
    m_pageManager(pageMgr),
    m_activePage(0),
    m_historyIndex(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);
    init();
}

HistoryViewPrivate::HistoryViewPrivate(WebPageController * pageMgr,
                                       QGraphicsWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(0),
    m_graphicsWidgetParent(parent),
    m_pageManager(pageMgr),
    m_activePage(0),
    m_historyIndex(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);
    init();
}

HistoryViewPrivate::~HistoryViewPrivate()
{
    delete m_actionForward;
    delete m_actionBack;
    delete m_actionOK;
    delete m_actionCancel;
}

void HistoryViewPrivate::init()
{
    // create the view's actions
    m_actionForward = new QAction("Forward",m_widgetParent);
    m_actionForward->setObjectName("Forward");
    m_actionBack = new QAction("Back",m_widgetParent);
    m_actionBack->setObjectName("Back");
    m_actionOK = new QAction("OK",m_widgetParent);
    m_actionOK->setObjectName("OK");
    m_actionCancel = new QAction("Cancel",m_widgetParent);
    m_actionCancel->setObjectName("Cancel");
}

/*!
 * \class HistoryView
 *
 * \brief The base class for the HistoryViews
 *
 * This class provides the basic routines to enable history navigation.
 * Derived classes (such as HistoryFlowView, and HistoryLiteView) supply
 * the exact "FlowInterface" to be used, and rely on much of the base-class functionality
 * for signal/slots, extracting history items from the current page, etc.
 */

/*!
  Basic HistoryView constructor requires a PageManager to manage the pages
  and a parent QWidget
*/
HistoryView::HistoryView(WebPageController * pageMgr,
                         QWidget* parent) :
    d(new HistoryViewPrivate(pageMgr, parent))
{
}

/*!
  Basic HistoryView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
*/
HistoryView::HistoryView(WebPageController * pageMgr,
                         QGraphicsWidget* parent) :
    d(new HistoryViewPrivate(pageMgr, parent))
{
}


HistoryView::~HistoryView()
{
    delete d;
}

/*!
  Retrieve the WebPageController assigned to this view
*/
WebPageController* HistoryView::webPageController()
{
    return d->m_pageManager;
}

/*!
  Return the view's Forward QAction
  For scrolling the history view forwards
*/
QAction * HistoryView::getActionForward()
{
    return d->m_actionForward;
}

/*!
  Return the view's Back QAction
  For scrolling the history view backwards
*/
QAction * HistoryView::getActionBack()
{
    return d->m_actionBack;
}

/*!
  Return the view's OK QAction
  For invoking the view's OK
*/
QAction * HistoryView::getActionOK()
{
    return d->m_actionOK;
}

/*!
  Return the view's Cancel QAction
  For invoking the view's Cancel
*/
QAction * HistoryView::getActionCancel()
{
    return d->m_actionCancel;
}

/*!
  Return the widget handle of this view
*/
QGraphicsWidget* HistoryView::widget() const
{
    return d->m_flowInterface;
}

/*!
  Return the title of this view for display
*/
QString HistoryView::title() const
{
    return QString("History");
}

/*!
  Return whether this view is active or not
*/
bool HistoryView::isActive()
{
    return d->m_isActive;
}


/*!
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.
*/
QList<QAction*> HistoryView::getContext()
{
    // for now, all actions valid at all times
    // but there may be some logic here to determine context
    QList<QAction*> contextList;
    contextList <<
        d->m_actionForward <<
        d->m_actionBack <<
        d->m_actionOK <<
        d->m_actionCancel;
    return contextList;
}



/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void HistoryView::activate()
{
    Q_ASSERT(!d->m_isActive);

    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->init();
    updateHistory(true);
    
    // now forward visual flow lite's signals
    connect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), SIGNAL(centerIndexChanged(int)));
    connect(d->m_flowInterface, SIGNAL(ok(int)), this, SIGNAL(ok(int)));
    connect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));

    // internally process the index change signal as well, and page load complete
    connect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(updateActions(int)));
    connect(d->m_pageManager,SIGNAL(loadFinished(bool)),this,SLOT(updateHistory(bool)));

    // auto-link relevant actions to slots
    connect(d->m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    connect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));

    widget()->installEventFilter(this);
    connect(this, SIGNAL(contextEvent(QObject *)), jsObject(), SLOT(onContextEvent(QObject *)));

    // show and set index
    d->m_flowInterface->show();
    d->m_historyIndex = 0;
    d->m_isActive = true;
    
    emit activated();
}

/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void HistoryView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    if(!d->m_flowInterface)
        return;

    // disconnect signals
    disconnect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SIGNAL(centerIndexChanged(int)));
    disconnect(d->m_flowInterface, SIGNAL(ok(int)), this, SIGNAL(ok(int)));
    disconnect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));

    // internally process the index change signal as well
    disconnect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(updateActions(int)));
    disconnect(d->m_pageManager,SIGNAL(loadFinished(bool)),this,SLOT(updateHistory(bool)));

    // auto-link relevant actions to slots
    disconnect(d->m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    disconnect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));

    widget()->removeEventFilter(this);
    disconnect(this, SIGNAL(contextEvent(QObject *)), jsObject(), SLOT(onContextEvent(QObject *)));
    
    // cleanup
    d->m_flowInterface->deleteLater();
    d->m_flowInterface = NULL;
    d->m_isActive = false;
    
    emit deactivated();
}

void HistoryView::updateHistory(bool status)
{
    if (!status) {
        return;
    }

    if (!d->m_flowInterface) {
        return;
    }

    // clear PictureFlow
    d->m_flowInterface->clear();

    d->m_activePage = d->m_pageManager->currentPage();

    if (d->m_activePage->history()->count() < 0) {
        return;
    }

    QWebHistoryItem item = d->m_activePage->history()->currentItem();
    d->m_activePage->savePageDataToHistoryItem(d->m_activePage->mainFrame(), &item);

    QList<QWebHistoryItem> items = d->m_activePage->history()->items();
    for (int i = 0; i < d->m_activePage->history()->count(); i++) {
        QWebHistoryItem item = items.at(i);
        WebPageData data = item.userData().value<WebPageData>();
        QImage img = data.m_thumbnail;
        d->m_flowInterface->addSlide(img);
    }

    int currentItemIndex(d->m_activePage->history()->currentItemIndex());
    d->m_flowInterface->setCenterIndex(currentItemIndex);
    updateActions(currentItemIndex);
}

int HistoryView::currentIndex() {

    return (d->m_activePage->history()->currentItemIndex());
}

/*!
  set the history view's center index
*/
void HistoryView::setCenterIndex(int index)
{
    // first emit center index change
    emit centerIndexChanged(index);

    d->m_historyIndex = index - d->m_activePage->history()->currentItemIndex();

    emit titleChanged(itemTitle(d->m_historyIndex));

    //if(d->m_flowInterface)
    //{
    //   qDebug() << "Setting center index " << index;
        //d->m_flowInterface->setCurentIndex(index);
    //}

}

/*!
  scroll forward in the history view
*/
void HistoryView::forward()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showNext();
}

/*!
  scroll back in the history view
*/
void HistoryView::back()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showPrevious();
}

void HistoryView::updateActions(int centerIndex)
{
    d->m_actionBack->setEnabled(centerIndex != 0);
    d->m_actionForward->setEnabled(centerIndex < (d->m_activePage->history()->count() - 1));
}

QRect HistoryView::centralRect()
{
    if(!d->m_flowInterface)
        return QRect();

    return d->m_flowInterface->centralRect();
}

bool HistoryView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu && obj == widget()) {
        return handleWidgetContextMenuEvent(static_cast<QContextMenuEvent *>(event));
    }

    // standard event processing
    return ControllableViewBase::eventFilter(obj, event);
}

bool HistoryView::handleWidgetContextMenuEvent(QContextMenuEvent *event)
{
//        WRT::HistoryViewEventContext *context = 
//            new WRT::HistoryViewEventContext(type(), 
//                                             cmEvent->pos(), 
//                                             d->m_flowInterface->centerIndex(), 
//                                             itemTitle(d->m_flowInterface->centerIndex()));

    // Create a javascript-accessible object with properties containing the required info.
    QObject *context = new QObject();
    context->setObjectName("historyContextEvent");
    context->setProperty("viewType", type());
    context->setProperty("itemIndex", d->m_flowInterface->centerIndex());
    context->setProperty("itemTitle", itemTitle(d->m_flowInterface->centerIndex()));

    // Create an object representing the position of the event.  Make it a child of the context object
    // so it can be accessed from javascript.
    new ScriptPoint(context, event->pos(), "pos");
    
    // Pass ownership to the javascript engine.
    QWebFrame *chrome = chromeFrame();
    if(chrome) {
        chrome->addToJavaScriptWindowObject(context->objectName(), context, QScriptEngine::ScriptOwnership);
    }

    emit contextEvent(context);
            
    return true;
}

QString HistoryView::itemTitle(int index) const
{
    QWebHistoryItem item = d->m_activePage->history()->itemAt(d->m_flowInterface->centerIndex());
    QString title;
    if (item.isValid()) {
        title = item.title();
        if (title.isNull() || title.isEmpty()) {
            title = item.url().toString();
        }
    }
    return title;
}

// -------------------------------------------

HistoryViewJSObject::HistoryViewJSObject(HistoryView* view, QWebFrame* webFrame, const QString& objectName)
  : ControllableViewJSObject(view, webFrame, objectName)
{
    connect(view,SIGNAL(ok(int)),this,SLOT(ok(int)));
    connect(view,SIGNAL(cancel()),this,SLOT(cancel()));
    
    connect(view,SIGNAL(activated()),this,SIGNAL(activated()));
    connect(view,SIGNAL(deactivated()),this,SIGNAL(deactivated()));
}

HistoryViewJSObject::~HistoryViewJSObject()
{
    disconnect(static_cast<HistoryView*>(m_contentView),SIGNAL(ok(int)),this,SLOT(ok(int)));
    disconnect(static_cast<HistoryView*>(m_contentView),SIGNAL(cancel()),this,SLOT(cancel()));
    
    disconnect(static_cast<HistoryView*>(m_contentView),SIGNAL(activated()),this,SIGNAL(activated()));
    disconnect(static_cast<HistoryView*>(m_contentView),SIGNAL(deactivated()),this,SIGNAL(deactivated()));
}

void HistoryViewJSObject::ok(int item)
{
    emit done(item);
}

void HistoryViewJSObject::cancel()
{
    emit done(historyView()->currentIndex());
}

void HistoryViewJSObject::onContextEvent(QObject *context){
    emit contextEvent(context);
}

/*!
  \fn void HistoryView::centerIndexChanged(int index);
  emitted when the center index changed
*/

/*!
  \fn void HistoryView::ok(int item);
  emitted when the ok action has occured, contains history index at the time of close
*/

/*!
  \fn void HistoryView::cancel();
  emitted when the cancel action has occured
*/

} // namespace WRT
