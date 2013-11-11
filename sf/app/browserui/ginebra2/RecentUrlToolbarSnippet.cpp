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
 * This class extends ToolbarSnippet class to hold the
 * windows view toolbar buttons
 *
 */

#include "RecentUrlToolbarSnippet.h"
#include "ToolbarChromeItem.h"
#include "ViewStack.h"
#include "GWebContentView.h"
#include "HistoryManager.h"
#include <QDebug>

namespace GVA {

    RecentUrlToolbarSnippet::RecentUrlToolbarSnippet(const QString& elementId, ChromeWidget * chrome,
                                                     const QWebElement & element)
        : DualButtonToolbarSnippet(elementId, chrome, element),
          m_action1(0)
    {      
        connect(m_chrome, SIGNAL(aspectChanged(int)) , this, SLOT(onAspectChanged()));    	
    }

    RecentUrlToolbarSnippet::~RecentUrlToolbarSnippet()
    {
        if(m_action1)
            delete m_action1;
    }

     void RecentUrlToolbarSnippet::onAspectChanged( ) {
    	 
        updateOwnerArea();      
     }     
    RecentUrlToolbarSnippet * RecentUrlToolbarSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
    {
        RecentUrlToolbarSnippet * that = new RecentUrlToolbarSnippet( elementId, chrome, element );
        that->setChromeWidget( new ToolbarChromeItem( that ) );
        return that;
    }
    
    void RecentUrlToolbarSnippet::addChild(ChromeSnippet * child) {
        WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(child);
        if (!s) {
            ToolbarActions_t* t = new ToolbarActions_t();
            if (child->elementId() == "RecentBackButton" ) {
                t->actionId = RECENTURL_VIEW_ACTION_BACK;
                t->actionName = RECENTURL_TOOLBAR_BACK;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back.png";
                t->disabledImg = "";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_pressed.png";
            }
            else if (child->elementId() == "RecentClearallButton" ) {
                t->actionId = RECENTURL_VIEW_ACTION_CLEARALL;
                t->actionName = RECENTURL_TOOLBAR_CLEARALL;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_clearall.png";
                t->disabledImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_clearall_disabled.png";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_clearall_pressed.png";
            }
            t->id = child->elementId();
            m_actionInfo.append(t);
        }

        WebChromeContainerSnippet::addChild(child);
    }

    void RecentUrlToolbarSnippet::setAction(ChromeSnippet * s) {
        ToolbarSnippet::setAction(s);

        ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
        int index = getIndex(s);

        if (index != -1 ) {
            ToolbarActions_t * t = m_actionInfo.at(index);
            QAction * action =  button->getDefaultAction();
            if (t->actionId == RECENTURL_VIEW_ACTION_BACK ) {
                if( !action ) {
                    action = new QAction(0);
                    button->setDefaultAction(action);
                    m_action1 = action;
                }
                connect(action, SIGNAL(triggered()), this, SLOT(handleBackButton()));
            }
            else if (t->actionId == RECENTURL_VIEW_ACTION_CLEARALL) {
                if( !action ) {
					// Action is created/handled/owned by HistoryManager
                    QAction * a = WRT::HistoryManager::getSingleton()->getActionClearHistory();
                    button->setDefaultAction(a);
                }
            }
        }
    }

    void RecentUrlToolbarSnippet::handleBackButton() {
        GWebContentView* webView = static_cast<GWebContentView*> (chrome()->getView("WebView"));
        if(webView)
            webView->showNormalPage();
        ViewStack::getSingleton()->switchView( TOOLBAR_WEB_VIEW, TOOLBAR_RECENTURL_VIEW );
    }

} // end of namespace GVA


