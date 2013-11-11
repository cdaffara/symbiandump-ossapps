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

#include <QtGui>

#include "mostvisitedpagestore.h"
#include "linearflowsnippet.h"
#include "mostvisitedpageview.h"
#include "webpagecontroller.h"
#include "HistoryManager.h"
#include "webpagedata.h"
#include "ExternalEventCharm.h"
#include "Utilities.h"
#include "wrtbrowsercontainer.h"
#include "wrtbrowsercontainer_p.h"

const int KLinearSnippetHeight = 120;

namespace GVA {
MostVisitedPagesWidget::MostVisitedPagesWidget(ChromeSnippet* snippet, ChromeWidget* chrome)
  : ChromeItem(snippet, chrome->layout())
  , m_parent(chrome->layout())
  , m_chrome(chrome)
  , m_flowInterface(0)
  , m_hideOnClose(true)
  , m_externalEventCharm(0)
{
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setOpacity(0.5);
    m_mostVisitedPageStore = new MostVisitedPageStore();
    WebPageController* pageController = WebPageController::getSingleton();
    connect(pageController, SIGNAL(loadFinished(const bool)), this, SLOT(onLoadFinished(const bool)));
    connect(pageController, SIGNAL(loadFinishedForBackgroundWindow(const bool, WRT::WrtBrowserContainer*)), this, SLOT(onLoadFinishedForBackgroundWindow(const bool, WRT::WrtBrowserContainer*)));
    connect(WRT::HistoryManager::getSingleton(),SIGNAL(historyCleared()),this,SLOT(clearMVStore()));
    //Registering for handling External Events
    m_externalEventCharm = new ExternalEventCharm(this);
    safe_connect(m_externalEventCharm, SIGNAL(externalMouseEvent(QEvent*, const QString &, const QString &)),
        		snippet, SIGNAL(externalMouseEvent(QEvent*, const QString &, const QString &)));
}

MostVisitedPagesWidget::~MostVisitedPagesWidget()
{
    if (m_flowInterface)
        m_flowInterface->deleteLater();
    if (m_mostVisitedPageStore)
        delete m_mostVisitedPageStore;
    if(m_externalEventCharm)
        delete m_externalEventCharm;
    WebPageController* pageController = WebPageController::getSingleton();
    disconnect(pageController, SIGNAL(loadFinished(const bool)), this, SLOT(onLoadFinished(const bool)));
    disconnect(WRT::HistoryManager::getSingleton(),SIGNAL(historyCleared()),this,SLOT(clearMVStore()));
}


void MostVisitedPagesWidget::open()
{
    if (m_flowInterface)
        delete m_flowInterface;

    m_flowInterface = new GVA::LinearFlowSnippet(this);
    m_flowInterface->setZValue(m_parent->zValue() + 1);
    m_flowInterface->resize(QSize(m_parent->size().width(), KLinearSnippetHeight));
    connect(m_flowInterface, SIGNAL(mouseEvent(QEvent::Type)),this,SIGNAL(mouseEvent(QEvent::Type)));

    //Initialize the page selection index
    m_selectIndex = -1;

    QString displayMode;

    if (m_parent->size().width() > m_parent->size().height()) {
        displayMode = "Landscape";

    } else {
        displayMode = "Portrait";
    }
    m_flowInterface->init(displayMode,qtTrId("txt_browser_most_visited_title"));

    MostVisitedPageList mvPageList = m_mostVisitedPageStore->pageList();

    for (int i = 0; i < mvPageList.size(); i++) {
        QImage *pageThumbnail = mvPageList[i]->m_pageThumbnail;
        bool removeTempThumbnail = false;
        if (!pageThumbnail) {
            removeTempThumbnail = true;
            pageThumbnail = new QImage("");
        }
        QUrl url = QUrl(mvPageList[i]->m_url);
        m_flowInterface->addSlide(*pageThumbnail, url.host());
        if (removeTempThumbnail) {
            delete pageThumbnail;
        }
    }

    setCenterIndex(displayMode);
    m_flowInterface->prepareStartAnimation();
    m_flowInterface->runStartAnimation();
    m_flowInterface->installEventFilter(this);
    connect(m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    connect(m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(closeAnimationCompleted()));
}

void MostVisitedPagesWidget::close(bool hide)
{
    //This will trigger close animation after which
    //the cleanup is done in closeAnimationCompleted
    if (m_flowInterface)
        m_flowInterface->runEndAnimation();
    m_hideOnClose = hide;
}

void MostVisitedPagesWidget::updatePos(QPointF pos, qreal &toolBarHeight)
{
    QGraphicsWidget::setPos(pos);
    m_flowInterface->setPos(pos.x(), m_parent->size().height() - (toolBarHeight+KSpaceToolbarMVP) - KLinearSnippetHeight);
}

void MostVisitedPagesWidget::resize(const QSize &size)
{
    QGraphicsWidget::resize(size);
    m_flowInterface->resize(QSize(m_parent->size().width(), KLinearSnippetHeight));
}

void MostVisitedPagesWidget::displayModeChanged(QString& newMode)
{
    m_flowInterface->displayModeChanged(newMode);
}

void MostVisitedPagesWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
    QGraphicsWidget::paint(painter, option, widget);
}

void MostVisitedPagesWidget::setCenterIndex(QString displayMode)
{
    //display beginning of the film strip first
    m_flowInterface->setCenterIndex((displayMode == "Portrait") ? 1 : 2);
}

void MostVisitedPagesWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    close();
}

