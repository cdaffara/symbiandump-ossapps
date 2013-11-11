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

#include "Utilities.h"
#include <QtCore/QUrl>
#include "ViewStack.h"
#include "ViewController.h"
#include "webpagecontroller.h"
#include "HistoryFlowView.h"
#include "WindowFlowView.h"
#include "ChromeSnippet.h"

namespace GVA {

static const QString KBookmarkHistoryViewName = "BookmarkHistoryView";
static const QString KBookmarkTreeViewName = "BookmarkTreeView";
static const QString KWebViewName = "WebView";
static const QString KWindowViewName = "WindowView";
static const QString KHistoryViewName = "HistoryView";
static const QString KSettingsViewName = "SettingsView";


ViewStack* ViewStack::getSingleton()
{
    static ViewStack* singleton = 0;
    if (!singleton)
    {
        singleton = new ViewStack;
        singleton->setObjectName("ViewStack");
    } // if (! singleton)

    assert(singleton);
    return singleton;
}

void ViewStack::fromWindowView(const QString &to)
{
    emit (deActivateWindowView());

    ChromeSnippet* tbSnippet = m_chrome->getSnippet("WindowViewToolbarId");
    if (tbSnippet)
        tbSnippet->hide();

    //m_chromeView->setViewofCurrentPage();

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));

    disconnect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));

    m_viewController->showContent(to);
}

void ViewStack::fromSettingsView(const QString &to)  // goto WebView
{
    m_viewController->showContent(to);
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("SettingsViewToolbarId");
    if (tbSnippet)
        tbSnippet->hide();

    WebPageController::getSingleton()->setSettingsLoaded(0);
}

void ViewStack::fromWebView(const QString &to)
{
    m_viewController->showContent(to);
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("WebViewToolbarId");
    if (tbSnippet)
        tbSnippet->hide(false);
}

void ViewStack::fromBookmarkTreeView(const QString &to)
{
     m_viewController->showContent(to);
    // Hide toolbar and dialog if visible
    ChromeSnippet* visibleSnippet = m_chrome->getSnippet("BookmarkViewToolbarId");
    if (visibleSnippet)
        visibleSnippet->hide();
    
    visibleSnippet = m_chrome->getSnippet("BookmarkDialogId");
    if (visibleSnippet)
        visibleSnippet->hide();
}


void ViewStack::fromBookmarkHistoryView(const QString &to)
{
    m_viewController->showContent(to);
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("RecentUrlViewToolbarId");
    if (tbSnippet)
        tbSnippet->hide();
    
    ChromeSnippet* visibleSnippet  = m_chrome->getSnippet("ClearHistoryDialogId");
    if (visibleSnippet)
         visibleSnippet->hide();
}

void ViewStack::toWindowView()
{
    emit(activateWindowView());

    ChromeSnippet* tbSnippet = m_chrome->getSnippet("WindowViewToolbarId");
    if (tbSnippet)
        tbSnippet->show();

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));

    safe_connect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));
}

void ViewStack::toSettingsView()
{
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("SettingsViewToolbarId");
    if (tbSnippet)
        tbSnippet->show();

    WebPageController::getSingleton()->setSettingsLoaded(0);

}

void ViewStack::toWebView()
{
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("WebViewToolbarId");
    if (tbSnippet)
        tbSnippet->show(false);
}

void ViewStack::toBookmarkHistoryView()
{
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("RecentUrlViewToolbarId");
    if (tbSnippet)
        tbSnippet->show();
}

void ViewStack::toBookmarkView()
{
    ChromeSnippet* tbSnippet = m_chrome->getSnippet("BookmarkViewToolbarId");
    if (tbSnippet)
        tbSnippet->show();

    emit activateBookmark();
}

void ViewStack::creatingPage(WRT::WrtBrowserContainer* page) {
    Q_UNUSED(page);

    //qDebug() << "VIEW STACK:: Received creating Page" << page;

    if (m_viewController->currentView()->type() == "webView" ) {
        //qDebug() << "ViewStack::page: " << page << "Created In " << m_viewController->currentView()->type() ;

        WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));
        windowView->setMode(WRT::WindowView::WindowViewModeTransition);
        windowView->onPageCreated(page);
        ViewStack::getSingleton()->switchView(KWindowViewName, KWebViewName);
    }

}

void ViewStack::switchView(const QString &to, const QString &from) {

    if (to == from) {
        return;
    }
    if (!m_viewController) {
        return;
    }

    if (from == KWindowViewName) {
        fromWindowView(to);
    }
    else if (from == KWebViewName) {
        fromWebView(to);
    }
    else if (from == KBookmarkHistoryViewName) {
        fromBookmarkHistoryView(to);
    }
    else if (from == KBookmarkTreeViewName) {
        fromBookmarkTreeView(to);
    }
    else if (from == KSettingsViewName) {
        fromSettingsView(to);
    }

    if (to == KWindowViewName) {
        toWindowView();
    }
    else if (to == KWebViewName) {
        toWebView();
    }
    else if (to == KBookmarkHistoryViewName) {
        toBookmarkHistoryView();
    }
    else if (to == KBookmarkTreeViewName) {
        toBookmarkView();
    }
    else if (to == KSettingsViewName) {
        toSettingsView();
    }
}


void ViewStack::loadHistoryItem(int item) {

    if (!m_viewController) {
        return;
    }
    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->view(KHistoryViewName));
    //var myIndex = window.viewManager.historyView.currentHistIndex;
    int myIndex = historyView->currentIndex();

    if (myIndex != item ) {
        //window.chrome.alert("loadFromHistory");
        WebPageController::getSingleton()->currentSetFromHistory(item);
        safe_connect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
}

void ViewStack::goBackFromWindowView() {
    switchView(KWebViewName, KWindowViewName);
}

void ViewStack::goBackFromRecentUrlView() {
    switchView(KWebViewName, KBookmarkHistoryViewName);
}

void ViewStack::goBackFromBookmarkView() {
    switchView(KWebViewName, KBookmarkTreeViewName);
}

void ViewStack::goBackFromSettingsView() {
    switchView(KWebViewName, KSettingsViewName);
}

void ViewStack::showContentView(int progress) {
    Q_UNUSED(progress);

    if (!m_viewController) {
        return;
    }

    if ( m_viewController->currentView()->type() == KWebViewName ) {
        disconnect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
}

}  // end of namespace