void MostVisitedPagesWidget::okTriggered(int index)
{
    Q_ASSERT(m_flowInterface);
    m_selectIndex = index;
    close();
}

void MostVisitedPagesWidget::closeAnimationCompleted()
{
    if (!m_flowInterface)
        return;

    QGraphicsWidget::hide();
    disconnect(m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    disconnect(m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(closeAnimationCompleted()));
    disconnect(m_flowInterface, SIGNAL(mouseEvent(QEvent::Type)),this,SIGNAL(mouseEvent(QEvent::Type)));

    m_flowInterface->removeEventFilter(this);
    m_flowInterface->deleteLater();
    m_flowInterface = NULL;

    if (m_selectIndex != -1)
        m_chrome->loadUrlToCurrentPage(m_mostVisitedPageStore->pageAt(m_selectIndex)->pageUrl());

    if (m_snippet->isVisible() && m_hideOnClose)
        m_snippet->ChromeSnippet::toggleVisibility();

    m_hideOnClose = false;
    emit closeComplete();
}

void MostVisitedPagesWidget::updateMVGeometry()
{
    qreal toolBarHeight;

    ChromeSnippet* visibleSnippet= m_chrome->getSnippet("WebViewToolbarId");
    if (visibleSnippet)
        toolBarHeight = visibleSnippet->widget()->geometry().height();

    resize(m_parent->size().toSize());
    updatePos(QPointF(0, 0), toolBarHeight);
}

 void MostVisitedPagesWidget::updateMVStore(WRT::WrtBrowserContainer *page)
  {
    Q_ASSERT(page);
    Q_ASSERT(!page->mainFrame()->url().isEmpty());

    QUrl pageUrl = page->mainFrame()->url();
    int pageRank = 0;
    QImage* pageThumbnail = NULL;
    //check if page exists in store along with its thumbnail
    if (!m_mostVisitedPageStore->contains(pageUrl.toString(), true)) {
        qreal scale = 200.0 / page->viewportSize().width();
        QImage img = page->pageThumbnail(scale, scale);
        pageThumbnail = new QImage(img);
    }

    //if it is a new page to the store, get its rank from history
    //FIX ME : need to optimize this code
    pageRank = WRT::HistoryManager::getSingleton()->getPageRank(pageUrl.toString());
    m_mostVisitedPageStore->pageAccessed(pageUrl, pageThumbnail, pageRank);
  }

 void MostVisitedPagesWidget::onLoadFinished(const bool ok)
 {
     if (ok) {
        WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
        updateMVStore(activePage);
     }
 }
 void MostVisitedPagesWidget::onLoadFinishedForBackgroundWindow(const bool ok, WRT::WrtBrowserContainer *page)
 {
     if (ok) {
         if (page)
		     updateMVStore(page);
     }
 }

void MostVisitedPagesWidget::clearMVStore()
{
    m_mostVisitedPageStore->clearMostVisitedPageStore();
}

} // endof namespace GVA

